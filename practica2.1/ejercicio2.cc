#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    struct addrinfo* result;
    struct addrinfo hints;

    memset((void *) &hints, 0, sizeof(struct addrinfo));

    hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family   = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(rc!=0) {
        
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

    bool quit = false;
    while (!quit) {
        int len = 80;
        char buffer[len] = "";
        struct sockaddr cliente;
        socklen_t client_len = sizeof(cliente);

        int bytes = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliente, &client_len);
        buffer[bytes]='\0'; 
            
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo((struct sockaddr *) &cliente, client_len, host, NI_MAXHOST,
            serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

        printf("Conexión desde Host:%s Puerto:%s\n",host, serv);
        printf("\tMensaje(%i): %s\n", bytes, buffer);

        sendto(sd, buffer, bytes, 0, (struct sockaddr *) &cliente, client_len);
    }
}