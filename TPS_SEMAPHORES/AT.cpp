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
#include<semaphore.h>
using namespace std;
#define PORT 15433
#define ADDR1 "at_R1"
#define ADDR2 "at_R2"
#define ADDR3  "at_R3"

sem_t *sem1,*sem2,*sem3;
int r1cl,r2cl,r3cl;
int j=0;

int co_server(int port)
{
    	int sfd;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;

	if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    
	int opt=1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("\n bind successful ");

	listen(sfd,10);
	return sfd;
}

int send_fd(int socket, int fd_to_send)
 {
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  /* storage space needed for an ancillary element with a paylod of length is CMSG_SPACE(sizeof(length)) */
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  int available_ancillary_element_buffer_space;

  /* at least one vector of one byte must be sent */
  message_buffer[0] = 'F';
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;

  /* initialize socket message */
  memset(&socket_message, 0, sizeof(struct msghdr));
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
  memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = available_ancillary_element_buffer_space;

  /* initialize a single ancillary data element for fd passing */
  control_message = CMSG_FIRSTHDR(&socket_message);
  control_message->cmsg_level = SOL_SOCKET;
  control_message->cmsg_type = SCM_RIGHTS;
  control_message->cmsg_len = CMSG_LEN(sizeof(int));
  *((int *) CMSG_DATA(control_message)) = fd_to_send;

  return sendmsg(socket, &socket_message, 0);
 }


int Aptitude()
{
      return rand()%100;
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

int get_usfd_client(string s)
{
   	int usfd;
	struct sockaddr_un userv_addr;
  	int userv_len,ucli_len;

  	usfd = socket(AF_UNIX, SOCK_STREAM, 0);

  	if(usfd==-1)
  	perror("\nsocket  ");

  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, s.c_str());

	userv_len = sizeof(userv_addr);

	if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len)==-1)
	perror("\n connect ");

	else printf("\nconnect succesful\n");
    
	return usfd;
}

void * aptitude(void * args)
{
        cout<<"thread started\n";
        int nsfd=*(int *)args;
        free(args);
        int apt_marks=Aptitude();
          int sem_value;
        if(false && apt_marks<35)
        {
               cout<<"mr. client "<<get_peer_name(nsfd)<<" you can leave for the day bcoz ur marks: "<<apt_marks<<endl;  
        }
        else
        {       j++;
               while(1)
               {
                            /*sem_getvalue(sem1, &sem_value);
                            printf("sem1 current value: %d\n", sem_value);
                            sem_getvalue(sem2, &sem_value);
                            printf("sem2 current value: %d\n", sem_value);
                            sem_getvalue(sem3, &sem_value);
                            printf("sem3 current value: %d\n", sem_value);*/
                           if(sem_trywait(sem1)==0)
                           {
                                     send_fd(r1cl,nsfd);
                                     cout<<"mr. client "<<get_peer_name(nsfd)<<"sent to R1 with "<<apt_marks<<endl;  
                                     sleep(5);
                                     sem_post(sem1);
                                     break;
                           }
                           else if(sem_trywait(sem2)==0)
                           {
                                   send_fd(r2cl,nsfd);
                                   cout<<"mr. client "<<get_peer_name(nsfd)<<"sent to R2 with "<<apt_marks<<endl; 
                                   sleep(5);
                                   sem_post(sem2);
                                   break;
                           }
                           else if(sem_trywait(sem3)==0)
                           {
                                  send_fd(r3cl,nsfd);
                                  cout<<"mr. client "<<get_peer_name(nsfd)<<"sent to R3 with "<<apt_marks<<endl;
                                  sleep(5);
                                  sem_post(sem3); 
                                  break;
                           }
               }
        }
        return (void*)5;
}

int main()
{
    srand(time(0));
    sem_unlink("/r1");
    sem_unlink("/r2");
    sem_unlink("/r3");
    sem1 = sem_open("/r1", O_CREAT, 0644, 1);
    sem2 = sem_open("/r2", O_CREAT, 0644, 1);
    sem3 = sem_open("/r3", O_CREAT, 0644, 1);
    int sfd=co_server(PORT);
    r1cl=get_usfd_client(ADDR1);
    r2cl=get_usfd_client(ADDR2);
    r3cl=get_usfd_client(ADDR3);
    struct pollfd fds[1];
    fds[0].fd=sfd;
    fds[0].events=POLLIN;
    vector<pthread_t> th(100);
/*     while (true) {
        int poll_ret = poll(fds, 1, 5000);  // Timeout of 2000ms

        if (poll_ret > 0) {  // At least one event occurred
            if (fds[0].revents & POLLIN) {  // New connection ready to accept
                cout << "polled\n";
                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                int *nsfd_ptr = (int *) malloc(sizeof(int));
                *nsfd_ptr = accept(fds[0].fd, (struct sockaddr *) &addr, &len);

                if (*nsfd_ptr == -1) {
                    perror("accept");
                    free(nsfd_ptr);
                } else {
                    pthread_create(&th[i], NULL, &aptitude, nsfd_ptr);
                    i++;
                }
            }
        } else if (poll_ret == 0) {
            // Timeout, no event
            cout << "No event, timeout\n";
        } else {
            // Error in poll()
            perror("poll error");
        }
    }*/
    int i=0;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    while(i<4)
    {
           int *nsfd_ptr = (int *) malloc(sizeof(int));
          *nsfd_ptr = accept(sfd, (struct sockaddr *) &addr, &len);
                if (*nsfd_ptr == -1) {
                    perror("accept");
                    free(nsfd_ptr);
                } else {
                    pthread_create(&th[i], NULL, &aptitude, nsfd_ptr);
                    i++;
                }
    }
    while(1);
    return 0;   
}
