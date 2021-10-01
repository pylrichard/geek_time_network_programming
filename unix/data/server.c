#include  "lib/common.h"

#define BUF_SIZE 4096

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: unix_data_server <local_path>");
    }

    int socket_fd;
    socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        error(1, errno, "create socket failed");
    }

    struct sockaddr_un server_address;
    char *local_path = argv[1];
    unlink(local_path);
    bzero(&server_address, sizeof(server_address));
    server_address.sun_family = AF_LOCAL;
    strcpy(server_address.sun_path, local_path);

    int ret = bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (ret < 0) {
        error(1, errno, "bind failed");
    }

    char receive_buffer[BUF_SIZE];
    char send_buffer[BUF_SIZE];
    struct sockaddr_un client_address;
    socklen_t client_len = sizeof(client_address);
    while (1) {
        bzero(receive_buffer, BUF_SIZE);
        ssize_t num = recvfrom(socket_fd, receive_buffer, BUF_SIZE, 0, (struct sockaddr *) &client_address, &client_len);
        if (num == 0) {
            printf("client quit");
            break;
        }
        printf("receive: %s\n", receive_buffer);

        bzero(send_buffer, BUF_SIZE);
        sprintf(send_buffer, "hi, %s", receive_buffer);

        size_t len = strlen(send_buffer);
        printf("now sending: %s \n", send_buffer);

        num = sendto(socket_fd, send_buffer, len, 0, (struct sockaddr *) &client_address, client_len);
        if (num != len)
            error(1, errno, "sendto error");
    }

    close(socket_fd);
    exit(0);
}