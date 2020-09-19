#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void retransmit(int fdin, int fdout) {
    char buffer[4096];
    int res;
    do {
        res = read(fdin, buffer, 4096);
        if (res != 0) {
            write(fdout, buffer, res);
        }
    } while (res != 0);
}

int
main(int argc, char** argv) {
#define server_name argv[1]
#define port_number atoi(argv[2])

    if (argc < 3) {
        printf("Como usar: ./client <url> <porta>\n");
        exit(2);
    }

    struct hostent* host = gethostbyname(server_name);
    if (host == NULL) {
        herror("Problemas com o servidor");
        exit(2);
    }
    /* Para explorar
    printf("Nome do servidor: %s\n", host->h_name);
    printf("Endereço IP: %s\n", inet_ntoa(*(struct in_addr*)host->h_addr));
    printf("Endereços: ");
    struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
    int i;
    for(i = 0; addr_list[i] != NULL; i++) {
        printf("%s ", inet_ntoa(*addr_list[i]));
    }
    printf("\n");
    */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // Comente essa linha e experimente
    server_addr.sin_addr = *((struct in_addr *) host->h_addr_list[0]);
    server_addr.sin_port = htons(port_number);

    int sock = socket(PF_INET, SOCK_STREAM, 0);

    int res_conn = connect (sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
    if (res_conn == -1) {
        perror ("connect");
        exit(2);
    }

    if (fork() == 0) {
        retransmit(0, sock);
        shutdown(sock, SHUT_WR);
        exit(0);
    } else {
        retransmit(sock, 1);
        close(1);
        int status;
        wait(&status);
    }
}