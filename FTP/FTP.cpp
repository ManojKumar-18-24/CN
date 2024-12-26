#include <bits/stdc++.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
using namespace std;

#define ADDRESS1 "Active"
#define ADDRESS2 "Passive"
#define PORT 12345

struct message
{
    long type;
    int port;
};

int ausfd, pusfd;
int msgid;

int send_fd(int socket, int fd_to_send)
{
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    /* storage space needed for an ancillary element with a paylod of length is
    CMSG_SPACE(sizeof(length)) */
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
    *((int *)CMSG_DATA(control_message)) = fd_to_send;
    return sendmsg(socket, &socket_message, 0);
}

int get_usfd_client(string address)
{
    int usfd;
    struct sockaddr_un userv_addr;
    int userv_len, ucli_len;
    usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (usfd == -1)
        perror("\nsocket ");
    bzero(&userv_addr, sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path, address.c_str());
    userv_len = sizeof(userv_addr);
    if (connect(usfd, (struct sockaddr *)&userv_addr, userv_len) == -1)
        perror("\n connect ");
    else
        printf("\nconnect succesful");
    return usfd;
}

int co_server(int port)
{
    int sfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("\n socket ");
    else
        printf("\n socket created successfully");
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        perror("\n bind : ");
    else
        printf("\n bind successful ");
    listen(sfd, 10);
    return sfd;
}

void *service(void *args)
{
    int nsfd = *(int *)args;
    free(args);
    char buffer[1024];
    int read = recv(nsfd, buffer, 1024, 0);
    cout<<"buffer= "<<buffer<<endl;
    int index = 2;
    int port;
    string filename;
    message msg;
    if (buffer[0] == 'a')
    {
        while (!(buffer[index] < '0' || buffer[index] > '9'))
        {
            port *= 10;
            port += (buffer[index] - '0');
            index++;
        }
        cout<<"port= "<<port<<endl;
        index++;
        while (index < read-1)
        {
            filename += buffer[index];
            index++;
        }
        cout<<"filename= "<<filename<<endl;
        cout<<"last "<<filename[filename.size()-1]<<endl;
        msg.type = 1;
        msg.port = port;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        int fd=open(filename.c_str(),O_RDONLY);
        cout<<"fd= "<<fd<<endl;
        sleep(5);
        send_fd(ausfd,fd);
    }
    else
    {
        while (index < read-1)
        {
            filename += buffer[index];
            index++;
        }
        cout<<"filename= "<<filename<<endl;
        msg.type = 2;
        msg.port = 12367;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        int fd=open(filename.c_str(),O_RDONLY);
        cout<<"fd= "<<fd<<endl;
        send(nsfd,&msg.port,sizeof(int),0);
        sleep(5);
        send_fd(pusfd,fd);
        close(fd);
    }
    close(nsfd);
    return (void*)5;
}

int main()
{
    key_t key=ftok("queue",65);
    msgid=msgget(key,0644|IPC_CREAT);
    ausfd = get_usfd_client(ADDRESS1);
    pusfd = get_usfd_client(ADDRESS2);
    struct pollfd fds[1];
    fds[0].fd = co_server(PORT);
    fds[0].events = POLLIN;
    pthread_t th[100];
    int thread_no = 0;
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);
    while (true)
    {
        poll(fds, 1, 2000);
        if (fds[0].revents & POLLIN)
        {
            int nsfd = accept(fds[0].fd, (struct sockaddr *)&cli_addr, &len);
            int *a = (int *)malloc(sizeof(int));
            *a=nsfd;
            pthread_create(&th[thread_no++], NULL, service, a);
        }
    }
    return 0;
}