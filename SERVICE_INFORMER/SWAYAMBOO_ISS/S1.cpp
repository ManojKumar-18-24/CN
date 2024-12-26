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
#include<poll.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
using namespace std;
#define PORT 12334

struct message{
      long msg_type;
      int port;
};

int msgid;
int co_server()
{
        int sfd;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;

	if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	int opt=1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("\n bind successful ");

	listen(sfd,10);

	cli_len=sizeof(cli_addr);
         struct pollfd fds[1];
         fds[0].fd=sfd;
         fds[0].events=POLLIN;
         while(1)
         {
              poll(fds,1,2000);
              if(fds[0].revents & POLLIN)
              {
                   int nsfd=accept(sfd,(struct sockaddr*)&cli_addr,&cli_len);
                   close(sfd);
                   cout<<"nsfd_got\n";
                   return nsfd;
              }
         }
	return sfd;
}

int get_usfd_server()
{
        int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, to_string(PORT).c_str());
	unlink(to_string(PORT).c_str());
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);
	return usfd;
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

void Service(int &nsfd)
{
         send(nsfd,"You_got_serviced_from_S1\0",25,0);
}

int main()
{
    int usfd=get_usfd_server();
    int nsfd=co_server();
    Service(nsfd);
    close(nsfd);
    key_t key;
    key=ftok("queue",65);
    msgid=msgget(key,0666|IPC_CREAT);
    message msg;
    msg.msg_type=1;
    msg.port=PORT;
    msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
    struct pollfd fds[1];
    fds[0].fd=usfd;
    fds[0].events=POLLIN;
    while(1)
    {
        poll(fds,1,2000);
        if(fds[0].revents & POLLIN)
        {
              struct sockaddr_in cli_addr;
              socklen_t cli_len=sizeof(cli_addr);
              int nusfd=accept(fds[0].fd,(struct sockaddr*)&cli_addr,&cli_len);
              int nsfd=recv_fd(nusfd);
              cout<<"\nservicing\n";
              Service(nsfd);
              close(nsfd);
        }
         sleep(1);
    }
    return 0;
}
