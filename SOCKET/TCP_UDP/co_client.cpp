#include<bits/stdc++.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

void Chat(int &client_fd)
{
    char hello[] = "Hello from client\n";
    char buffer[1024] = { 0 };
    send(client_fd, hello, strlen(hello), 0);
    printf("Hello message sent from server\n");
    int valread = read(client_fd, buffer,
                   1024 - 1); 
    printf("%s\n", buffer);
}

int main(int argc, char const* argv[])
{
    vector<int> cfd(1);
    struct sockaddr_in serv_addr[1];
    vector<int> port={9999};
    for(int i=0;i<1;i++)
    {
        cfd[i]=socket(AF_INET, SOCK_STREAM, 0);
        if(cfd[i]<0)
        {
           cout<<"client side failed\n";
           return 0;
        }
    }

    for(int i=0;i<1;i++)
    {
         serv_addr[i].sin_family = AF_INET;
         serv_addr[i].sin_port = htons(port[i]);
    }
    
    for(int i=0;i<1;i++)
    {
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr)<= 0) {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
       }
    }
    
    for(int i=0;i<1;i++)
    {
        if(connect(cfd[i],(struct sockaddr*)&serv_addr[i],sizeof(serv_addr[i]))<0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }
    }
    
    for(int i=0;i<1;i++)
    {
       Chat(cfd[i]);
    }
    
    for(int i=0;i<1;i++)close(cfd[i]);
    
    return 0;
}
