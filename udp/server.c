#include "lib/common.h"

#define SRV_PORT 12345
#define BUF_SIZE 4096

static int packet_count;

static void interrupt_handler(int signal) {
    //打印接收的报文总数
    printf("\nreceived %d datagrams\n", packet_count);
    exit(0);
}

int main() {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SRV_PORT);

    bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));

    socklen_t client_len;
    char receive_buffer[BUF_SIZE];
    packet_count = 0;

    //响应Ctrl+C
    signal(SIGINT, interrupt_handler);

    struct sockaddr_in client_address;
    client_len = sizeof(client_address);
    while (1) {
        int num = recvfrom(socket_fd, receive_buffer, BUF_SIZE, 0, (struct sockaddr *) &client_address, &client_len);
        receive_buffer[num] = 0;
        printf("received %d bytes: %s\n", num, receive_buffer);

        char send_buffer[BUF_SIZE];
        sprintf(send_buffer, "hi, %s", receive_buffer);
        sendto(socket_fd, send_buffer, strlen(send_buffer), 0, (struct sockaddr *) &client_address, client_len);

        packet_count++;
    }
}