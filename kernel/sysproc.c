#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Stores a trace mask in the current process's metadata
uint64
sys_trace(void)
{
  int mask;
  struct proc *proc;

  argint(0, &mask);
  proc = myproc();

  if (proc == 0)
  {
    return -1;
  }
  else
  {
    proc->tracemask = mask;
    return 0;
  }
}

extern uint64 countfree(void);
extern uint64 countproc(void);

uint64
sys_sysinfo(void)
{
  uint64 addr;      // Address of the sysinfo struct, passed as an argument
  struct proc *p;   // Pointer to the current process
  uint64 freemem;   // Amount of free memory available
  uint64 nproc;     // Number of processes currently in use

  if ((p = myproc()) == 0)
    return -1;

  argaddr(0, &addr);

  // Copy out the freemem and nproc values to the userspace sysinfo struct
  if (copyout(p->pagetable, addr, (char*)&freemem, sizeof(uint64)) < 0)
    return -1;
  if (copyout(p->pagetable, addr + sizeof(uint64), (char*)&nproc, sizeof(uint64)) < 0)
    return -1;

  return 0;
}
