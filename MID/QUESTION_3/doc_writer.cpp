#include<bits/stdc++.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include<poll.h>
#include <unistd.h>
using namespace std;

int main()
{
    char fifo[]="news_fifo";
    mkfifo(fifo,0666);
    int fd2=open("document.txt",O_CREAT|O_RDWR|O_TRUNC,0666);
    while(1)
    {
          int fd=open(fifo,O_RDONLY);
          char buf[50];
          cout<<"stopped before reading\n";
          read(fd,buf,50);
          cout<<buf;
          write(fd2,buf,strlen(buf)); 
          sleep(2);
    }
    return 0;
} 

