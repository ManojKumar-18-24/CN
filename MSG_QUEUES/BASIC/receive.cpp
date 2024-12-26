#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <unistd.h>
using namespace std;

struct message {
    long msg_type;
    char msg_text[100];
};

int main() {
    key_t key;
    int msgid;

    // Generate unique key
    key = ftok("progfile", 65);

    // Access the message queue
    msgid = msgget(key, 0666 | IPC_CREAT);

    message msg;

    // Receive message
    msgrcv(msgid, &msg, sizeof(msg), 1, 0);

    // Display the message
    cout << "Message received: " << msg.msg_text << std::endl;

    // Destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}

