#include<bits/stdc++.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<poll.h>
#include <arpa/inet.h>
using namespace std;

void ChildFunc(int &new_socket)
{
    char buffer[1024];
    char hello[]="mahesh.. machaleni mahesh\n";
    int valread = read(new_socket, buffer,
                   1024 - 1); 
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent from server\n");
    close(new_socket);
}

int main(int argc, char const* argv[])
{
   vector<int> port={8019,8023,8024};
   vector<int> sfd(3);
   struct sockaddr_in address[3];
   struct pollfd pfd[3];
   socklen_t addrlen[3];
   for(int i=0;i<3;i++)addrlen[i]=sizeof(address[i]);
   int opt=1;
   for(int i=0;i<3;i++)
   {
      sfd[i]=socket(AF_INET,SOCK_STREAM,0);
      if(sfd[i]<0)
      {
         cout<<"socket prooblem\n";
         return 0;
      }
   }
   for(int i=0;i<3;i++)
   {
      if(setsockopt(sfd[i],SOL_SOCKET,SO_REUSEADDR|
               SO_REUSEPORT,&opt,sizeof(opt))) {
         cout<<"socket opt prooblem\n";
         return 0;      
      }
   }
   
   for(int i=0;i<3;i++)
   {
      inet_pton(AF_INET, "127.0.0.1", &address[i].sin_addr);
      address[i].sin_family = AF_INET;
      address[i].sin_addr.s_addr = INADDR_ANY;
      address[i].sin_port = htons(port[i]); 
      if(bind(sfd[i],(struct sockaddr*)&address[i],sizeof(address[i]))<0)
      {
         cout<<"socket bind prooblem\n";
         return 0; 
      }
      
   }
   
   for(int i=0;i<3;i++)
   {
       if(listen(sfd[i],3)<0)
       {
           cout<<"Not listening\n";
       }
   }
   
   for(int i=0;i<3;i++)
   {
      pfd[i].fd=sfd[i];
      pfd[i].events=POLLIN;
   }

   nfds_t nfs=3;
   
   while(true)
   { 
      poll(pfd,nfs,10000);
      
      for(int i=0;i<3;i++)
      {
         if(pfd[i].revents & POLLIN)
         {
            cout<<"for i value= "<<i<<" got polled\n"; 
            int nsfd=accept(sfd[i],(struct sockaddr*)&address[i],&addrlen[i]);
            if(nsfd<0)
            {
                cout<<"i value= "<<i<<" nsfd problem\n";
                return 0;
            }
            int c=0;
            c=fork();
            if(c==0)
            {
                 for(int j=0;j<3;j++)close(sfd[j]);
                 ChildFunc(nsfd);
                 return 0;
            }
            close(nsfd);
         }
      }
   } 
   for(int i=0;i<3;i++)close(sfd[i]);
   return 0;
}

/*#include <bits/stdc++.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
using namespace std;

void ChildFunc(int new_socket)
{
    char buffer[1024];
    char hello[] = "mahesh.. machaleni mahesh\n";
    int valread = read(new_socket, buffer, sizeof(buffer) - 1); 
    if (valread > 0) {
        buffer[valread] = '\0';  // Null-terminate the string
        printf("Received: %s\n", buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent from server\n");
    } else {
        perror("read failed");
    }
    close(new_socket);
}

int main(int argc, char const* argv[])
{
    vector<int> port = {8019, 8023, 8024};
    vector<int> sfd(3);
    struct sockaddr_in address[3];
    struct pollfd pfd[3];
    socklen_t addrlen = sizeof(address[0]);
    int opt = 1;

    // Create sockets
    for(int i = 0; i < 3; ++i)
    {
        sfd[i] = socket(AF_INET, SOCK_STREAM, 0);
        if(sfd[i] < 0)
        {
            perror("Socket creation failed");
            return 1;
        }
    }

    // Set socket options
    for(int i = 0; i < 3; ++i)
    {
        if(setsockopt(sfd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
        {
            perror("Set socket options failed");
            return 1;
        }
    }

    // Bind sockets
    for(int i = 0; i < 3; ++i)
    {
        address[i].sin_family = AF_INET;
        address[i].sin_addr.s_addr = INADDR_ANY;
        address[i].sin_port = htons(port[i]); 
        if(bind(sfd[i], (struct sockaddr*)&address[i], sizeof(address[i])) < 0)
        {
            perror("Socket bind failed");
            return 1;
        }
    }

    // Listen on sockets
    for(int i = 0; i < 3; ++i)
    {
        if(listen(sfd[i], 3) < 0)
        {
            perror("Listen failed");
            return 1;
        }
    }

    // Initialize pollfd structures
    for(int i = 0; i < 3; ++i)
    {
        pfd[i].fd = sfd[i];
        pfd[i].events = POLLIN;
    }

    while(true)
    {
        int poll_result = poll(pfd, 3, 100000);
        if (poll_result == -1)
        {
            perror("Poll failed");
            return 1;
        }
        else if (poll_result == 0)
        {
            printf("Poll timeout\n");
            continue;
        }

        for(int i = 0; i < 3; ++i)
        {
            if(pfd[i].revents & POLLIN)
            {
                cout<<i<<"came for this\n";
                int nsfd = accept(sfd[i], (struct sockaddr*)&address[i], &addrlen);
                if (nsfd < 0)
                {
                    perror("Accept failed");
                    return 1;
                }

                int c = fork();
                if (c < 0)
                {
                    perror("Fork failed");
                    return 1;
                }

                if (c == 0)
                {
                    // In child process
                    for (int j = 0; j < 3; ++j) close(sfd[j]);
                    ChildFunc(nsfd);
                    exit(0);
                }
                close(nsfd);
            }
        }
    }

    // Close sockets
    for(int i = 0; i < 3; ++i) close(sfd[i]);
    return 0;
}
*/






