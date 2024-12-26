#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<cstring>
using namespace std;

struct message{
  long msg_type;
  char text[100];
  pid_t sender;
  pid_t receiver;
};

void GetremPid(int &msgid,vector<pid_t> &pid)
{
     for(int i=0;i<2;i++)
     {
       message msg;
       msg.msg_type=getpid();
       msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),getpid(),0);
       pid[i]=msg.sender;
      }
}

void Drama(int &msgid,vector<pid_t> &pid)
{
         srand(time(0));
         while(1)
         {
               int r=rand()%2;
               message msg;
               msg.msg_type=1;
               msg.sender=getpid(); //can ask input here...
               msg.receiver=pid[r];
               cin.getline(msg.text,100);
               msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
               cout<<getpid()<<":"<<msg.text<<endl;
               message r_msg;
               r_msg.msg_type=getpid();
               msgrcv(msgid,&r_msg,sizeof(r_msg)-sizeof(long),getpid(),0);
               cout<<"                                  "<<r_msg.sender<<":"<<r_msg.text<<endl;
         }
}

void Parent(int &msgid,vector<pid_t> &pid)
{
    srand(time(0));
    while(1)
    {
               int r=rand()%2;
               message msg;
               msg.msg_type=1;
               msg.sender=getpid(); //can ask input here...
               msg.receiver=pid[r];
               cout<<getpid()<<":";cin.getline(msg.text,100);
               cout<<endl;
               msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
               //cout<<getpid()<<":"<<msg.text<<endl;
   }
}

void Child(int &msgid,vector<pid_t> &pid)
{
   while(1)
   {
                message r_msg;
               //r_msg.msg_type=getppid();
               msgrcv(msgid,&r_msg,sizeof(r_msg)-sizeof(long),getppid(),0);
               cout<<"                                  "<<r_msg.sender<<":"<<r_msg.text<<endl;
   }
}

int main()
{
     key_t key;
     key=ftok("chat",65);
     int msgid=msgget(key,0666|IPC_CREAT);
     message msg;
     msg.msg_type=1;
     msg.sender=getpid();
     msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
     vector<pid_t> pid(2);
    GetremPid(msgid,pid);   
     for(int i=0;i<2;i++)cout<<pid[i]<<" ";cout<<endl;
     int c=0;
     c=fork();
     if(c==0)
     {
        for(int i=0;i<2;i++)cout<<pid[i]<<" ";cout<<endl;
        Child(msgid,pid);
     }
     else Parent(msgid,pid);
     //Drama(msgid,pid);
     return 0;
}
