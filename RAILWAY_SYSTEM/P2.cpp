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
#include<semaphore.h>
using namespace std;
#define PORT 12336
#define ADDR1 "at_R2"

sem_t *sem;

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
	int nsfd=accept(sfd,(struct sockaddr*)&cli_addr,(socklen_t*)&cli_len);
	if(nsfd==-1)cout<<"\nnsfd gone\n";
	return sfd;
}

int get_usfd_server(string addr)
{
     	int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, addr.c_str());
	unlink(addr.c_str());
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);
	ucli_len=sizeof(ucli_addr);
	int nusfd;
	nusfd=accept(usfd, (struct sockaddr *)&ucli_addr, (socklen_t*)&ucli_len);
	return nusfd;
}

int get_peer_name(int new_socket)
{
   struct sockaddr_in peer;
   int peer_len;
   
   peer_len = sizeof(peer);
     
   if (getpeername(new_socket, (struct sockaddr*)&peer, (socklen_t*)&peer_len) == -1) {
      perror("getpeername() failed");
      return -1;
   }

      /* Print it.    */
   //printf("\nPeer's IP address is: %s\n", inet_ntoa(peer.sin_addr));
   printf("\nPeer's port is: %d came for round1\n", (int) ntohs(peer.sin_port));
   return (int) ntohs(peer.sin_port);
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

int anusfd;
int nsfd;

void Krupiya_Dyan_Dijiye()
{
       struct pollfd fds[1];
       fds[0].fd=anusfd;
       fds[0].events=POLLIN;
       int sem_value;
       while(1)
       {
                sem_getvalue(sem, &sem_value);
              printf("sem1 current value: %d\n", sem_value);
              sem_wait(sem);
              cout<<"you can enter platform\n";
              poll(fds,1,2000);
              if(fds[0].revents & POLLIN)
              {
                     int nsfd=recv_fd(fds[0].fd);
                     cout<<get_peer_name(nsfd)<<"Bhogis: ";
                     char buffer[2];
                     for(int i=0;i<20;i++)
                     {
                           recv(nsfd,buffer,2,0);
                           cout<<buffer[0]<<buffer[1]<<" ";
                     }
                     cout<<endl;
                     char a[20];
                     int read=recv(nsfd,a,20,0);
                     cout<<get_peer_name(nsfd)<<" sent msg..that it is leaving..i.e. ";
                     for(int i=0;i<read;i++)cout<<a[i];cout<<endl;
                     //sleep(100);
              }
             //sleep(15);
              sem_post(sem);
              cout<<"platform came out\n";
              sleep(2);
       }
}

void* Advertise(void * args)
{
       struct pollfd fds[1];
       fds[0].fd=nsfd;
       fds[0].events=POLLIN;
       char buffer[100];
       while(1)
       {
             poll(fds,1,2000);
             if(fds[0].revents & POLLIN)
             {
                  int read=recv(fds[0].fd,buffer,100,0);
                  cout<<"Add: ";
                  for(int i=0;i<read;i++)cout<<buffer[i];
                  cout<<endl;
             }
             //else cout<<"Add not polled\n";
       }
}

int main()
{  sem_unlink("/r2");
   sem = sem_open("/r2", 1);
    anusfd=get_usfd_server(ADDR1);
    nsfd=co_server(PORT);
    pthread_t th;
    pthread_create(&th,NULL,&Advertise,NULL);
    Krupiya_Dyan_Dijiye();
    return 0;
}
