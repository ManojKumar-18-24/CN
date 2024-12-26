#include <stdio.h>
#include <stdlib.h>
#include <sys/capability.h>
#include <unistd.h>

int main() {
    const char* target_ip = "172.20.183.44";  // Target IP to block
    cap_t caps = cap_get_pid(getpid());  // Get the capabilities of the current process
    cap_value_t cap_net_admin = CAP_NET_ADMIN;
    cap_flag_value_t flag;

    // Check if the process has the CAP_NET_ADMIN capability
    if (cap_get_flag(caps, cap_net_admin, CAP_EFFECTIVE, &flag) == 0 && flag == CAP_SET) {
        printf("CAP_NET_ADMIN capability granted. Blocking IP address %s...\n", target_ip);

        // Block the IP using iptables (blocking outgoing traffic to target_ip)
        char command[256];
        snprintf(command, sizeof(command), "sudo iptables -A OUTPUT -d %s -j REJECT", target_ip);
        if (system(command) == -1) {
            perror("Error blocking IP address");
        } else {
            printf("IP address %s blocked successfully.\n", target_ip);
        }
    } else {
        printf("CAP_NET_ADMIN capability not granted. Cannot perform network operations.\n");
    }

    cap_free(caps);
    return 0;
}

