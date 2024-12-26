#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>

int main()
{
    char* fifo="user2_fifo";
    mkfifo(fifo,0666);
    int fd=open(fifo,0666);
    //while(1)
    //{
        write(fd,"Hi from user2",13);
        printf("Hi from user2\n");
        //sleep(10);
        //fd=open(fifo,O_RDONLY);
        //while(1)
        //{
        //char buffer[13];
        //int r=read(fd,buffer,13);
        //if(r<=0)break;
        //printf("              %s \n",buffer);
        //}
        sleep(10);
        fd=open(fifo,O_RDONLY);
        while(1)
        {
        char buffer[1024];
        int r=read(fd,buffer,1024);
        if(r<=0)break;
        printf("              %s \n",buffer);
        
        }
    //}
   return 0;
}
