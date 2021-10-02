#include "lib/common.h"

#define SRV_PORT 12345
#define BUF_SIZE 4096
#define SHUTDOWN "shutdown"
#define CLOSE "close"

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: grace_client <ip address>");
    }
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SRV_PORT);
    inet_pton(AF_INET, argv[1], &server_address.sin_addr);

    socklen_t server_len = sizeof(server_address);
    int ret = connect(socket_fd, (struct sockaddr *) &server_address, server_len);
    if (ret < 0) {
        error(1, errno, "connect failed");
    }

    char send_buffer[BUF_SIZE];
    char receive_buffer[BUF_SIZE + 1];
    fd_set read_mask;
    fd_set all_reads;
    FD_ZERO(&all_reads);
    FD_SET(0, &all_reads);
    FD_SET(socket_fd, &all_reads);
    while (1) {
        read_mask = all_reads;
        //使用select多路复用观测连接套接字和标准输入的I/O事件
        ret = select(socket_fd + 1, &read_mask, NULL, NULL, NULL);
        if (ret <= 0)
            error(1, errno, "select failed");

        if (FD_ISSET(socket_fd, &read_mask)) {
            size_t num = read(socket_fd, receive_buffer, BUF_SIZE);
            if (num < 0) {
                error(1, errno, "read error");
            } else if (num == 0) {
                error(1, 0, "server terminated\n");
            }
            receive_buffer[num] = 0;
            fputs(receive_buffer, stdout);
            fputs("\n", stdout);
        }

        if (FD_ISSET(0, &read_mask)) {
            if (fgets(send_buffer, BUF_SIZE, stdin) != NULL) {
                if (strncmp(send_buffer, SHUTDOWN, sizeof(SHUTDOWN)) == 0) {
                    FD_CLR(0, &all_reads);
                    if (shutdown(socket_fd, SHUT_WR)) {
                        error(1, errno, "shutdown failed");
                    }
                } else if (strncmp(send_buffer, CLOSE, sizeof(CLOSE)) == 0) {
                    FD_CLR(0, &all_reads);
                    if (close(socket_fd)) {
                        error(1, errno, "close failed");
                    }
                    sleep(6);
                    exit(0);
                } else {
                    size_t i = strlen(send_buffer);
                    if (send_buffer[i - 1] == '\n') {
                        send_buffer[i - 1] = 0;
                    }

                    printf("now sending %s\n", send_buffer);
                    size_t num = write(socket_fd, send_buffer, strlen(send_buffer));
                    if (num < 0) {
                        error(1, errno, "write failed");
                    }
                    printf("send bytes: %zu\n", num);
                }
            }
        }
    }
}
