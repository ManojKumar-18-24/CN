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
using namespace std;
#define PORT 12334
#define PORT2 8096
int generate_client(int port)
{
	int sfd;
	struct sockaddr_in serv_addr;

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_STREAM , 0))==-1)
	perror("\n socket");
	else printf("\n socket created successfully\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	//serv_addr.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET,"127.0.0.1", &serv_addr.sin_addr);

	if(connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr))==-1)
	perror("\n connect : ");
	else printf("\nconnect succesful");
	return sfd;
}

void recv_vector(int socket_fd, std::vector<int>& vec) {
    int size;
    
    // Receive the size of the vector first
    recv(socket_fd, &size, sizeof(size), 0);
    
    // Resize the vector to hold the incoming data
    vec.resize(size);
    
    // Receive the vector data
    recv(socket_fd, vec.data(), size * sizeof(int), 0);
}

int main()
{
     sleep(1);
     int srand(time(0));
     int clfd=generate_client(PORT);
     char buffer[30];
     recv(clfd,buffer,sizeof(buffer),0);
     cout<<buffer<<endl;
     sleep(6);
     cout<<"\nSevicing from ISS\n";
     while(1)
     {
         int vlfd=generate_client(PORT2);
         vector<int> vec;
         recv_vector(vlfd, vec);
         cout << "Received vector: ";
         for (int val : vec) {
              cout << val << " ";
         }
         if(vec[0]==0)return 0;
         int s=rand()%vec.size();
         cout<<"\nrequested_for "<<vec[s]<<" port number\n";
         clfd=generate_client(vec[s]);
         char buffer[30];
         recv(clfd,buffer,sizeof(buffer),0);
         cout << buffer<<endl;
         sleep(1);
    }
    return 0;
}
