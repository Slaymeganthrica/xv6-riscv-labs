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

    nprocs = getprocs(uproc);
    if (nprocs < 0)
        exit(-1);

    printf("pid\tstate\t\tpriority\tsize\tppid\tname\n");
    for (i = 0; i < nprocs; i++)
    {
        switch(uproc[i].state) {
            case SLEEPING: state = "sleeping"; break;
            case RUNNABLE: state = "runnable"; break;
            case RUNNING: state = "running "; break;
            case ZOMBIE: state = "zombie  "; break;
            default: state = "unknown "; break;
        }
        printf("%d\t%s\t%d\t\t%d\t%d\t%s\n", 
               uproc[i].pid, state, uproc[i].priority,
               uproc[i].size, uproc[i].ppid, uproc[i].name);
    }

    exit(0);
}
