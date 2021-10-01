#include "lib/common.h"

#define BUF_SIZE 4096

int main(int argc, char **argv) {
    if (argc != 3) {
        error(1, 0, "usage: unix_data_client <client_local_path> <server_local_path>");
    }

    int socket_fd;
    struct sockaddr_un client_address, server_address;

    socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        error(1, errno, "create socket failed");
    }

    bzero(&client_address, sizeof(client_address));
    client_address.sun_family = AF_LOCAL;
    strcpy(client_address.sun_path, argv[1]);
    //将本地套接字bind到本地路径，UDP客户端程序不需要这么做
    //本地数据报套接字需要指定一个本地路径，以便服务器端回包时，可以正确找到地址
    int ret = bind(socket_fd, (struct sockaddr *) &client_address, sizeof(client_address));
    if (ret < 0) {
        error(1, errno, "bind failed");
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sun_family = AF_LOCAL;
    strcpy(server_address.sun_path, argv[2]);

    char send_buffer[BUF_SIZE];
    bzero(send_buffer, BUF_SIZE);
    char receive_buffer[BUF_SIZE];
    while (fgets(send_buffer, BUF_SIZE, stdin) != NULL) {
        size_t i = strlen(send_buffer);
        if (send_buffer[i - 1] == '\n') {
            send_buffer[i - 1] = 0;
        }
        size_t len = strlen(send_buffer);
        printf("now sending %s \n", send_buffer);

        ssize_t num = sendto(socket_fd, send_buffer, len, 0, (struct sockaddr *) &server_address, sizeof(server_address));
        if (num != len)
            error(1, errno, "sendto error");

        num = recvfrom(socket_fd, receive_buffer, BUF_SIZE, 0, NULL, NULL);
        receive_buffer[num] = 0;

        fputs(receive_buffer, stdout);
        fputs("\n", stdout);
    }

    exit(0);
}