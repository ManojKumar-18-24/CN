#include<bits/stdc++.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#include<stdio.h>
#include<poll.h>
#include<sys/msg.h>
using namespace std;
int n=3;

struct message{
    long msg_type;
    char text[256];
};

int main()
{

    ios::sync_with_stdio(true);
    cin.tie(NULL);
    key_t key;
    int msgid;
    key=ftok("queue",65);
    if(key==-1)
    {
       cout<<"key failure";
       return 0;
    }
    char fifo[]="news_fifo";
    mkfifo(fifo,0666);
    int fd=open(fifo,O_WRONLY|O_NONBLOCK);
    msgid=msgget(key,0666 | IPC_CREAT);
    vector<FILE*> files(n);
    vector<int> file_d(n);
    struct pollfd fds[n];
    vector<string> v={"./R1","./R2","./R3"};
    for(int i=0;i<n;i++)
    {
       files[i]=popen(v[i].c_str(),"r");
       file_d[i]=fileno(files[i]);
       fds[i].fd=file_d[i];
       fds[i].events=POLLIN;
    }
    nfds_t nfs=n;
    
     while(1)
    {
         poll(fds,nfs,1000);
         char buf[3][256];
         for(int i=0;i<3;i++)
         {
              if(fds[i].revents & POLLIN)
              {        //cout<<"hi";
                        fgets(buf[i],sizeof(buf[i]),files[i]);
                        cout<<"buf "<<i<<" = "<<buf[i]<<endl;
                        buf[i][strlen(buf[i])]='\0';
                        if(buf[i][0]==(char)'/' && buf[i][1]==(char)'d')
                        {
                              write(fd,buf[i],strlen(buf[i]));
                              cout<<"yes\n";
                        }
                        else 
                        {
                              message msg;
                              msg.msg_type=1;
                              for(int j=0;j<strlen(buf[i]);j++)msg.text[j]=buf[i][j];
                              msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0);
                       }
              }
              else cout<<i<<" not polled\n";
         }
         sleep(2);
    }
    return 0;
}
