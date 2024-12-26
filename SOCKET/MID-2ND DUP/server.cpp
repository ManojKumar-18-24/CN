/*#include<bits/stdc++.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<poll.h>
using namespace std;

void Init_CO_socket(int &server_fd,int port)
{
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void Init_CL_socket(int &sockfd,int port)
{
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return ;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(sockfd);
        return ;
    }
}

void S2_Service(int &sfd)
{
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[1024];
    
    // Receive data from client
    int n = recvfrom(sfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
    if(n < 0){
        perror("recvfrom failed");
        return;
    }
    
    buffer[n] = '\0'; // Null-terminate the received string
    cout << "Received from client on S2: " << buffer << endl;

    // Send acknowledgment to the client
    char ack[] = "Message received from S2\n";
    sendto(sfd, ack, strlen(ack), 0, (const struct sockaddr *)&cliaddr, len);
    cout << "Acknowledgment sent to client from S2" << endl;
}

void S5_Service(int &sfd)
{
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[1024];
    
    // Receive data from client
    int n = recvfrom(sfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
    if(n < 0){
        perror("recvfrom failed");
        return;
    }
    
    buffer[n] = '\0'; // Null-terminate the received string
    cout << "Received from client on S5: " << buffer << endl;

    // Send acknowledgment to the client
    char ack[] = "Message received from S5\n";
    sendto(sfd, ack, strlen(ack), 0, (const struct sockaddr *)&cliaddr, len);
    cout << "Acknowledgment sent to client from S5" << endl;
}

int main()
{
       vector<int> sfd(5);
       int port=6734;
       vector<string> services={"./S1","./S3","./S5"};
       struct pollfd fds[5];
       for(int i=0;i<5;i++)
       {
             if(i%2!=0)Init_CL_socket(sfd[i],port+i);
             else Init_CO_socket(sfd[i],port+i);
             fds[i].fd=sfd[i];
             fds[i].events=POLLIN;
       }
       nfds_t nfd=5;
       cout<<"hi\n";
       while(1)
       {
                 poll(fds,nfd,2000);
                 for(int i=0;i<5;i++)
                 {
                        if(fds[i].revents & POLLIN)
                        {
                                 cout<<i<<" got polled\n";
                                 if(i%2==0)
                                 {
                                             struct sockaddr_in address;
                                             socklen_t len=sizeof(address);
                                             int nsfd=accept(sfd[i],(struct sockaddr*)&address,&len);
                                             if(nsfd<0)
                                             {
                                                       cout<<i<<" nsfd failed\n";
                                             }
                                             int c=0; 
                                             c=fork();
                                             if(c==0)
                                             {
                                                     for(int j=0;j<5;j++)close(sfd[j]);
                                                     close(nsfd);
                                                     sleep(2);
                                                     execl( services[i/2].c_str() ,services[i/2].c_str(),to_string(port+i).c_str(),(char *)NULL);
                                                     return 0;
                                             }
                                             close(sfd[i]);
                                             close(nsfd);
                                 }
                                 else 
                                 {
                                           if(i==1)
                                           {
                                                    S2_Service(sfd[i]);
                                           }
                                           else
                                           {
                                                   S5_Service(sfd[i]);
                                           }
                                 }
                        }
                        //else cout<<i<<"not polled\n";
                 }
                 sleep(2);
       }
       return 0;
}*/
