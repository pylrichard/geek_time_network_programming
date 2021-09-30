#include "lib/common.h"

#define BUF_SIZE 4096
#define SRV_PORT 12345

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: client <ip address>");
    }
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SRV_PORT);
    inet_pton(AF_INET, argv[1], &server_address.sin_addr);

    socklen_t server_len = sizeof(server_address);

    struct sockaddr *reply_address;
    reply_address = malloc(server_len);

    char send_buffer[BUF_SIZE], receive_buffer[BUF_SIZE + 1];
    socklen_t len;
    int num;

    struct timeval timeout = {1, 0};
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    while (fgets(send_buffer, BUF_SIZE, stdin) != NULL) {
        int i = strlen(send_buffer);
        if (send_buffer[i - 1] == '\n') {
            send_buffer[i - 1] = 0;
        }

        printf("now sending %s\n", send_buffer);
        size_t ret = sendto(socket_fd, send_buffer, strlen(send_buffer), 0, (struct sockaddr *) &server_address, server_len);
        if (ret < 0) {
            error(1, errno, "send failed ");
        }
        printf("send bytes: %zu \n", ret);

        len = 0;
        num = recvfrom(socket_fd, receive_buffer, BUF_SIZE, 0, reply_address, &len);
        if (num < 0)
            error(1, errno, "receive failed");
        receive_buffer[num] = 0;
        fputs(receive_buffer, stdout);
        fputs("\n", stdout);
    }

    exit(0);
}