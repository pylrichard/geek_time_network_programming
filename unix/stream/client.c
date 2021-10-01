#include "lib/common.h"

#define BUF_SIZE 4096

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: client <local_path>");
    }

    int socket_fd;
    struct sockaddr_un server_address;

    socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        error(1, errno, "create socket failed");
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sun_family = AF_LOCAL;
    //本地套接字使用文件路径作为目标标识，sun_path代表目标文件路径
    strcpy(server_address.sun_path, argv[1]);

    //本地套接字不会有三次握手
    int ret = connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        error(1, errno, "connect failed");
    }

    char send_buffer[BUF_SIZE];
    char receive_buffer[BUF_SIZE];
    bzero(send_buffer, BUF_SIZE);

    while (fgets(send_buffer, BUF_SIZE, stdin) != NULL) {
        int size = sizeof(send_buffer);
        if (write(socket_fd, send_buffer, size) != size)
            error(1, errno, "write error");

        if (read(socket_fd, receive_buffer, BUF_SIZE) == 0)
            error(1, errno, "server terminated prematurely");

        fputs(receive_buffer, stdout);
    }

    exit(0);
}