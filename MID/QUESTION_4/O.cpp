#include<bits/stdc++.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<poll.h>
#include<sys/socket.h>
#include<sys/msg.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
using namespace std;

struct message{
     long msg_type;
     char text[100];
};

int size=5,port=6783;

void Socket_Init(vector<int>&sfd)
{
   struct sockaddr_in address[size];
   socklen_t addrlen[size];
   for(int i=0;i<size;i++)addrlen[i]=sizeof(address[i]);
   int opt=1;
   for(int i=0;i<size;i++)
   {
      sfd[i]=socket(AF_INET,SOCK_STREAM,0);
      if(sfd[i]<0)
      {
         cout<<"socket prooblem\n";
         return;
      }
   }
   for(int i=0;i<size;i++)
   {
      if(setsockopt(sfd[i],SOL_SOCKET,SO_REUSEADDR|
               SO_REUSEPORT,&opt,sizeof(opt))) {
         cout<<"socket opt prooblem\n";
         return ;      
      }
   }
   
   for(int i=0;i<size;i++)
   {
      inet_pton(AF_INET, "127.0.0.1", &address[i].sin_addr);
      address[i].sin_family = AF_INET;
      address[i].sin_addr.s_addr = INADDR_ANY;
      address[i].sin_port = htons(port+i); 
      if(bind(sfd[i],(struct sockaddr*)&address[i],sizeof(address[i]))<0)
      {
         cout<<"socket bind prooblem\n";
         return ; 
      }
      
   }
   
   for(int i=0;i<size;i++)
   {
       if(listen(sfd[i],5)<0)
       {
           cout<<"Not listening\n";
       }
   }
}

int main()
{
    key_t key;
    int msgid;
    key=ftok("queue",65);
    msgid=msgget(key,0666|IPC_CREAT);
    vector<int> sfd(size);
    Socket_Init(sfd);
    struct sockaddr_in address[size];
    socklen_t addrlen[size];
    struct pollfd fds[size];
    for(int i=0;i<size;i++)
    {
          fds[i].fd=sfd[i];
          fds[i].events=POLLIN;
    }
    struct pollfd joined[3*size];
    vector<bool> sent_chat(3*size,false);
    nfds_t ptr=0;
    int first_time=false;
    nfds_t nfd=size;
    while(1)
    {
         //cout<<"hi\n";
         poll(fds,nfd,1000);
         for(int i=0;i<size;i++)
         {
                if(fds[i].revents & POLLIN)
                {
                        cout<<"polled\n";
                        if(first_time==false)
                        {
                               message msg;
                               msg.msg_type=2;  /*notifying prof. to start the class*/
                               msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);   /*to prof1.*/                                 
                        }
                        int nsfd=accept(sfd[i],(struct sockaddr*)&address[i],&addrlen[i]);
                        joined[ptr].fd=nsfd;
                        joined[ptr].events=POLLIN;
                        ptr++;
                }
         }
         //for(int i=0;i<6;i++)
         //{
                    message msg;
                    int r=msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),1,IPC_NOWAIT);
                    if(r==-1)continue;
                    if(msg.text[0]=='e' && msg.text[1]=='n' && msg.text[2]=='d')
                    {
                          cout<<"class ended\n";
                          break;
                    }
                    for(int i=0;i<ptr;i++)
                    {
                            send(joined[i].fd,msg.text,strlen(msg.text),0);
                     }
         //}
         poll(joined,ptr,1000);  /*students sending chats*/
         for(int i=0;i<ptr;i++)
         {
                 if(joined[i].revents & POLLIN)
                 {
                        char buffer[100];
                        recv(joined[i].fd,buffer,sizeof(buffer),0);
                        message msg;
                        msg.msg_type=4;
                        for(int i=0;i<strlen(buffer);i++)msg.text[i]=buffer[i];
                        msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
                        sent_chat[i]=true;
                        cout<<i<<"chatted\n";
                 }
         }
    }
    for(int i=0;i<ptr;i++)
    {
           send(joined[i].fd,"attendence_marked\n",18,0);
           if(sent_chat[i])send(joined[i].fd,"Appreciated_for_ur_involvment\n",30,0);
           send(joined[i].fd,"class_ended\n",12,0);
    }
    return 0;
}
