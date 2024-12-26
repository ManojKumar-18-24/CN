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
#define ADDRESS  "/tmp/A_to_B"

int port=7692;
int circle=3;
int msgid;
vector<string> services={"./S1","./S2","./S3","./S4"};
int sock_fd;
struct message{
   long msg_type;
   int index;
};
vector<int> sfd(4);
int Get_usfd()
{
        int usfd;
	struct sockaddr_un userv_addr;
  	int userv_len,ucli_len;

  	usfd = socket(AF_UNIX, SOCK_STREAM, 0);

  	if(usfd==-1)
  	perror("\nsocket  ");

  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, ADDRESS);

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

void Service()
{
  for(int i=0;i<4;i++)
   {   
          message msg;
          msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),1,0);
          sock_fd=sfd[msg.index];
          for(int i=0;i<7;i++)
          {
              struct sockaddr_in address;
              socklen_t len=sizeof(address);
              int nsfd=accept(sock_fd,(struct sockaddr*)&address,&len);
              if(nsfd<0)cout<<"failed\n";
              int d=0;
              d=fork();
              if(d==0)
              {   
                      cout<<"in service provider A\n";
                     dup2(nsfd,1);
                     dup2(nsfd,0);
                     execl(services[msg.index].c_str(),services[msg.index].c_str(),"A",(char*)NULL);
              }
              close(nsfd);
          }
           send_fd(Get_usfd(),sock_fd);
           msg.msg_type=2;
           msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
    }
    return;
}

int main()
{
   struct sockaddr_in addr[4];
   socklen_t len[4];
   struct pollfd fds[4];
   int opt=1;
   nfds_t nfs=4;
    key_t key;
    key=ftok("chat",65);
    msgid=msgget(key,0666|IPC_CREAT);
   for(int i=0;i<4;i++)
   {
         sfd[i]=socket(AF_INET,SOCK_STREAM,0);
         len[i]=sizeof(addr[i]);
   }
   
   for(int i=0;i<4;i++)
   {
         setsockopt(sfd[i],SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));
   }

   for(int i=0;i<4;i++)
   {
         inet_pton(AF_INET, "127.0.0.1", &addr[i].sin_addr);
         addr[i].sin_family=AF_INET;
         addr[i].sin_addr.s_addr=INADDR_ANY;
         addr[i].sin_port=htons(port+i);
         bind(sfd[i],(struct sockaddr*)&addr[i],sizeof(addr[i]));
   }
   
   for(int i=0;i<4;i++)
   {
       listen(sfd[i],21);
   }
   
   for(int i=0;i<4;i++)
   {
        fds[i].fd=sfd[i];
        fds[i].events=POLLIN;
   }
   int child=false,c=0;
   vector<bool> mask(4,true);
   while(1)
   {
       //for(int i=0;i<4;i++)count[i]%=4*circle;
       poll(fds,nfs,10000);
       
       for(int i=0;i<4;i++)
       {
             if(fds[i].revents &POLLIN && mask[i])
             {
                    message msg;
                    msg.msg_type=1;
                    msg.index=i;
                    msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
                    if(child==false) 
                    {     
                           child=true;
                           int c=0;
                           c=fork();
                           if(c==0)
                           {
                               Service();
                               return 0;
                           } 
                   }
                   msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),5,0);
                   cout<<i<<"  msg-5_received\n";
                   close(sfd[i]); 
                   //sleep(5);
             }
       }
   }
   return 0;
}
