#include "lib/common.h"

#define SRV_PORT 12345
#define BACKLOG_SIZE 1024
#define BUF_SIZE 4096

static int packet_count;

static void interrupt_handler(int signal) {
    printf("\nreceived %d tcp packet\n", packet_count);
    exit(0);
}

int main() {
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SRV_PORT);

    int ret = bind(listen_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        error(1, errno, "bind failed");
    }

    ret = listen(listen_fd, BACKLOG_SIZE);
    if (ret < 0) {
        error(1, errno, "listen failed");
    }

    signal(SIGINT, interrupt_handler);
    signal(SIGPIPE, SIG_DFL);

    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    int conn_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client_len);
    if (conn_fd < 0) {
        error(1, errno, "accept failed");
    }

    char receive_buffer[BUF_SIZE];
    char send_buffer[BUF_SIZE];
    packet_count = 0;

    while (1) {
        int num = read(conn_fd, receive_buffer, BUF_SIZE);
        if (num < 0) {
            error(1, errno, "read error");
        } else if (num == 0) {
            error(1, 0, "client closed\n");
        }
        receive_buffer[num] = 0;
        printf("received %d bytes: %s\n", num, receive_buffer);
        packet_count++;

        sprintf(send_buffer, "hi, %s", receive_buffer);

        sleep(5);

        num = send(conn_fd, send_buffer, strlen(send_buffer), 0);
        printf("send bytes: %d\n", num);
        if (num < 0) {
            error(1, errno, "write error");
        }
    }
}
