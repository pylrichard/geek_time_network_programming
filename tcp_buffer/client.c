#include "lib/common.h"

#define BUF_SIZE 102400

#define SRV_PORT 12345

void send_data(int socket_fd) {
    char* data;
    data = malloc(BUF_SIZE + 1);
    for (int i = 0; i < BUF_SIZE; i++) {
        data[i] = 'a';
    }
    data[BUF_SIZE] = '\0';

    const char* ptr;
    ptr = data;
    size_t remaining = strlen(data);
    while (remaining) {
        int written = send(socket_fd, ptr, remaining, 0);
        fprintf(stdout, "send into buffer %ld \n", written);
        if (written <= 0) {
            error(1, errno, "send failed");
            return;
        }
        remaining -= written;
        ptr += written;
    }
}

int main(int argc, char **argv) {
    int socket_fd;
    struct sockaddr_in address;

    if (argc != 2) {
        error(1, 0, "usage: tcp_buffer_client <ip address>");
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(SRV_PORT);
    inet_pton(AF_INET, argv[1], &address.sin_addr);
    int ret = connect(socket_fd, (struct sockaddr *) &address, sizeof(address));
    if (ret < 0) {
        error(1, errno, "connect failed ");
    }

    send_data(socket_fd);

    exit(0);
}
