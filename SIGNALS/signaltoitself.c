#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>

void sig_function()
{
    printf("Raised a signal\n");
}

int main()
{
    signal(SIGUSR1,sig_function);
    raise(SIGUSR1);
    return 0;
}
