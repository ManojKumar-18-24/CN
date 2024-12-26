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

struct mess{
    long msg_type;
    pid_t pi;
};

int it=3,msgid;
bool chatting=true;
pid_t pid;

int Generate_Client(int port)
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

void Teaching()
{
      if(chatting==false)cout<<"my doubt clear ended\n\n";;
      cout<<"teaching started\n\n\n";
      for(int i=0;i<10;i++)
      {
           message msg;
           msg.msg_type=1;
           //cout<<"\n";
           strcpy(msg.text,"I am teaching\n");
           msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
           //sleep(1);
      }
      cout<<"my teaching ended\n\n";
      sleep(5);
}

void See_Chat()
{
      cout<<"my doubt clear started\n\n\n";
      while(chatting)
      {
             message msg;
             int r=msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),4,IPC_NOWAIT);
             if(r!=-1)
             {
                  cout<<msg.text<<endl;
                  if(msg.text[0]>='0' && msg.text[0]<='9')
                  {
                        int port=0;
                        for(int j=0;j<strlen(msg.text);j++)
                        {
                                 if(msg.text[j]==' ')break;
                                 port=port*10+(msg.text[j]-'0');
                        }
                        int clfd=Generate_Client(port);
                        send(clfd,"particular_answer_to_your_doubt\n",32,0);
                  }
                  else 
                  {
                          message msg;
                          msg.msg_type=1;
                          strcpy(msg.text,"answer to your general doubt\n");
                          msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
                  }
             }
      }  
}

void sig_hand1(int a)
{
       chatting=false;
       //cout<<"sig-usr1 raised\n";
       sleep(5);
       kill(pid,SIGUSR2);
       Teaching();
       kill(pid,SIGUSR1);
}

void sig_hand2(int a)
{
     chatting=true;
     See_Chat();
}

int main()
{
    cout<<"mypid= "<<getpid()<<endl;
    //cin>>pid;
    signal(SIGUSR1,sig_hand1);
    signal(SIGUSR2,sig_hand2);
    key_t key=ftok("queue",65);
    msgid=msgget(key,0666|IPC_CREAT);
    mess mg,mg1;
    mg.msg_type=6;mg.pi=getpid();
    msgsnd(msgid,&mg,sizeof(mg)-sizeof(long),0);
    msgrcv(msgid,&mg1,sizeof(mg1)-sizeof(long),5,0);
    pid=mg1.pi;
    cout<<pid<<endl;
    sleep(5);
    int i=0;
    while(i<1e9)
    {
             i++;
             pause();
    }
    return 0;
}
