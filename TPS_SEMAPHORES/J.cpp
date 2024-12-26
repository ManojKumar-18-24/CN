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
#define PORT 15000


vector<int> nsfds;
int msgid;
struct message{
       long msg_type;
       int port;
       int marks;
};

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

void * get_marks(void * args)
{
      int count=0;
      map<int,int> mp;
      message msg;
      for(int i=0;i<12;i++)
      {
              msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),3,0);
              mp[msg.port]+=msg.marks;
      }
      vector<vector<int>> store(4,vector<int>(2)) ;
      int i=0;
      cout<<endl;
      for(auto it=mp.begin();it!=mp.end();++it)
      {
            cout<<it->first<<" "<<it->second<<endl;
            store[i][0]=it->second;
            store[i][1]=it->first;
            i++;
      }
      sort(store.begin(),store.end());
      for(int i=0;i<nsfds.size();i++)
      {
           for(int j=3;j>1;j--)
           {
                 send(nsfds[i],&store[j][1],sizeof(int),0);
                 send(nsfds[i],&store[j][0],sizeof(int),0);
          }
      }
}

int main()
{
    nsfds.resize(0);
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
    pthread_t s;
    pthread_create(&s,NULL,&get_marks,NULL);
    int sfd=co_server(PORT);
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    while(1)
    {
                int nsfd_ptr = accept(sfd, (struct sockaddr *) &addr, &len);
                if (nsfd_ptr == -1) {
                    perror("accept");
                }
                else {
                   nsfds.push_back(nsfd_ptr);
                }
    }
    return 0;
}
