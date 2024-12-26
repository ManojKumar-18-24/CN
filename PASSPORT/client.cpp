
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

#define PORT 12346

const char *documents_ihave[] = {
    "Completed passport application form                              \n ", // 43 characters
    "Passport-size photographs                                        \n",  // 43 characters
    "Proof of identity (e.g., National ID card)                       \n",  // 43 characters
    "Proof of address (e.g., utility bill)                            \n",  // 43 characters
    "Date of birth certificate                                        \n",  // 43 characters
    "Proof of citizenship (e.g., Birth certificate, Voter ID)         \n",  // 57 characters
    "Marriage certificate (if applicable)                             \n",  // 57 characters
    "Parent's consent form (for minors)                               \n",  // 57 characters
    "Travel itinerary (if applicable)                                \n",   // 57 characters
    "Proof of legal name change (if applicable)                       \n",  // 57 characters
    "National Tax Number (TIN) certificate (if applicable)            \n",  // 54 characters
    "Proof of employment or student status (if applicable)            \n",  // 54 characters
    "Affidavit of support (if applicable)                             \n",  // 54 characters
    "Previous passport (if renewing)                                  \n",  // 54 characters
    "Payment receipt for passport application fee                     \n"   // 54 characters
};

string req_documents[15]={""};

int generate_client(int port)
{
    int sfd;
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("\n socket");
    else
        printf("\n socket created successfully\n");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if (connect(sfd, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) == -1)
        perror("\n connect : ");
    else
        printf("\nconnect succesful");
    return sfd;
}

string receive_string(int rsfd, char *buffer, size_t buffer_size) {
    sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);

    // Receive the message
    ssize_t received_bytes = recvfrom(rsfd, buffer, buffer_size, 0, (sockaddr *)&src_addr, &addr_len);
    if (received_bytes < 0) {
        perror("recvfrom failed");
    } else if (received_bytes > 0) {
        buffer[received_bytes] = '\0'; // Null-terminate the received data
        
        // Skip IP header if using raw sockets (for IPv4)
        char *payload = buffer + 20;
        
        std::cout << "Received message: " << payload << std::endl;
        std::cout << "From IP: " << inet_ntoa(src_addr.sin_addr) << std::endl;
        return payload;
    } else {
        std::cout << "No data received." << std::endl;
    }
    return "";
}

void get_stream(int rsfd)
{
    struct sockaddr_in client;
    socklen_t clilen = sizeof(client);
    char buffer[1024];
    for (int i = 0; i < 15; i++)
    {
        req_documents[i]=receive_string(rsfd,buffer,sizeof(buffer));
    }
}

int main()
{
    int rsfd = socket(AF_INET, SOCK_RAW, 75);
    get_stream(rsfd);
    //char buffer[1024]; // Adjust buffer size as needed
    //receive_string(rsfd, buffer, sizeof(buffer));
    sleep(5);
    int clfd = generate_client(PORT);
    for(int i=0;i<15;i++)
    {
        send(clfd,documents_ihave[i],strlen(documents_ihave[i]),0);
        sleep(2);
    }
    bool success;
    recv(clfd,&success,sizeof(bool),0);
    cout<<"result: "<<success<<endl;    
    return 0;
}