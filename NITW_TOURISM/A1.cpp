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

vector<string> address={"T1","T2","T3"};
vector<string> location={"L1","L2","L3"};
vector<int> nusfd;

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

int get_usfd_server(int i)
{
     	int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, address[i].c_str());
	unlink(address[i].c_str());
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

void * tour(void * args)
{
     pair<int, int>* p = (pair<int, int>*)args;  // Cast back to pair<int, int>*
     cout << "Thread received pair: (" << get_peer_name(p->first) << ", " << p->second << ")" << endl;
     string buffer="You are touring "+ location[p->second]+" ahladham....\n";
     int c=0;
     c=fork();
     if(c==0)
     {
            int r=rand()%100;
            cout<<"r value for "<<get_peer_name(p->first)<<"  "<<r<<endl;
            if(r>10)
            {
                send(p->first,"oh no vehicle gone",18,0);
                send(nusfd[p->second],"ban\n",4,0);
                return 0;
            }
            send(p->first,buffer.c_str(),buffer.length(),0);
            send(nusfd[p->second],"ban\n",4,0);
     }
     return (void*)5;
}

int main()
{
        srand(time(0));
        nusfd.resize(3);
        for(int i=0;i<3;i++)nusfd[i]=get_usfd_server(i);
        struct pollfd fds[3];
        for(int i=0;i<3;i++)
        {
             fds[i].fd=nusfd[i];
             fds[i].events=POLLIN;
        }
        vector<pthread_t> th(100);
        int i=0;
        while(1)
        {
               poll(fds,3,2000);
               for(int j=0;j<3;j++)
               {
                      if(fds[j].revents & POLLIN)
                      {
                             pair<int, int>* p = (pair<int, int>*)malloc(sizeof(pair<int, int>));
                            *p = make_pair(recv_fd(nusfd[j]), j);  // Assign values to the pair
                             pthread_create(&th[i],NULL,&tour,(void*)p);
                             i++;
                      }
               }
        }
        return 0;
}
