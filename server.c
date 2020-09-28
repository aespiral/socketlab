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

char* inputs[256]; // global; uso específico neste exemplo (universal server)

int install_tcp_server_socket(struct sockaddr_in* addr) {
    int s = socket(PF_INET, SOCK_STREAM, 0);

    int res_bind = bind(s,(struct sockaddr*) addr, sizeof(struct sockaddr_in));
    if (res_bind < 0) {
        perror("bind");
        exit(2);
    }
    int res_listen = listen(s, 10);
    if (res_listen <0) {
	perror("listen");
	exit(2);
    }
    return s;
}

void treat_connection(int server, int client);

// treat_connection é uma global, pois C não tem funções de ordem superior
void tcp_server(struct sockaddr_in* addr) {
    signal(SIGPIPE, SIG_IGN);
    int server_sock = install_tcp_server_socket(addr);
    int client;

    socklen_t addr_size = sizeof(struct sockaddr_in);
    
    while(1) {
        client = accept(server_sock, (struct sockaddr*) addr, &addr_size);
	if (client < 0) {
	    perror("accept");
	    exit(2);
	}
	treat_connection(server_sock, client);
    }
}    

void service(int client_sock);

void fork_treatment(int server, int client) {
    
    if (fork() == 0) {

        close(server);
	service(client);
	exit(0);

    }

    close(client); // desative esta linha e observe o efeito com `ps aux | grep client`
}

void double_fork_treatment(int server, int client) {

    int k, status;

    k = fork();

    if (k == 0) {

	if (fork() != 0) {
            // processo filho do original não tem função, e apenas termina 
            exit(0); 
	} else {
	    // processo neto do original executa o serviço, e termina
	    close(server);
	    service(client);
	    exit(0);
	}

    } else {
	// processo original espera o filho terminar e retorna
	waitpid(k, &status, 0);

    }

    close(client);
}

void treat_connection(int server, int client) {
    // PENDENTE: registro das conexões (logging)
    fork_treatment(server, client);
}

int
main(int argc, char** argv) {
    
    if (argc < 2) {
        puts("./server <porta> <comando> [arg1 ... argn]");
        exit(2);
    }

    int port = atoi(argv[1]);

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

    tcp_server(&addr);

}

void service(int c) {
    dup2(c, 0);
    dup2(c, 1);
    dup2(c, 2);
    close(c);

    execvp(inputs[0], inputs);
}


