#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

int make_socket(uint16_t port) {
    int sock;
    struct sockaddr_in address;

    // 创建字节流类型的IPV4 Socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 绑定到port和ip
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    // 通用地址
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    // 把IPV4地址转换成通用地址格式，同时传递长度
    if (bind(sock, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return sock;
}

int main() {
    int socket_fd = make_socket(12345);
    printf("socket_fd:%d", socket_fd);

    exit(0);
}