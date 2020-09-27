#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

int
main(int argc, char** argv) {
    
    if (argc < 2) {
        puts("./server <porta> <comando> [arg1 ... argn]");
        exit(2);
    }

    int port = atoi(argv[1]);

    char* args[3] = {"ls", "-l", NULL};

    struct in_addr host;
    inet_aton("127.0.0.1", &host);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr = host;
    addr.sin_port = htons(port);

    //==========================================================================
    //install_tcp_server_socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    int res_bind = bind(sock,(struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if (res_bind < 0) {
        perror("bind");
        exit(2);
    }
    int res_listen = listen(sock, 10);
    if (res_listen <0) {
	perror("listen");
	exit(2);
    }
    //fim de install_tcp_server_socket com resultado em sock
    
    //==========================================================================
    //tcp_server
    signal(SIGPIPE, SIG_IGN);
    int server_sock = sock;
    int client;

    while(1) {
        client = accept(server_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)i);
	if (client < 0) {
	    perror("accept");
	    exit(2);
	}

	//---------------------------------------------------------------------
	

    }

}

