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
#include<semaphore.h>
using namespace std;

sem_t *sem1,*sem2,*sem3;

int main()
{
    sem_unlink("/r1");
    sem_unlink("/r2");
    sem_unlink("/r3");
    sem1 = sem_open("/r1", O_CREAT, 0644, 1);
    sem2 = sem_open("/r2", O_CREAT, 0644, 1);
    sem3 = sem_open("/r3", O_CREAT, 0644, 1);
           while(1)
           {
                            if(sem_trywait(sem1)==0)
                           {
                                     cout<<"R1 started\n";
                                     sleep(5);
                                     sem_post(sem1);
                                     cout<<"ended\n";
                                     sleep(2);
                           }
                           else if(sem_trywait(sem2)==0)
                           {
                                  cout<<"R2 started\n";
                                   sleep(5);
                                   sem_post(sem2);
                                   cout<<"ended\n";
                                     sleep(2);
                           }
                           else if(sem_trywait(sem3)==0)
                           {
                                  cout<<"R3 started\n";
                                  sleep(5);
                                  sem_post(sem3); 
                                  cout<<"ended\n";
                                     sleep(2);
                           }
          }
    return 0;
}


