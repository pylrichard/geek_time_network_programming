#include  "lib/common.h"

#define BACKLOG_SIZE    1024
#define BUF_SIZE        4096

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: server <local_path>");
    }

    int listen_fd, conn_fd;
    socklen_t client_len;
    struct sockaddr_un client_address, server_address;

    //套接字类型AF_LOCAL和AF_UNIX等价
    listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        error(1, errno, "create socket failed");
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sun_family = AF_LOCAL;
    char *local_path = argv[1];
    //unlink把存在的文件删除，保持幂等性
    unlink(local_path);
    //设置一个本地文件路径
    strcpy(server_address.sun_path, local_path);

    int ret = bind(listen_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        error(1, errno, "bind failed");
    }

    ret = listen(listen_fd, BACKLOG_SIZE);
    if (ret < 0) {
        error(1, errno, "listen failed");
    }

    client_len = sizeof(client_address);
    conn_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client_len);
    if (conn_fd < 0) {
        error(1, errno, "accept failed");
    }

    char receive_buffer[BUF_SIZE];
    char send_buffer[BUF_SIZE];
    while (1) {
        bzero(receive_buffer, BUF_SIZE);
        if (read(conn_fd, receive_buffer, BUF_SIZE) == 0) {
            printf("client quit");
            break;
        }
        printf("receive: %s", receive_buffer);

        bzero(send_buffer, BUF_SIZE);
        sprintf(send_buffer, "hi, %s", receive_buffer);

        int size = sizeof(send_buffer);
        if (write(conn_fd, send_buffer, size) != size)
            error(1, errno, "write error");
    }

    close(listen_fd);
    close(conn_fd);
    exit(0);
}