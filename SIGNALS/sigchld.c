#include<stdio.h>
#include<signal.h>
#include<fcntl.h>
#include<unistd.h>

void chld()
{
    printf("child died\n");
}


int main()
{
  signal(SIGCHLD,chld);
  int c=0;
  c=fork();
  if(c>0)
  {
    wait();
    printf("parent process started\n");
  }
  else
  {
     printf("about to die\n");
   }
  return 0;
}
