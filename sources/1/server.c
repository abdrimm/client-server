#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

enum errors {
    OK,
    ERR_INCORRECT_ARGS,
    ERR_SOCKET,
    ERR_SETSOCKETOPT,
    ERR_BIND,
    ERR_LISTEN
};

int init_socket(int port) {
    //open socket, return socket descriptor
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Fail: open socket");
        _exit(ERR_SOCKET);
    }
 
    //set socket option
    int socket_option = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_option,	            (socklen_t)sizeof(socket_option));
    if (server_socket < 0) {
        perror("Fail: set socket options");
        _exit(ERR_SETSOCKETOPT);
    }
    //set socket address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *) &server_address,	        (socklen_t)sizeof(server_address)) < 0) {
        perror("Fail: bind socket address");
        _exit(ERR_BIND);
    }

    //listen mode
    if (listen(server_socket, 5) < 0) {
        perror("Fail: bind socket address");
        _exit(ERR_LISTEN);
    }
    return server_socket;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        puts("Incorrect args.");
        puts("./server <port> <clients>");
        puts("Example:");
        puts("./server 5000 1");
        return ERR_INCORRECT_ARGS;
    }
    int port = atoi(argv[1]);
    int clients = atoi(argv[2]);
    int *client_socket = malloc(clients * sizeof(int));
    int server_socket = init_socket(port);
    puts("Wait for connection");
    struct sockaddr_in client_address;
    socklen_t size;
    char data;
    for (int i = 0; i < clients; i++) {
        client_socket[i] = accept(server_socket, 
                               (struct sockaddr *) &client_address,
                               &size);
    }
    while(1) {
        for(int i = 0; i < clients; i++) {
            int ret = read(client_socket[i], &data, 1);
            if(ret <= 0)
                break;
            printf("client %d: %c\n", i + 1, data);
        }
    }
    for(int i = 0; i < clients; i++) {
        close(client_socket[i]);
    }
    free(client_socket);
    return OK;
}
