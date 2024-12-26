// process1.c
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>  // For O_CREAT, O_EXCL
#include <unistd.h>

int main() {
    // Open or create a named semaphore with an initial value of 0
    sem_unlink("/example_semaphore");
    sem_t *sem = sem_open("/example_semaphore", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
 
    for(int i=0;i<5;i++)
   {   
       sem_wait(sem);
       printf("Process 1: Starting critical section...\n");
       sleep(2);  // Simulate work in the critical section
       printf("Process 1: Critical section done.\n");

       // Post (signal) to the semaphore to let process 2 proceed
      sem_post(sem);
      printf("Process 1: Semaphore signaled.\n");

      sleep(10);
   }
   // Close the semaphore
      sem_close(sem);
    return 0;
}

