#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

int main() {
    // Open pipes to P2, P3, and P4
    FILE* pipe_p2 = popen("./p2", "r");
    FILE* pipe_p3 = popen("./p3", "r");
    FILE* pipe_p4 = popen("./p4", "r");
    
    // Open pipe to P5
    FILE* pipe_p5 = popen("./p5", "w");
    
    if (!pipe_p2 || !pipe_p3 || !pipe_p4 || !pipe_p5) {
        perror("popen");
        return 1;
    }

    // Read from P2 and write to P5
    char buf[256];
    for (int i = 0; i < 2; ++i) {
        if (fgets(buf, sizeof(buf), pipe_p2)) {
            fputs(buf, pipe_p5);
        }
    }

    // Read from P3 and write to P5
    for (int i = 0; i < 3; ++i) {
        if (fgets(buf, sizeof(buf), pipe_p3)) {
            fputs(buf, pipe_p5);
        }
    }

    // Read from P4 and write to P5
    for (int i = 0; i < 4; ++i) {
        if (fgets(buf, sizeof(buf), pipe_p4)) {
            fputs(buf, pipe_p5);
        }
    }

    // Close all pipes
    pclose(pipe_p2);
    pclose(pipe_p3);
    pclose(pipe_p4);
    pclose(pipe_p5);

    return 0;
}

