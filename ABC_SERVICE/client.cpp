#include<bits/stdc++.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<poll.h>
#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
using namespace std;

int m_port=7692;

void Chat(int &client_fd,int i)
{
    cout<<"connected successfully\n";
    char hello[] = "Hello_from_client\n";
    //char buffer1[1024];
    //int valread=recv(client_fd,buffer1,1023,0);
    //cout<<buffer1<<endl;
    char buffer[1024] = { 0 };
    send(client_fd, hello, strlen(hello), 0);
    printf("Hello message sent from server & socket %d is working \n",i);
    int valread = recv(client_fd, buffer,
                   1024 - 1,0); 
    printf("%s\n", buffer);
}

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

int main()
{
     for(int i=0;i<4;i++)
     {
             for(int j=0;j<21;j++)
             {
                   int client_fd=Generate_Client(m_port+i);
                   Chat(client_fd,i);
                   close(client_fd);
             }
             sleep(3);
     }
    return 0;
}
