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

struct message{
      long msg_type;
      int port;
};

int msgid;
int co_server(int PORT)
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
	else{ printf("\n bind successful ");
         cout<<"\n"<<PORT<<" was created\n";
	listen(sfd,10);
	return sfd;
	}
	return -1;
}

int get_usfd_client(int port)
{
        int usfd;
	struct sockaddr_un userv_addr;
  	int userv_len,ucli_len;

  	usfd = socket(AF_UNIX, SOCK_STREAM, 0);

  	if(usfd==-1)
  	perror("\nsocket  ");

  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, to_string(port).c_str());

	userv_len = sizeof(userv_addr);

	if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len)==-1)
	perror("\n connect ");

	else printf("\nconnect succesful");
	return usfd;
}

int send_fd(int socket, int fd_to_send)
{
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  /* storage space needed for an ancillary element with a paylod of length is CMSG_SPACE(sizeof(length)) */
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  int available_ancillary_element_buffer_space;

  /* at least one vector of one byte must be sent */
  message_buffer[0] = 'F';
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;

  /* initialize socket message */
  memset(&socket_message, 0, sizeof(struct msghdr));
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
  memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = available_ancillary_element_buffer_space;

  /* initialize a single ancillary data element for fd passing */
  control_message = CMSG_FIRSTHDR(&socket_message);
  control_message->cmsg_level = SOL_SOCKET;
  control_message->cmsg_type = SCM_RIGHTS;
  control_message->cmsg_len = CMSG_LEN(sizeof(int));
  *((int *) CMSG_DATA(control_message)) = fd_to_send;

  return sendmsg(socket, &socket_message, 0);
}

int main()
{
    struct pollfd fds[100];
    int ports[100];
    key_t key;
    key=ftok("queue",65);
    msgid=msgget(key,0666|IPC_CREAT);
    message msg;
    int size=0;
    set<int> rem_ports;
    while(1)
    {
         int ret=msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),2,IPC_NOWAIT);
         if(ret!=-1  && rem_ports.find(msg.port)==rem_ports.end())
         {
               int fd=co_server(msg.port);
               if(fd!=-1){
               fds[size].fd=fd;
               fds[size].events=POLLIN;
               ports[size]=msg.port;
               size++;
               cout<<"\nmsg_received\n";
               rem_ports.insert(msg.port);}
         }
         poll(fds,size,2000);
         for(int i=0;i<size;i++)
         {
              if(fds[i].revents & POLLIN)
              {
                  cout<<ports[i]<<" got polled\n";
                  struct sockaddr_in cli_addr;
                  socklen_t cli_len=sizeof(cli_addr);
                  int nsfd=accept(fds[i].fd,(struct sockaddr*)&cli_addr,&cli_len);
                  int usfd=get_usfd_client(ports[i]);
                  send_fd(usfd,nsfd);
                  cout<<"fd sent\n";
                  close(nsfd);
              }
         }
          sleep(1);
    }
     return 0;
}
