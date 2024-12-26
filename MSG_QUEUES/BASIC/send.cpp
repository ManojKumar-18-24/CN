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

    // Create message queue and return identifier
    msgid = msgget(key, 0666 | IPC_CREAT);

    message msg;
    msg.msg_type = 1;

    cout << "Enter a message: ";
    cin.getline(msg.msg_text, 100);

    // Send message
    msgsnd(msgid, &msg, sizeof(msg), 0);

    cout << "Message sent: " << msg.msg_text << std::endl;

    return 0;
}

