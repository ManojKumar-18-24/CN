#include<stdio.h>
#include<signal.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>


/*
struct sigaction {
    void (*sa_handler)(int);          // Signal handler function
    void (*sa_sigaction)(int, siginfo_t *, void *); // Alternate handler for more info
    sigset_t sa_mask;                 // Set of signals to be blocked during the execution of the handler
    int sa_flags;                     // Flags to modify behavior (e.g., SA_RESTART, SA_SIGINFO)
    void (*sa_restorer)(void);        // Legacy field (often set to NULL)
};
*/

void sig_action()
{
    printf("Received Ctrl+C siganl\n");
}

int main()
{
  struct sigaction sa;
  sa.sa_handler=sig_action;
  sa.sa_flags=0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT,&sa,NULL);
  while(1)
  {
     ;
  }
  return 0;
}
