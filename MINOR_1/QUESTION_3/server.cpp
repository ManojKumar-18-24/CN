#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/types.h>
#include <sys/stat.h>
using namespace std;

struct message{
  long msg_type;
  int client_pid;
};
int msgid;
pid_t using_pid=0;

void send_using_pid(int a)
{
      message msg;
      msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),getpid(),0);
      msg.msg_type=msg.client_pid;
      if(using_pid!=0)msg.client_pid=using_pid;
      //cout<<"my using_pid sending= \n"<<msg.client_pid<<endl;
      msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
      return;
}

void Serving(int fd)
{
      while(1)
      {
         //cout<<"msg received before\n"<<endl;
         message rcv;
         msgrcv(msgid,&rcv,sizeof(rcv)-sizeof(long),1,0);
         //cout<<"msg received after\n"<<endl;
         using_pid=rcv.client_pid;
         cout<<"I am serving to "<<using_pid<<endl<<endl;
         for(int i=0;i<10;i++)
         {
              write(fd,"using server\0",sizeof("using server\0"));
              //cout<<"serving to "<<using_pid<<endl;
         }
         sleep(5);
      }
}

int main()
{
    cout<<"pid= "<<getpid()<<endl;
    char fifo[]="resource";
    mkfifo(fifo,0666);
    int fd=open(fifo,O_WRONLY);
    key_t key;
    key = ftok("manoj", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);
    signal(SIGUSR2,send_using_pid);
    Serving(fd);
    return 0;
}
