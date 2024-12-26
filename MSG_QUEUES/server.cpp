#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<cstring>
using namespace std;
//server
struct message{
  long msg_type;
  char text[100];
  pid_t sender;
  pid_t receiver;
};

void SendtoChilds(int &msgid,vector<pid_t>& pid)
{
  for(int i=0;i<3;i++)
  {
     for(int j=0;j<3;j++)
     {
         if(i!=j)
         {
            message msg;
            msg.msg_type=pid[i];
            msg.sender=pid[j];
            msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
         }
     }
  }
}

void Getpids(int &msgid,vector<pid_t>&pid)
{
  for(int i=0;i<3;i++)
  {
       message msg;
       msg.msg_type=1;
       msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),1,0);
       pid[i]=msg.sender;
  }
  return;
}

void Drama(int &msgid,vector<pid_t>&pid)
{
    while(1)
    {
        message msg;
        msg.msg_type=1;
        msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),1,0);
        cout<<msg.receiver<<":"<<msg.text<<endl;
        message send;
        for(int i=0;i<100;i++)send.text[i]=msg.text[i];
        send.msg_type=msg.receiver;
        send.sender=msg.sender;
        msgsnd(msgid,&send,sizeof(send)-sizeof(long),0);
   }
}

int main()
{
  key_t key;
  int msgid;
  key=ftok("chat",65);
  msgid=msgget(key,0666|IPC_CREAT);
  vector<pid_t> pid(3);
  Getpids(msgid,pid);
  SendtoChilds(msgid,pid);
  for(int i=0;i<3;i++)cout<<pid[i]<<" ";
  Drama(msgid,pid);
  return 0;
}
