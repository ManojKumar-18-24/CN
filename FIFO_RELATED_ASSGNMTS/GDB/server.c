#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>

void Create_Tempfile()
{
    char* fifo="gdb-fifo";
    mkfifo(fifo,0666);
    int fifo_fd=open(fifo,O_RDONLY);
    int bytes_read;
    char buffer[1024];
    int fd=open("p_server.cpp",O_RDONLY|O_CREAT|O_WRONLY,0664);
    while((bytes_read=read(fifo_fd,buffer,1024))>0)
    {
       write(fd,buffer,bytes_read);
    }
}

int CompareFiles()
{
   int fd1=open("output.txt",O_RDONLY,0664);
   int fd2=open("server_output.txt",O_RDONLY,0664);
   int bytes_read1,bytes_read2;
   
   while(1)
   {
       char buffer1[1024],buffer2[1024];
       bytes_read1=read(fd1,buffer1,1024);
       bytes_read2=read(fd2,buffer2,1024);
       printf("%d & %d\n",bytes_read1,bytes_read2);
       printf("buffer1= %s \n",buffer1);
       printf("buffer2= %s \n",buffer2);
       if(bytes_read1!=bytes_read2)return 0;
       if(bytes_read1<=0)return 1;
       for(int i=0;i<bytes_read1;i++)
       {
          if(buffer1[i]!=buffer2[i])
          {
             close(fd1);
             close(fd2);
             return 0;
          }
       }
   }
   close(fd1);
   close(fd2);
   return 1;
}

int main()
{
    Create_Tempfile();
    system("g++ p_server.cpp -o p_serv");
    int c=0;
    c=fork();
    if(c>0)
    {
        wait();
        if(CompareFiles())printf("success\n");
        else printf("poindi \n");
    }
    else
    {
         int input_fd=open("input.txt",O_RDONLY);
         int output_fd=open("server_output.txt",O_RDONLY|O_CREAT|O_WRONLY,0664);
         dup2(input_fd,0);
         dup2(output_fd,1);
         execl("p_serv","./p_serv",(char*)NULL);
         close(input_fd);
         close(output_fd);
    }
    return 0;
}
