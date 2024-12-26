#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<poll.h>

int Read_Print(int fd,char* buffer)
{
      //int fd=open(fifo,O_RDONLY);
      int bytes_read;
      //while(1)
      //{
           bytes_read=read(fd,buffer,1024);
           //if(bytes_read<=0)break;
           for(int i=0;i<bytes_read;i++)
           {
              printf("%c",buffer[i]);
           }
           printf("\n");
           //write(o_fd,buffer,bytes_read);
      //}
      printf("\n");
      return bytes_read;
}

int main()
{
    char* fifo1="user1_fifo";
    mkfifo(fifo1,0666);
    int fd1=open(fifo1,O_RDONLY|O_NONBLOCK);
    char* fifo2="user2_fifo";
    mkfifo(fifo2,0666);
    int fd2=open(fifo2,O_RDONLY|O_NONBLOCK);
    struct pollfd fds[2];
    int timeout=20000;
    int ret;
    nfds_t nfd=2;
    fds[0].fd=fd1;
    fds[0].events=POLLIN;   
    fds[1].fd=fd2;
    fds[1].events=POLLIN; 
    //while(1)
    //{
         ret=poll(fds,nfd,timeout);
         printf("ret= %d \n",ret);
         char buffer[2][1024];
         int arr[2]={0,0};
         for(int i=0;i<2;i++)
         {
             if((fds[i].revents & POLLIN))
             {
               printf("for i = %d \n",i);
               arr[i]=Read_Print(fds[i].fd,buffer[i]);
               for(int j=0;j<arr[i];j++)printf("%c",buffer[i][j]);
               printf("\n");
             }
         }
         fd1=open(fifo1,O_WRONLY);
         write(fd1,buffer[1],arr[1]);
         fd2=open(fifo2,O_WRONLY);
         write(fd2,buffer[0],arr[0]);
         
    //}
    return 0;
}
