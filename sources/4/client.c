#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

enum errors {
    OK,
    ERR_INCORRECT_ARGS,
    ERR_SOCKET,
    ERR_CONNECT
};

char *get_word(int *size) {
    char *answ = NULL;
    char c = getchar();
    int cnt = 0;
    while (c != '\n' && c != ' ') {
        cnt++;
        answ = realloc(answ, (cnt + 1) * sizeof(char));
        answ[cnt - 1] = c;
        c = getchar();
    }
    answ[cnt] = '\0';
    *size = cnt + 1;
    return answ;
}

int init_socket(const char *ip, int port) {
    // open socket, result is socket descriptor
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Fail: open socket");
        _exit(ERR_SOCKET);
    }

    // prepare server address
    struct hostent *host = gethostbyname(ip);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    memcpy(&server_address.sin_addr, host -> h_addr_list[0],
        sizeof(server_address));

    // connection
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
    pid_t pid[2];
    pid[0] = fork();
    if (pid[0] == 0) {
        char *word;
        int size_w;
        // for (word = get_word(&size_w);
        // strcmp(word, "exit") && strcmp(word, "quit");
        // word = get_word(&size_w))
        while (1) {
                word = get_word(&size_w);
                write(server, word, size_w);
                // printf("Send word: ");
                // puts(word);
                free(word);
        }
    }
    pid[1] = fork();
    if (pid[1] == 0) {
        char *word = NULL;
        do {
            free(word);
            word = NULL;
            char ch;
            read(server, &ch, 1);
            printf("%d: ", ch + 1);
            read(server, &ch, 1);
            int w_size = 1;
            for ( ; ch != 0; w_size++) {
                word = realloc(word, sizeof(char) * w_size);
                word[w_size - 1] = ch;
                read(server, &ch, 1);
            }
            puts(word);
        } while (1);
    }
    waitpid(pid[0], 0, 0);
    waitpid(pid[1], 0, 0);
    close(server);
    return OK;
}
