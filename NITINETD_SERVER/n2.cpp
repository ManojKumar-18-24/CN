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
#define ADDRESS1 "HAHA"
#define ADDRESS2  "HAHAHA"

vector<string> services={"./s1","./s2"};
int get_usfd_server(string s)
{
     	int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path,s.c_str());
	unlink(s.c_str());
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);
	int nusfd;
	ucli_len=sizeof(ucli_addr);
	nusfd=accept(usfd, (struct sockaddr *)&ucli_addr, (socklen_t*)&ucli_len);
	if(nusfd==-1)cout<<"nusfd gone";
	return nusfd;
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
   //printf("\nPeer's port is: %d came for round1\n", (int) ntohs(peer.sin_port));
   return (int) ntohs(peer.sin_port);
}

void * serve(void *args)
{
       pair<int, int>* p = (pair<int, int>*)args;  // Cast back to pair<int, int>*
       cout << "Thread received pair: (" << get_peer_name(p->first) << ", " << p->second << ")" << endl;
       sleep(5);
       int c=0;
       c=fork();
       if(c==0)
       {
           dup2(p->first,0);
           dup2(p->first,1);
           execl(services[p->second].c_str(),services[p->second].c_str(),"N2",(char*)NULL);
       }
       return (void*)5;
}

int main()
{
     vector<int> nusfd(2);
     nusfd[0]=get_usfd_server(ADDRESS1);
     nusfd[1]=get_usfd_server(ADDRESS2);
     struct pollfd fds[2];
     fds[0].fd=nusfd[0];
     fds[0].events=POLLIN;
     fds[1].fd=nusfd[1];
     fds[1].events=POLLIN;
     vector<pthread_t> th(100);
     int j=0;
     while(1)
     {
           poll(fds,2,2000);
           for(int i=0;i<2;i++)
           {
                 if(fds[i].revents & POLLIN)
                 {
                       cout<<"\n"<<i<<" got polled\n";
                       int *nsfd_ptr = (int *) malloc(sizeof(int));
                       *nsfd_ptr=recv_fd(fds[i].fd);
                       pair<int, int>* p = (pair<int, int>*)malloc(sizeof(pair<int, int>));
                        *p = make_pair(*nsfd_ptr, i);
                        if (*nsfd_ptr == -1) {
                                perror("accept");
                                free(nsfd_ptr);
                       } else{
                                pthread_create(&th[j], NULL, &serve, (void*)p);
                                 j++;
                       }
                 }
           }
     }
     return 0;
}
