#include<bits/stdc++.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<poll.h>
#include<sys/socket.h>
#include<sys/msg.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
using namespace std;

int port=6785;
int s_port=7871;

int Generate_Client()
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

int Give_sfd()
{
    int server_fd;
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
    address.sin_port = htons(s_port);

   
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
     return server_fd;
}

int main()
{
        struct pollfd fds[1];
        int client_fd=Generate_Client();
        fds[0].fd=client_fd;
        fds[0].events=POLLIN;
        nfds_t nfd=1;
        int i=1;
        int sfd=Give_sfd();
        struct sockaddr_in address;
        socklen_t len;
        while(true)
        {
             poll(fds,nfd,5000);
             if(fds[0].revents & POLLIN)
             {
                    char buf[100];
                    recv(fds[0].fd,buf,sizeof(buf),0);
                    cout<<buf<<endl;
                     for(int i=0;i<strlen(buf);i++)if(buf[i]=='c')return 0;
             }
             /*if(i==10)
             {
                   send(client_fd,"7871 doubt",5,0);
                   sleep(2);
                   int nsfd=accept(sfd,(struct sockaddr*)&address,&len);      S3 doesn't send particular doubt.....
                   char buf[100];
                   recv(nsfd,buf,sizeof(buf),0);
                   cout<<buf<<endl;
             }*/
             if(i==15)
             {
                 send(client_fd,"doubt",5,0);
             }
             sleep(1);
             i++;
        }
        return 0;
}
