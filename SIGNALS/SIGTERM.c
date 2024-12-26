#include<stdio.h>
#include<signal.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>

void hi()
{
    int i=0;
    while(i<5){printf("\nI wont terminate\n");i++;}
   signal(SIGTERM,SIG_DFL);
    kill(11023,SIGTERM);
}

int main()
{
    signal(SIGTERM,hi);
    kill(11023,SIGTERM);
    return 0;
}
