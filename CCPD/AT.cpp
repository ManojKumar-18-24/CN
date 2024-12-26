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
#define PORT 20000
#define ADDRESS "apt_rounds"

struct message{
       long msg_type;
       int port;
       int marks;
};

int msgid;

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
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);
	return usfd;
}

void get_nsfd(struct pollfd sfds[],vector<int>&cnsfd)
{
      int count=0;
      vector<int> mask(5,true);
      while(count<5)
      {
             poll(sfds,5,2000);
             for(int i=0;i<5;i++)
             {
                    if((sfds[i].revents & POLLIN) && mask[i])
                    {
                           cout<<"polled"<<endl;
                           struct sockaddr_in addr;
                           socklen_t len=sizeof(addr);
                           count++;
                           cnsfd[i]=accept(sfds[i].fd,(struct sockaddr*)&addr,&len);
                           close(sfds[i].fd);
                           mask[i]=false;
                    }
             }
      }
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

int Aptitude()
{
     return rand()%100;
}

int main()
{
      srand(time(0));
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
      vector<int> cnsfd(5);
      struct pollfd sfds[5];
      for(int i=0;i<5;i++)
      {
            sfds[i].fd=co_server(PORT+i);
            sfds[i].events=POLLIN;
      }
      struct pollfd usfd[1];
      usfd[0].fd=get_usfd_server();
      usfd[0].events=POLLIN;
      get_nsfd(sfds,cnsfd);
      vector<int> apt_marks(5);
      for(int i=0;i<5;i++)
      {
            apt_marks[i]=Aptitude();
      }
      vector<vector<int>> result(5,vector<int>(2));
      for(int i=0;i<5;i++)
      {
            result[i][0]=apt_marks[i];
            result[i][1]=cnsfd[i];
      }
      sort(result.begin(),result.end());
      message msg;
      msg.msg_type=2;
      //int fd=open("doc.txt",O_RDONLY);
      for(int i=0;i<4;i++)
      {
             msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
             while(1)
             {
                    poll(usfd,1,2000);
                    if(usfd[0].revents & POLLIN)
                    { 
                           cout<<"\n"<<i<<" th fd sent\n";
                           struct sockaddr_in addr;
                           socklen_t len;
                           sleep(1);
                           int nusfd=accept(usfd[0].fd,(struct sockaddr*)&addr,&len);
                           if(nusfd==-1)cout<<"\npail";
                           send_fd(nusfd,result[i][1]);
                           close(nusfd);
                           break;
                    }
                    else cout<<"\nwaiting on "<<i;
                    
             }
      }
      return 0;
}
