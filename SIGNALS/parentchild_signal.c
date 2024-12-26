#include<stdio.h>
#include<signal.h>
#include<fcntl.h>
#include<unistd.h>

void ptoc()
{
   printf("child received form parent\n");
}

void ctop()
{
   printf("parent received form child\n");
}

int main()
{
  int c=0;
  signal(SIGUSR1,ptoc);
  signal(SIGUSR2,ctop);
  c=fork();
  
  if(c+>0)
  {
       while(1) kill(c,SIGUSR1);
  }
  else
  {
       while(1) kill(getppid(),SIGUSR2);
  }
  return 0;
}
