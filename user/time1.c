#include "kernel/types.h"
#include "user/user.h"
#include "pstat.h"

int main(int argc, char *argv[]) {
    int start, end, elapsed;
    int pid, status;
    struct rusage ru;

    if (argc < 2) {
        fprintf(2, "Usage: time1 <command> [args...]\n");
        exit(1);
    }

    start = uptime();
    pid = fork();

    if (pid == 0) {
        // Child process
        exec(argv[1], &argv[1]);
        fprintf(2, "exec %s failed\n", argv[1]);
        exit(1);
    } else if (pid > 0) {
        // Parent process
        wait2(&status, &ru);
        end = uptime();
        elapsed = end - start;

        int cpu_percent = 0;
        if (elapsed > 0) {
            cpu_percent = (ru.cputime * 100) / elapsed;
        }

        printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n", 
               elapsed, ru.cputime, cpu_percent);
        exit(0);
    } else {
        fprintf(2, "fork failed\n");
        exit(1);
    }
}
