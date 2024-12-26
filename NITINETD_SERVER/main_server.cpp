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
#define PORT1 12378
#define PORT2 13456
#define ADDRESS1 "HAHA"
#define ADDRESS2  "HAHAHA"

vector<int> nsfd1={-1,-1};
vector<int> nsfd2={-1,-1,-1};
vector<string> services={"./s1","./s2"};
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
	return sfd;
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
   	strcpy(userv_addr.sun_path,s.c_str());

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
       int c=0;
       c=fork();
       if(c==0)
       {
           dup2(p->first,0);
           dup2(p->first,1);
           execl(services[p->second].c_str(),services[p->second].c_str(),"N",(char*)NULL);
       }
       sleep(100);
       if(p->second==0)
       {
             if(nsfd1[1]==p->first)nsfd1[1]=-1;
             else nsfd1[0]=-1;
       }
       else {
           if(nsfd2[0]==p->first)nsfd2[0]=-1;
           else if(nsfd2[1]==p->first)nsfd2[1]=-1;
           else nsfd2[2]=-1;
       }
       return (void*)5;
}

int main()
{
      vector<int> sfd(2);
      sfd[0]=co_server(PORT1);
      sfd[1]=co_server(PORT2);
      vector<int> cusfd(2);
      cusfd[0]=get_usfd_client(ADDRESS1);
      cusfd[1]=get_usfd_client(ADDRESS2);
      struct pollfd fds[2];
      for(int i=0;i<2;i++)
      {
            fds[i].fd=sfd[i];
            fds[i].events=POLLIN;
      }
      struct sockaddr_in addr;
      socklen_t len = sizeof(addr);
      vector<pthread_t> th(100);
      int i=0;
      while(1)
      {
          poll(fds,2,2000);
          for(int i=0;i<2;i++)
          {
                 if(fds[i].events & POLLIN)
                 {
                       int *nsfd_ptr = (int *) malloc(sizeof(int));
                       *nsfd_ptr = accept(fds[i].fd, (struct sockaddr *) &addr, &len);
                       pair<int, int>* p = (pair<int, int>*)malloc(sizeof(pair<int, int>));
                       *p = make_pair(*nsfd_ptr, i);
                       int index=-1;
                        if(i==0)
                        {     int j=0;
                               for(j=0;j<2;j++)if(nsfd1[j]==-1)index=j;
                               cout<<" index "<<j<<" is empty for "<<i<<" sfd\n";
                               if(index!=-1)nsfd1[index]=*nsfd_ptr;
                        }
                        else
                        {
                              int j=0;
                              for(j=0;j<3;j++)if(nsfd2[j]==-1)index=j;
                              cout<<" index "<<j<<" is empty for "<<i<<" sfd\n";
                              if(index!=-1)nsfd2[index]=*nsfd_ptr;
                        }
                       if (*nsfd_ptr == -1) {
                                perror("accept");
                                free(nsfd_ptr);
                       } else if(index!=-1){
                                pthread_create(&th[i], NULL, &serve, (void*)p);
                                 i++;
                       }
                       else
                       {
                             cout<<get_peer_name(*nsfd_ptr)<<" has been diverted to n2 i value: "<<i<<endl;
                             send_fd(cusfd[i],*nsfd_ptr);
                             close(*nsfd_ptr);
                       }
                 }
          }
      }
      return 0;
}
