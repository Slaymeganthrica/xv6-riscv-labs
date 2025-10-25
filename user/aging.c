#include "kernel/types.h"
#include "user/user.h"

// CPU-intensive function
static void hog(void) { 
    volatile int x = 0; 
    for(;;) x++; 
}

int main(void) {
    printf("Starting aging test...\n");
    
    // Set parent to high priority
    setpriority(49);
    
    // Fork a high-priority CPU hog (inherits priority 49)
    int hogpid = fork();
    if (hogpid == 0) { 
        printf("High-priority hog started (pid: %d, priority: 49)\n", getpid());
        hog(); 
        exit(0); 
    }

    // Set parent to low priority for next child
    setpriority(0);
    int start = uptime();
    
    // Fork a low-priority child (inherits priority 0)
    int lowpid = fork();
    if (lowpid == 0) {
        int t = uptime();
        printf("[LOW] first scheduled at tick %d (wait = %d ticks, base priority = 0)\n",
               t, t - start);
        exit(0);
    }

    printf("Parent waiting for low-priority child...\n");
    
    // Wait for the low child to run (thanks to aging)
    wait(0);
    printf("Low-priority child completed!\n");

    // Cleanup hog process
    kill(hogpid);
    wait(0);
    
    printf("Aging test completed successfully!\n");
    exit(0);
}
