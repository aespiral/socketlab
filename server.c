#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>


int
main(int argc, char** argv) {
    
    if (argc < 2) {
        puts("./server <porta> <comando> [arg1 ... argn]");
        exit(2);
    }

    int port = atoi(argv[1]);

    char* inputs[argc - 2];
    int i;
    for (i = 0; i < argc - 2; i++)
        inputs[i] = argv[i + 2];
    inputs[i] = NULL;    

    struct in_addr host;
    inet_aton("127.0.0.1", &host);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr = host;
    addr.sin_port = htons(port);

    signal(SIGPIPE, SIG_IGN);
    int s = socket(PF_INET, SOCK_STREAM, 0);

    int res_bind = bind(s,(struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if (res_bind < 0) {
        perror("bind");
        exit(2);
    }
    int res_listen = listen(s, 10);
    if (res_listen < 0) {
	perror("listen");
	exit(2);
    }
    
    int c;

    socklen_t addr_size = sizeof(struct sockaddr_in);
    
    while(1) {
        c = accept(s, (struct sockaddr*) &addr, &addr_size);
	if (c < 0) {
	    perror("accept");
	    exit(2);
	}
    
        if (fork() == 0) {

            close(s);
           
	    dup2(c, 0);
            dup2(c, 1);
            dup2(c, 2);
            close(c);

            execvp(inputs[0], inputs);

	    exit(0);
        }

        close(c);
    }
}    

