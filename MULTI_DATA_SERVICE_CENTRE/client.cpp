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

int port=8080;

void Chat(int &client_fd)
{
       int r=rand()%3;
       r++;
       char temp[2];
       if(r==1)strcpy(temp,"1\0");if(r==2)strcpy(temp,"2\0");if(r==3)strcpy(temp,"3\0");
       cout<<"requested for "<<r<<" th service\n";
       send(client_fd,temp,1,0);
       char buf[256];
       for(int i=0;i<1;i++)
       {
            recv(client_fd,buf,sizeof(buf),0);
            cout<<buf<<endl;
       }
       cout<<"one-service completed\n";
}

int Generate_Client( )
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
    srand(time(0));
    while(1)
    {
        int cfd=Generate_Client();
        Chat(cfd);
        close(cfd);
        sleep(2);
    }
    return 0;
}
