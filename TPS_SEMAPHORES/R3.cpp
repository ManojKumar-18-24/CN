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
#define PORT 15433
#define ADDR1 "at_R3"
#define ADDR2  "R2_R3"
#define ADDR3  "R1_R3"

sem_t *sem;
set<int> st;
int msgid;
struct message{
       long msg_type;
       int port;
       int marks;
};

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
	return usfd;
}

int Conducting_Round()
{
      return rand()%100;
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
   printf("\nPeer's port is: %d came for round3\n", (int) ntohs(peer.sin_port));
   if(st.find((int) ntohs(peer.sin_port))!=st.end())return 0;
   message msg;
   msg.msg_type=3;
   msg.port=(int) ntohs(peer.sin_port);
   msg.marks=Conducting_Round();
   msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
   st.insert((int) ntohs(peer.sin_port));
   return 1;
}

int get_usfd_client()
{
   	int usfd;
	struct sockaddr_un userv_addr;
  	int userv_len,ucli_len;

  	usfd = socket(AF_UNIX, SOCK_STREAM, 0);

  	if(usfd==-1)
  	perror("\nsocket  ");

  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, ADDR3);

	userv_len = sizeof(userv_addr);

	if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len)==-1)
	perror("\n connect ");

	else printf("\nconnect succesful\n");
    
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

int Accept(int usfd)
{
    struct pollfd fds[1];
    fds[0].fd=usfd;
    fds[0].events=POLLIN;
    while(1)
    {
         poll(fds,1,2000);
         if(fds[0].revents & POLLIN)
         {
               cout<<"polled\n";
               struct sockaddr_in addr;
               socklen_t len=sizeof(addr);
               int nusfd_ptr = accept(fds[0].fd, (struct sockaddr*)&addr, &len);
               return nusfd_ptr;
         }
   }
}
int anusfd,rnusfd,cusfd;

void TakeRounds()
{
       struct pollfd fds[1];
       fds[0].fd=anusfd;
       fds[0].events=POLLIN;
       struct pollfd rds[1];
       rds[0].fd=rnusfd;
       rds[0].events=POLLIN;
       while(1)
       {
              sem_wait(sem);
              cout<<"round started\n";
              poll(fds,1,2000);
              if(fds[0].revents & POLLIN)
              {
                     int nsfd=recv_fd(fds[0].fd);
                     if(get_peer_name(nsfd)==0)cout<<"it's round discarded\n";
                     else{
                     cout<<"it's Round Going on\n";
                     sleep(5);
                     send_fd(cusfd,nsfd);}
              }
              else 
              {
                     poll(rds,1,2000);
                     if(rds[0].revents & POLLIN)
                     {
                             int nsfd=recv_fd(rds[0].fd);
                     if(get_peer_name(nsfd)==0)cout<<"it's round discarded\n";
                     else{
                     cout<<"it's Round Going on\n";
                     sleep(5);
                     send_fd(cusfd,nsfd);}
                     }
              }
              cout<<"round ended\n";
              sem_post(sem);
              sleep(2);
       }
}

int main()
{
      key_t  key;
      if((key=ftok("queue",'B')) == -1)
      {
	   perror("key");
	   exit(1);
      }
      if((msgid=msgget(key,0644|IPC_CREAT))==-1)
      {
	    perror("Key");
	    exit(1);
      }
    int ausfd=get_usfd_server(ADDR1);
    int rusfd=get_usfd_server(ADDR2);
    anusfd=Accept(ausfd);
    cusfd=get_usfd_client();
    sleep(5);
    rnusfd=Accept(rusfd);
    sem = sem_open("/r3", 0);
    TakeRounds();
    return 0;
}
