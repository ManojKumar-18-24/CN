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

#define ADDRESS1 "A-PROCESS"
#define ADDRESS2 "B-PROCESS"
#define ADDRESS3 "C-PROCESS"
#define PORT 12346

int ausfd, busfd, cusfd;
int raw_sockfd;
int msgid;

struct message
{
    long type;
    bool success;
};

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

void *service(void *args)
{
    int nsfd = *(int *)args;
    cout << "nsfd= " << nsfd << endl;
    bool success = true;
    message msg;
    free(args);
    send_fd(ausfd, nsfd);
    nsfd = recv_fd(ausfd);
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);
    success = (msg.success && success);
    cout<<"success= "<<success<<endl;
    send_fd(busfd, nsfd);
    nsfd = recv_fd(busfd);
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);
    success = (msg.success && success);
    cout<<"success= "<<success<<endl;
    send_fd(cusfd, nsfd);
    nsfd = recv_fd(cusfd);
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);
    success = (msg.success && success);
    cout<<"success= "<<success<<endl;
    send(nsfd, &success, sizeof(bool), 0);
    close(nsfd);
    return (void *)5;
}

void send_string(int rsfd, string message, const char *destination_ip) {
    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(destination_ip);

    // Send the message
    if (sendto(rsfd, message.c_str(), 6, 0, (sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto failed");
    } else {
        std::cout << "Message sent successfully" << std::endl;
    }
}

int main()
{
    key_t key = ftok("queue", 65);
    msgid = msgget(key, 0644 | IPC_CREAT);
    raw_sockfd = socket(AF_INET, SOCK_RAW, 75);
    //send_string(raw_sockfd,"hello","192.168.137.242");
    ausfd = get_usfd_client(ADDRESS1);
    busfd = get_usfd_client(ADDRESS2);
    cusfd = get_usfd_client(ADDRESS3);
    sleep(5);
    send_fd(ausfd, raw_sockfd);
    raw_sockfd = recv_fd(ausfd);
    cout<<"raw-socket= "<<raw_sockfd<<endl;
    send_fd(busfd, raw_sockfd);
    raw_sockfd = recv_fd(busfd);
    cout<<"raw-socket= "<<raw_sockfd<<endl;
    send_fd(cusfd, raw_sockfd);
    raw_sockfd = recv_fd(cusfd);
    cout<<"raw-socket= "<<raw_sockfd<<endl;
    struct pollfd fds[1];
    fds[0].fd = co_server(PORT);
    fds[0].events = POLLIN;
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);
    pthread_t th[100];
    int thread_no = 0;
    while (true)
    {
        poll(fds, 1, 2000);
        if (fds[0].revents & POLLIN)
        {
            int nsfd = accept(fds[0].fd, (struct sockaddr *)&cli_addr, &len);
            int *a = (int *)malloc(sizeof(int));
            *a = nsfd;
            pthread_create(&th[thread_no++], NULL, service, a);
        }
    }
    return 0;
}