// process2.c
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>  // For O_CREAT, O_EXCL
#include <unistd.h>

int main() {
    // Open the named semaphore created by process 1
    sem_t *sem = sem_open("/example_semaphore", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    printf("Process 2: Waiting for semaphore...\n");

    for(int i=0;i<5;i++){   // Wait (block) until process 1 signals the semaphore
    sem_wait(sem);

    printf("Process 2: Entering critical section.\n");
    sleep(2);  // Simulate work in the critical section
    printf("Process 2: Critical section done.\n");
    sem_post(sem);
    sleep(10);
}
    // Close the semaphore
    sem_close(sem);

    // Remove the semaphore from the system
    sem_unlink("/example_semaphore");

    return 0;
}

