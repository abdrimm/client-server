#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    ERR_CONNECT
};

int init_socket(const char *ip, int port) {
    //open socket, result is socket descriptor
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Fail: open socket");
        _exit(ERR_SOCKET);
    }

    //prepare server address
    struct hostent *host = gethostbyname(ip);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    memcpy(&server_address.sin_addr, host -> h_addr_list[0], sizeof(server_address));

    //connection
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    memcpy(&sin.sin_addr, host->h_addr_list[0], sizeof(sin.sin_addr));
    if (connect(server_socket, (struct sockaddr*) &sin, sizeof(sin)) < 0) {
        perror("Fail: connect");
        _exit(ERR_CONNECT);
    }
    return server_socket;
}

int read_line(int fd, char **string) {
    char *str = NULL;
    char ch;
    int i = 0, status;
    while (((status = read(fd, &ch, sizeof(char))) > 0) && (ch != '\0')) {
        if (i % 16 == 0) {
            str = realloc(str, 16 * ((i / 16) + 1) * sizeof(char));
        }
        str[i] = ch;
        i++;
    }
    str = realloc(str, (i + 1) * sizeof(char));
    str[i] = '\0';
    *string = str;
    return status;
}

void write_line(int fd, char *string) {
    int i = 0;
    while (string[i] != '\0') {
        write(fd, &string[i], 1);
        i++;
    }
    write(fd, &string[i], 1);
}



int main(int argc, char **argv) {
    if (argc != 3) {
        puts("Incorrect args.");
        puts("./client <ip> <port>");
        puts("Example:");
        puts("./client 127.0.0.1 5000");
        return ERR_INCORRECT_ARGS;
    }
    char *ip = argv[1];
    int port = atoi(argv[2]);
    int server = init_socket(ip, port);
    char nameOfFile[256];
    char *arr[6] = {"GET", " ", "HTTP/1.1\n", "Host:", " ", "\n"};
    arr[4] = argv[1];
    char *str[6] = {NULL};
    int len = 0;
    char *str_error = "HTTP/1.1 404\n";
    char *str_not_error = "HTTP/1.1\n";
    char *string = NULL;
    fgets(nameOfFile, 256, stdin);
    while (strcmp(nameOfFile, "exit\n") != 0) { 
        nameOfFile[strlen(nameOfFile) - 1] = '\0';
        arr[1] = nameOfFile;
        for(int i = 0; i < 6; i++) {
            write_line(server, arr[i]);
        }
        read_line(server, &str[0]);
        if ((strcmp(str[0], str_error) != 0) && (strcmp(str[0], str_not_error) != 0)) {
            read_line(server, &str[1]);
            read_line(server, &str[2]);
            read_line(server, &str[3]);
            read_line(server, &str[4]);
            read(server, &len, sizeof(int));
            read_line(server, &str[5]);
            read_line(server, &string);
            printf("%s%s%s%s%s%d%s%s\n", str[0], str[1], str[2], str[3], str[4], len, str[5], string);
        } else {
            printf("%s\n", str[0]);
        }
        arr[1] = NULL;
        fgets(nameOfFile, 256, stdin);
    }
    close(server);   
    return OK;
}