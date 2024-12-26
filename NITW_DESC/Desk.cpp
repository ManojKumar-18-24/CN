
/*
    NITW DESK
    22CSB0C23
    Samudrala Manojkumar
    Desk
*/
#include <bits/stdc++.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
using namespace std;

#include <bits/stdc++.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
using namespace std;

int srsfd,rrsfd,r2rsfd;
int receive_rsfd;
int send_answer;
void send_string(int rsfd, string message, const char *destination_ip)
{
    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(destination_ip);
    // Send the message
    if (sendto(rsfd, message.c_str(), message.length() + 1, 0, (sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto failed");
    }
    else
    {
        std::cout << "Message sent successfully" << std::endl;
    }
}

bool r1=true;

string receive_string(int rsfd, char *buffer, size_t buffer_size)
{
    sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);
    // Receive the message
    ssize_t received_bytes = recvfrom(rsfd, buffer, buffer_size, 0,
                                      (sockaddr *)&src_addr, &addr_len);
    if (received_bytes < 0)
    {
        perror("recvfrom failed");
    }
    else if (received_bytes > 0)
    {
        buffer[received_bytes] = '\0'; // Null-terminate the received data
        // Skip IP header if using raw sockets (for IPv4)
        char *payload = buffer + 20;
        std::cout << "Received message: " << payload << std::endl;
        std::cout << "From IP: " << inet_ntoa(src_addr.sin_addr) << std::endl;

        if(r1){send_string(rrsfd,payload,"172.20.247.122");r1=false;}
        else{send_string(r2rsfd,payload,"172.20.247.122");r1=true;}
        return payload;
    }
    else
    {
        std::cout << "No data received." << std::endl;
    }
    return "";
}

void * recv_question(void *args)
{
    char buffer[1024];
    while(true)
    {
        receive_string(srsfd,buffer,sizeof(buffer));
    }
}

void* receive_change(void* args)
{
    char buffer[1024];
    while(true)
    {
        string answer=receive_string(receive_rsfd,buffer,sizeof(buffer));
        send_string(send_answer,answer,"172.20.247.122");
    }
}

int main()
{
    srsfd = socket(AF_INET, SOCK_RAW, 75);
    rrsfd=socket(AF_INET,SOCK_RAW,77);
    r2rsfd=socket(AF_INET,SOCK_RAW,78);
    receive_rsfd=socket(AF_INET,SOCK_RAW,IPPROTO_RAW);
    send_answer=socket(AF_INET,SOCK_RAW,76);
    pthread_t th[2];
    pthread_create(&th[1],NULL,recv_question,NULL);
    pthread_create(&th[0],NULL,receive_change,NULL);
    while(1);
    return 0;
}