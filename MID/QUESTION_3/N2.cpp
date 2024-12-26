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
      char text[256];
};

int stop=1,port=7869;

void sig_hand1(int a)
{
         stop=0;
         cout<<"telecast started msg reading stopped\n";
}

void sig_hand2(int b)
{
        stop=1;
        cout<<"telecast stopped msg reading started\n";
}

void Telecast(int clfd)
{
   char buffer[32];
   for(int i=0;i<5;i++)
   {
         recv(clfd,buffer,32-1,0);
         cout<<buffer<<endl;
         sleep(1);
    }
}

int Generate_Client()
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
     
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return client_fd;
}

int main()
{
  key_t key;
  int msgid;
  cout<<"mypid ="<<getpid()<<endl;
  pid_t pid;
  cin>>pid;
  key=ftok("queue",65);
  msgid=msgget(key,0666|IPC_CREAT);
  signal(SIGUSR1,sig_hand1);
  signal(SIGUSR2,sig_hand2);
  sleep(5);
  while(1)
  {
        while(stop==0)sleep(1);   /*during livetelecast stop=0*/
        cout<<"hi";
        message msg;
        msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),1,0);
        if(msg.text[0]>='0' && msg.text[0]<='9')
        {
               /* socket...*/
               kill(pid,SIGUSR1);
               int clfd=Generate_Client();
               //cout<<"live telecast...\n"<<msg.text<<endl;
               Telecast(clfd);
               kill(pid,SIGUSR2);
        }
        else cout<<msg.text<<endl;
        sleep(2);
  }
  return 0;
}


