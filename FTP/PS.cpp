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

#define ADDRESS1 "Passive"

struct message
{
    long type;
    int port;
};

int pnusfd;
int msgid;

int get_usfd_server(string address)
{
    int usfd;
    struct sockaddr_un userv_addr, ucli_addr;
    socklen_t userv_len, ucli_len = sizeof(ucli_addr);
    usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    perror("socket");
    bzero(&userv_addr, sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path, address.c_str());
    unlink(address.c_str());
    userv_len = sizeof(userv_addr);
    if (bind(usfd, (struct sockaddr *)&userv_addr, userv_len) == -1)
        perror("server: bind");
    listen(usfd, 5);
    int nusfd = accept(usfd, (struct sockaddr *)&ucli_addr, &ucli_len);
    return nusfd;
}

int co_server(int port)
{
    int sfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len=sizeof(cli_addr);
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
    int nsfd=accept(sfd,(struct sockaddr*)&cli_addr,&cli_len);
    if(nsfd==-1)
    {
        cout<<"didn't get nsfd\n";
    }
    return nsfd;
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
    if (recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
        return -1;
    if (message_buffer[0] != 'F')
    {
        /* this did not originate from the above function */
        return -1;
    }
    if ((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
    {
        /* we did not provide enough space for the ancillary element array */
        return -1;
    }
    /* iterate ancillary elements */
    for (control_message = CMSG_FIRSTHDR(&socket_message);
         control_message != NULL;
         control_message = CMSG_NXTHDR(&socket_message, control_message))
    {
        if ((control_message->cmsg_level == SOL_SOCKET) &&
            (control_message->cmsg_type == SCM_RIGHTS))
        {
            sent_fd = *((int *)CMSG_DATA(control_message));
            return sent_fd;
        }
    }
    return -1;
}

void send_data(int nsfd, int fd)
{
    char buffer[1024];
    int reads;
    while ((reads = read(fd, buffer, 1024)) > 0)
    {
        send(nsfd, buffer, reads, 0);
        cout<<"buffer= "<<buffer<<endl;
    }
    return;
}

void *service(void *args)
{
    int port = *(int *)args;
    cout<<"port= "<<port<<endl;
    int nsfd = co_server(port);
    free(args);
    struct pollfd fds[1];
    fds[0].fd = pnusfd;
    fds[0].events = POLLIN;
    while (1)
    {
        poll(fds, 1, 2000);
        if (fds[0].revents & POLLIN)
        {
            int fd = recv_fd(fds[0].fd);
            cout<<"fd= "<<fd<<endl;
            send_data(nsfd, fd);
            break;
        }
    }
    return (void*)5;
}

int main()
{
    pnusfd = get_usfd_server(ADDRESS1);
    key_t key = ftok("queue", 65);
    msgid = msgget(key, 0644 | IPC_CREAT);
    pthread_t th[100];
    int thread_no = 0;
    while (true)
    {
        message msg;
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 2, 0);
        int port = msg.port;
        cout<<"port= "<<port<<endl;
        int*a =(int*)malloc(sizeof(int));
        *a=msg.port;
        pthread_create(&th[thread_no++],NULL,service,a);
        sleep(3);
    }
    return 0;
}