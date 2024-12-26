#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<poll.h>

int main()
{
  struct pollfd fds[1];
  int timeout=5000;
  int ret;
  nfds_t nfd=1;
  fds[0].fd=0;
  fds[0].events=POLLIN;
  //printf("waiting 5 seconds \n");
  ret=poll(fds,nfd,timeout);
  //printf("ret=%d\n",ret);
  if(ret!=0)
  {
     if(fds[0].revents & POLLIN)
     {
        printf("fds[0] read successfully\n");
     }
  }
  return 0;
}
