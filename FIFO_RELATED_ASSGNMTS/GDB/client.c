#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>

int main()
{
  char* fifo="gdb-fifo";
  mkfifo(fifo,0666);
  int fd=open("p.cpp",O_RDONLY);
  off_t file_size=lseek(fd,0,SEEK_END);
  if(file_size==-1)
  {
    perror("Error while reading p.cpp\n");
    close(fd);
    return 1;
  }
  lseek(fd,0,SEEK_SET);
  char buffer[file_size+1];
  ssize_t bytes_read=read(fd,buffer,file_size);
  buffer[bytes_read]='\0';
  int fifo_fd=open(fifo,O_WRONLY);
  write(fifo_fd,buffer,file_size);
  close(fd);
  close(fifo_fd);
  return 0;
}
