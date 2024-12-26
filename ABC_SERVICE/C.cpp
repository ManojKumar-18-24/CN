#include<bits/stdc++.h>
#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<poll.h>
using namespace std;
#define ADDRESS  "/tmp/B_to_C"

int msgid;
vector<string> services={"./S1","./S2","./S3","./S4"};

struct message{
   long msg_type;
   int index;
};

int Get_ussfd()  /*server_side*/
{
	int  usfd;
	struct sockaddr_un userv_addr;
  	int userv_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket allabadia");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);
        return usfd;
}

void Service(int &sfd)
{
      message msg;
      msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),3,0);
      for(int i=0;i<7;i++)
      {
              struct sockaddr_in address;
              socklen_t len=sizeof(address);
              int nsfd=accept(sfd,(struct sockaddr*)&address,&len);
              if(nsfd<0)cout<<"failed\n";
              int d=0;
              d=fork();
              if(d==0)
              {   
                     cout<<"in service provider C\n";
                     dup2(nsfd,1);
                     dup2(nsfd,0);
                     execl(services[msg.index].c_str(),services[msg.index].c_str(),"C",(char*)NULL);
              }
              close(nsfd);
      }
      msg.msg_type=5;
      msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
      return;
}

int recv_fd(int socket)
{
  int sent_fd, available_ancillary_element_buffer_space;
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

  /* start clean */
  memset(&socket_message, 0, sizeof(struct msghdr));
  memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

  /* setup a place to fill in message contents */
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

  if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
   return -1;

  if(message_buffer[0] != 'F')
  {
   /* this did not originate from the above function */
   return -1;
  }

  if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
  {
   /* we did not provide enough space for the ancillary element array */
   return -1;
  }

  /* iterate ancillary elements */
   for(control_message = CMSG_FIRSTHDR(&socket_message);
       control_message != NULL;
       control_message = CMSG_NXTHDR(&socket_message, control_message))
  {
   if( (control_message->cmsg_level == SOL_SOCKET) &&
       (control_message->cmsg_type == SCM_RIGHTS) )
   {
    sent_fd = *((int *) CMSG_DATA(control_message));
    return sent_fd;
   }
  }

  return -1;
}

int main()
{
       key_t key;
       key=ftok("chat",65);
       msgid=msgget(key,0666|IPC_CREAT);
       for(int i=0;i<4;i++)
       {
            int usfd=Get_ussfd();
            struct pollfd fds[1];
            fds[0].fd=usfd;
            fds[0].events=POLLIN;
            nfds_t nfs=1;
            struct sockaddr_un ucli_addr;
            socklen_t ucli_len=sizeof(ucli_addr);
            while(1)
            {
                 
                 poll(fds,nfs,1000);
                 if(fds[0].revents & POLLIN)
                 {
                     int nusfd;cout<<"polled\n";
	             nusfd=accept(usfd, (struct sockaddr *)&ucli_addr, &ucli_len);
	             int fd=recv_fd(nusfd);
	             Service(fd);
	             break;
                 }
             }    
       }
       return 0;
}
