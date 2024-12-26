#include<stdio.h>
#include<signal.h>
#include<fcntl.h>
#include<unistd.h>

void sig_fn()
{
    printf("you raised ur self/n");
}

int main()
{
  signal(SIGUSR1,sig_fn);
  raise(SIGUSR1);
  return 0;
}
