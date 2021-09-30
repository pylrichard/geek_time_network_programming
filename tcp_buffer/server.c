#include "lib/common.h"

#define BUF_SIZE 1024
#define SRV_PORT 12345
#define BACKLOG_SIZE 1024

void read_data(int socket_fd) {
    size_t n;
    char buf[BUF_SIZE];

    int time = 0;
    while (1) {
        fprintf(stdout, "block in read\n");
        n = readn(socket_fd, buf, BUF_SIZE);
        if (n == 0)
            return;

        time++;
        fprintf(stdout, "1k read for %d \n", time);
        //每次读取1k数据，休眠1秒模拟服务端处理时延
        usleep(1000);
    }
}

int main() {
    int listen_fd, conn_fd;
    socklen_t client_len;
    struct sockaddr_in client_address, server_address;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SRV_PORT);

    //bind到本地地址，端口为12345
    bind(listen_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    listen(listen_fd, BACKLOG_SIZE);

    /* 循环处理客户端请求 */
    while (1) {
        client_len = sizeof(client_address);
        conn_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client_len);
        read_data(conn_fd);
        //关闭连接套接字，注意不是监听套接字
        close(conn_fd);
    }
}