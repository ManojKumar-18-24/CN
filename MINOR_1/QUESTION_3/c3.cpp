#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/types.h>
#include <sys/stat.h>
using namespace std;

pid_t next_pid;
int msgid,fd;
struct message{
  long msg_type;
  int client_pid;
};

void TakeService(int a)
{
    message msg;
    msg.msg_type=1;
    msg.client_pid=getpid();
    msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
    sleep(8);
    for(int i=0;i<10;i++)
    {
         char buffer[13];
         read(fd,buffer,13);
         cout<<"served by buffer  :"<<buffer<<endl;
    }
    cout<<"sending signal to " <<next_pid<<"  "<<getpid()<<endl;
    kill(next_pid,SIGUSR1);
}

void send_next_pid(int a)
{
      cout<<"sigusr2-raised \n";
      message msg;
      msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),getpid(),0);
      int temp=msg.client_pid;
      msg.msg_type=msg.client_pid;
      msg.client_pid=next_pid;
      msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
      sleep(10);
      next_pid=temp;
      return;
}

int SendtoReceive(pid_t &sender)
{
    message msg;
    msg.msg_type=sender;
    msg.client_pid=getpid();
    msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
    kill(sender,SIGUSR2);
    sleep(2);
    msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),getpid(),0);
    return msg.client_pid;
}

int main()
{
    char fifo[]="resource";
    mkfifo(fifo,0666);
    fd=open(fifo,O_RDONLY);
    key_t key;
    key = ftok("manoj", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);
    signal(SIGUSR2,send_next_pid);
    signal(SIGUSR1,TakeService);
    next_pid=getpid();
    pid_t serverpid;
    cout<<"give me pid : ";
    cin>>serverpid;
    int using_pid=SendtoReceive(serverpid);
    next_pid=SendtoReceive(using_pid);
    cout<<"set up complete\n"<<next_pid<<" &"<<getpid()<<endl;
    sleep(3);
    if(next_pid==getpid())raise(SIGUSR1);
    while(1);
   return  0;
}


