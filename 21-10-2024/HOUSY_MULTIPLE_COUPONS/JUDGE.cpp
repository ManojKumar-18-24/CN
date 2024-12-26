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
#define ADDRESS "judge"

int get_usfd_server()
{
     	int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);
	ucli_len=sizeof(ucli_addr);

	int nusfd;
	nusfd=accept(usfd, (struct sockaddr *)&ucli_addr,(socklen_t *) &ucli_len);
	if(nusfd==-1)cout<<"nsfd gone\n";
	return nusfd;
}

int recv_fd(int socket)
{
  int sent_fd, available_ancillary_element_buffer_space;
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

  /* start clean */
  memset(&socket_message, 0, sizeof(struct msghdr));
  memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

  /* setup a place to fill in message contents */
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

  if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
   return -1;

  if(message_buffer[0] != 'F')
  {
   /* this did not originate from the above function */
   return -1;
  }

  if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
  {
   /* we did not provide enough space for the ancillary element array */
   return -1;
  }

  /* iterate ancillary elements */
   for(control_message = CMSG_FIRSTHDR(&socket_message);
   	control_message != NULL;
   	control_message = CMSG_NXTHDR(&socket_message, control_message))
  {
   if( (control_message->cmsg_level == SOL_SOCKET) &&
   	(control_message->cmsg_type == SCM_RIGHTS) )
   {
	sent_fd = *((int *) CMSG_DATA(control_message));
	return sent_fd;
   }
  }
   return -1;
}
int get_peer_name(int &new_socket)
{
   struct sockaddr_in peer;
   int peer_len;
   
   peer_len = sizeof(peer);
     
   if (getpeername(new_socket, (struct sockaddr*)&peer, (socklen_t*)&peer_len) == -1) {
      perror("getpeername() failed");
      return -1;
   }

      /* Print it.    */
   //printf("Peer's IP address is: %s\n", inet_ntoa(peer.sin_addr));
   //printf("Peer's port is: %d\n", (int) ntohs(peer.sin_port));
   return (int) ntohs(peer.sin_port);
}

unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short*)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void send_service_info_client(int &rsfd,int proto_num,string dest_ip)
{
    char packet[4096];
    struct ip *ip_header = (struct ip *)packet;
    struct sockaddr_in dest_addr;
    // Zero out the packet buffer
    memset(packet, 0, sizeof(packet));
    // Fill in the IP header fields
    ip_header->ip_hl = 5;                   // Header length
    ip_header->ip_v = 4;                    // IPv4
    ip_header->ip_tos = 0;                  // Type of Service
    string s;
    for(int i=0;i<5;i++)
    {
           s+=(to_string(rand()%100));
           if(i==4)s+="\n";
           else s+=" ";
    }
    ip_header->ip_len = sizeof(struct ip) + sizeof(s); // Total length
    ip_header->ip_id = htonl(54321);        // Identification
    ip_header->ip_off = 0;                  // Fragment offset
    ip_header->ip_ttl = 255;                // Time to Live
    ip_header->ip_p = proto_num;         // Custom protocol number
    ip_header->ip_sum = 0;                  // Initial checksum
    ip_header->ip_src.s_addr = inet_addr("10.0.2.15");  // Source IP
    ip_header->ip_dst.s_addr = inet_addr(dest_ip.c_str());      // Destination IP

    // Calculate IP checksum
    ip_header->ip_sum = checksum(packet, ip_header->ip_len);

    // Add data after the IP header
    char *data = packet + sizeof(struct ip);
    strcpy(data, s.c_str());

    // Prepare destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    dest_addr.sin_addr.s_addr = inet_addr(dest_ip.c_str());

    // Send the packet
    if (sendto(rsfd, packet, ip_header->ip_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }

    printf("Packet sent to client.\n");
}

int main()
{
       int nusfd=get_usfd_server();
       cout<<"nusfd= "<<nusfd<<endl;
       sleep(5);
       for(int i=0;i<3;i++)
       { 
               int nsfd=recv_fd(nusfd);
               cout<<"\n"<<get_peer_name(nsfd)<<endl;
       }
       sleep(10);
       int rsfd=socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
       for(int i=0;i<5;i++)
       {
              send_service_info_client(rsfd,78,"10.0.2.15");
              cout<<"sent\n";
       }
       struct pollfd fds[1];
       fds[0].fd=nusfd;
       fds[0].events=POLLIN;
       int count=0;
       while(count<3)
       {
               poll(fds,1,2000);
               if(fds[0].revents & POLLIN)
               {
                         int nsfd=recv_fd(fds[0].fd);
                         if(nsfd==-1)break;
                         cout<<"\n"<<get_peer_name(nsfd)<<endl;
                         send(nsfd,"prize\n",6,0);
                         count++;
               }
                sleep(2);
       }
       while(1);
       return 0;
}
