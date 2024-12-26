#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>

void ptoc()
{
    printf("Child received signal sent by Parent\n");
}

void ctop()
{
    printf("Parent received signal sent by Child\n");
}

int main()
{
    signal(SIGUSR1,ptoc);
    signal(SIGUSR2,ctop);
    int c=0;
    c=fork();
    if(c>0)
    {
       while(1) kill(c,SIGUSR1);
    }
    else
    {
       while(1) kill(getppid(),SIGUSR2);
    }
    return 0;
}
