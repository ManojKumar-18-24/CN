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

#define PORT 13478
#define ADDRESS1 "B1"
#define ADDRESS2 "B2"

int petrol = 500;
int diesel = 500;
int gas = 500;

int psfd, dsfd, gsfd;
int b1usfd, b2usfd;
int msgid;

struct message{
long type;
char buff[20];
int r;
};

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

pthread_mutex_t Mutex;

bool b1 = true;

void *service(void *args)
{
    pair<int, int> *p = (pair<int, int> *)args;
    cout << p->first << " " << p->second << endl;
    int r;
    message msg;
    if (p->first == 0)
    {
        r = rand() % 50;
        petrol -= r;
        msg.r=r;
        string s="petrol";
        for(int i=0;i<6;i++)msg.buff[i]=s[i];
        send(p->second, &r, sizeof(int), 0);
    }
    else if (p->first == 1)
    {
        r = rand() % 50;
        diesel -= r;
        msg.r=r;
        string s="diesel";
        for(int i=0;i<6;i++)msg.buff[i]=s[i];
        send(p->second, &r, sizeof(int), 0);
    }
    else
    {
        r = rand() % 50;
        gas -= r;
        msg.r=r;
        string s="gasgas";
        for(int i=0;i<6;i++)msg.buff[i]=s[i];
        send(p->second, &r, sizeof(int), 0);
    }
    pthread_mutex_lock(&Mutex);
    if (b1 == true)
    {    send_fd(b1usfd,p->second);msg.type=1;msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0); b1=false;}
    else{ send_fd(b2usfd,p->second);msg.type=2;msgsnd(msgid,&msg,sizeof(msg)-sizeof(long),0); b1=true;}
    pthread_mutex_unlock(&Mutex);
    return (void *)5;
}

int main()
{
    key_t key = ftok("queue", 65);
    msgid = msgget(key, 0644 | IPC_CREAT);
    pthread_mutex_init(&Mutex, NULL);
    b1usfd = get_usfd_client(ADDRESS1);
    b2usfd = get_usfd_client(ADDRESS2);
    struct pollfd fds[3];
    for (int i = 0; i < 3; i++)
    {
        fds[i].fd = co_server(PORT + i);
        fds[i].events = POLLIN;
    }
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    pthread_t th[100];
    int thread_no = 0;
    while (true)
    {
        poll(fds, 3, 200);
        for (int i = 0; i < 3; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                cout << "got polled\n";
                int nsfd = accept(fds[i].fd, (struct sockaddr *)&addr, &len);
                pair<int, int> *p = (pair<int, int> *)malloc(sizeof(pair<int, int>));
                *p = make_pair(i, nsfd);
                pthread_create(&th[thread_no++], NULL, service, (void *)p);
            }
        }
    }
    return 0;
}