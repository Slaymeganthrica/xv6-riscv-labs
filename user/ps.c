/*#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

int main(int argc, char **argv)
{
    struct pstat uproc[NPROC];
    int nprocs;
    int i;
    char *state;
    static char *states[] = {
        [SLEEPING] "sleeping",
        [RUNNABLE] "runnable",
        [RUNNING] "running ",
        [ZOMBIE] "zombie  "};

    nprocs = getprocs(uproc);
    if (nprocs < 0)
        exit(-1);

    printf("pid\tstate\t\tpriority\tsize\tppid\tname\n");
    for (i = 0; i < nprocs; i++)
    {
        state = states[uproc[i].state];
        printf("%d\t%s\t%d\t\t%d\t%d\t%s\n", uproc[i].pid, state,
               uproc[i].size, uproc[i].ppid, uproc[i].name);
    }

    exit(0);
}
*/
#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

int main(int argc, char **argv)
{
    struct pstat uproc[NPROC];
    int nprocs;
    int i;
    char *state;
    uint current_ticks;

    nprocs = getprocs(uproc);
    if (nprocs < 0)
        exit(-1);

    current_ticks = uptime();	//get current time for age calculation

    printf("pid\tstate\t\tsize\tppid\tpriority\tage\tname\n");
    for (i = 0; i < nprocs; i++)
    {
        switch(uproc[i].state) {
            case SLEEPING: state = "sleeping"; break;
            case RUNNABLE: state = "runnable"; break;
            case RUNNING: state = "running "; break;
            case ZOMBIE: state = "zombie  "; break;
            default: state = "unknown "; break;
        }

	if(uproc[i].state == RUNNABLE){
	  uint age = current_ticks - uproc[i].readytime;
        printf("%d\t%s\t%d\t%d\t%d\t\t%d\t%s\n", 
               uproc[i].pid, state, uproc[i].size, uproc[i].ppid,
               uproc[i].priority, age, uproc[i].name);
	} else{
	   printf("%d\t%s\t%d\t%d\t%d\t\tN/A\t%s\n",
		  uproc[i].pid, state, uproc[i].size, uproc[i].ppid,
		  uproc[i].priority, uproc[i].name); 
	}
	/*uint age = current_ticks - uproc[i].readytime;
	printf("%d\t%s\t%d\t%d\t%d\t\t%d\t(readytime=%d)\t%s\n",
		uproc[i].pid, state, uproc[i].size, uproc[i].ppid,
		uproc[i].priority, age, uproc[i].readytime, uproc[i].name);*/
    }

    exit(0);
}
