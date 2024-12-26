#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <poll.h>
#include <fcntl.h>

int main() {
    int n=3,nsfd;
    int sfd[n];
    struct sockaddr_in server_addr[n];
    int addr_len = sizeof(server_addr[0]);

    struct pollfd fds[n];
    

    for(int i=0;i<n;i++)
    {
        sfd[i] = socket(AF_INET, SOCK_STREAM, 0);
        inet_pton(AF_INET, "127.0.0.1", &server_addr[i].sin_addr);
        server_addr[i].sin_family = AF_INET;
        server_addr[i].sin_port = htons(8080+i);
        bind(sfd[i], (struct sockaddr*)&server_addr[i], sizeof(server_addr[i]));
        listen(sfd[i], n);

        fds[i].fd=sfd[i];
        fds[i].events=POLLIN;
    }

    while(1)
    {
        poll(fds, n, 5000);
        for(int i=0;i<3;i++)
        {
            if(fds[i].revents & POLLIN)
            {
                nsfd = accept(sfd[i], (struct sockaddr*)&server_addr[i], (socklen_t*)&addr_len);

                int c = fork();
                if(c>0)
                {
                    close(nsfd);
                }
                else
                {
                    for(int j=0;j<3;j++)
                    close(sfd[j]);

                    char buffer[1024] = {0};
                    read(nsfd, buffer, 1024);
                    printf("Received: %s\n", buffer);

                    char response[] = "Hello from server";
                    send(nsfd, response, strlen(response), 0);

                    exit(0);
                }
            }
        }
    }

    // Close socket
    close(nsfd);
    return 0; 
}
