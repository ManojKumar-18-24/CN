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
#define PORT 30000
#define ADDRESS1 "judge"
#define ADDRESS2 "prize"

int co_server(int port)
{
    	int sfd;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;

	if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    
	int opt=1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("\n bind successful ");

	listen(sfd,10);
	cli_len=sizeof(cli_addr);
	int nsfd;
	if((nsfd = accept(sfd , (struct sockaddr *)&cli_addr , &cli_len))==-1)
	perror("\n accept ");
	else printf("\n accept successful");
	return nsfd;
}

int get_peer_name(int &new_socket)
{
   struct sockaddr_in peer;
   int peer_len;
   
   peer_len = sizeof(peer);
     
   if (getpeername(new_socket, (struct sockaddr*)&peer, (socklen_t*)&peer_len) == -1) {
      perror("getpeername() failed");
      return -1;
   }

      /* Print it.    */
   //printf("Peer's IP address is: %s\n", inet_ntoa(peer.sin_addr));
   //printf("Peer's port is: %d\n", (int) ntohs(peer.sin_port));
   return (int) ntohs(peer.sin_port);
}

int get_usfd_client(string s)
{
   	int usfd;
	struct sockaddr_un userv_addr;
  	int userv_len,ucli_len;

  	usfd = socket(AF_UNIX, SOCK_STREAM, 0);

  	if(usfd==-1)
  	perror("\nsocket  ");

  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, s.c_str());

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
     srand(time(0));
      int jusfd=get_usfd_client(ADDRESS1);
     int pusfd=get_usfd_client(ADDRESS2);
     vector<int> nsfd(3);
     for(int i=0;i<3;i++)
     {
         nsfd[i]=co_server(PORT+i);
         cout<<"\n"<<get_peer_name(nsfd[i])<<endl;
         for(int j=0;j<3;j++){int r=rand()%100;send(nsfd[i],&r,sizeof(int),0);}
    }
    cout<<"done";
    for(int i=0;i<3;i++)
    {
              send_fd(jusfd,nsfd[i]);
              cout<<"\n"<<get_peer_name(nsfd[i])<<endl;
              send_fd(pusfd,nsfd[i]);
    }
    sleep(10);
    struct pollfd fds[3];
    for(int i=0;i<3;i++)
    {
             fds[i].fd=nsfd[i];
             fds[i].events=POLLIN;
    }
    sleep(1);
    //while(1)
    //{
              poll(fds,3,2000);
              for(int i=0;i<3;i++)
              {
                         if(fds[i].revents & POLLIN)
                         {
                               cout<<i<<" got polled\n";
                               send_fd(pusfd,fds[i].fd);
                         }
              }
    //}
    return 0;
}
