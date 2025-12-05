#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;
  struct proc *p = myproc();

  if(argint(0, &n) < 0)
    return -1;

  addr = p->sz;
  if (n == 0)
    return addr;

  uint64 new_sz = addr + n;
  if(new_sz < p->sz){
    return (uint64)-1;
  }
  p->sz = new_sz;
  /*old eager allocatoin, we don't call growproc right away for lazy allocatoin*/
  /*if(growproc(n) < 0)
    return -1;*/
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
    return -1;
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

uint64
sys_freepmem(void)
{
  uint64 pages = kfreepages_count();
  return pages * PGSIZE;
}

//--semaphore system calls--

// read a semaphore ID from user memory and return the kernel semaphore pointer.
static struct semaphore*
get_sem(uint64 uaddr)
{
  struct proc *p = myproc();
  int id;

  // copy integer (sem_t) from user space → kernel variable
  if(copyin(p->pagetable, (char *)&id, uaddr, sizeof(id)) < 0)
    return 0;

  // check bounds
  if(id < 0 || id >= NSEM)
    return 0;

  // check if this semaphore is actually valid
  if(semtable.sem[id].valid == 0)
    return 0;

  return &semtable.sem[id];
}


// sem_init(sem_t *sem, int pshared, unsigned int value)

uint64
sys_sem_init(void)
{
  struct proc *p = myproc();
  uint64 uaddr;     // user pointer to sem_t
  int pshared;      // ignored
  int value;        // initial semaphore count
  int id;           // index in semtable

  // read arguments from user space
  argaddr(0, &uaddr);
  argint(1, &pshared);
  argint(2, &value);

  // allocate a new semaphore
  id = semalloc();
  if(id < 0)
    return -1;

  // initialize the kernel semaphore entry
  acquire(&semtable.sem[id].lock);
  semtable.sem[id].count = value;
  release(&semtable.sem[id].lock);

  // write the semaphore ID back to user space
  if(copyout(p->pagetable, uaddr, (char *)&id, sizeof(id)) < 0){
    semdealloc(id);
    return -1;
  }

  return 0;
}

// sem_wait(sem_t *sem) — block until count > 0
uint64
sys_sem_wait(void)
{
  uint64 uaddr;
  struct semaphore *s;

  argaddr(0, &uaddr);
  s = get_sem(uaddr);
  if(s == 0)
    return -1;

  acquire(&s->lock);

  // if count is 0, sleep until someone posts
  while(s->count == 0){
    sleep(s, &s->lock);
  }

  // a resource is now available
  s->count--;

  release(&s->lock);
  return 0;
}

// sem_post(sem_t *sem) — increment count, wake waiters
uint64
sys_sem_post(void)
{
  uint64 uaddr;
  struct semaphore *s;

  argaddr(0, &uaddr);
  s = get_sem(uaddr);
  if(s == 0)
    return -1;

  acquire(&s->lock);
  s->count++;
  wakeup(s);   // wake sleeping threads
  release(&s->lock);

  return 0;
}

// sem_destroy(sem_t *sem) — mark unused
uint64
sys_sem_destroy(void)
{
  struct proc *p = myproc();
  uint64 uaddr;
  int id;

  argaddr(0, &uaddr);

  // read integer from user
  if(copyin(p->pagetable, (char *)&id, uaddr, sizeof(id)) < 0)
    return -1;

  if(id < 0 || id >= NSEM)
    return -1;

  // cannot destroy invalid semaphore
  if(semtable.sem[id].valid == 0)
    return -1;

  semdealloc(id);
  return 0;
}
