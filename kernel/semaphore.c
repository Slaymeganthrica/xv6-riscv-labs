#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "spinlock.h"

struct semtab semtable ;

void seminit ( void )
{
	initlock (& semtable . lock , " semtable ") ;

	for ( int i = 0; i < NSEM ; i ++){
		initlock (& semtable . sem [ i ]. lock , " sem ") ;
	}
}

int semalloc(void){
  acquire(&semtable.lock);

  for(int i = 0; i < NSEM; i++){
    if(semtable.sem[i].valid == 0){
      semtable.sem[i].valid = 1;
      semtable.sem[i].count = 0;  //will be set by sys_sem_init
      release(&semtable.lock);
      return i;
    }
  }

release(&semtable.lock);
return -1; //no free semaphore
}

void semdealloc(int id){
  acquire(&semtable.lock);

  semtable.sem[id].valid = 0;
  semtable.sem[id].count = 0; //safe reset

  release(&semtable.lock);
}





