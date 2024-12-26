#include<stdio.h>
#include<signal.h>
#include<fcntl.h>
#include<unistd.h>


void sig_fn()
{
    printf("alarm raised\n");
}

int main()
{
    signal(SIGALRM,sig_fn);
    alarm(5);
    pause();
    return 0;
}
