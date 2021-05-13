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
        std::cout << gai_strerror(rc) << std::endl;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);
    if(sd<0) {
        std::cout << gai_strerror(sd) << std::endl;
    }

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

    bool quit = false;
    while (!quit) {
        //Definimos el buffer para recibir mensajes y declaramos el sockaddr del cliente
        int len = 80;
        char buffer[len] = "";
        struct sockaddr cliente;
        socklen_t client_len = sizeof(cliente);

        //Recibimos el mensaje del cliente
        int bytes = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliente, &client_len);
        buffer[bytes]='\0'; 
            
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        //Tomamos los datos de fexha y hora
        time_t timeData;
        time(&timeData);

        //Mensaje que le mandaremos al cliente
        char timeMessage[80];
        size_t bytesToSend;

        //Sacamos la info del cliente y luego la imprimimos
        getnameinfo((struct sockaddr *) &cliente, client_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
        printf("%d bytes de %s:%s\n", bytes, host, serv);

        //Dependiendo del mensaje recibido realizamos una u otra acción
        switch (buffer[0])
        {
            case 't':
                bytesToSend = strftime(timeMessage, sizeof(timeMessage), "%T", localtime(&timeData));
                sendto(sd, timeMessage, bytesToSend, 0, (struct sockaddr *)&cliente, client_len);
                break;
            case 'd':
                bytesToSend = strftime(timeMessage, sizeof(timeMessage), "%F", localtime(&timeData));
                sendto(sd, timeMessage, bytesToSend, 0, (struct sockaddr *)&cliente, client_len);
                break;
            case 'q':
                sendto(sd, "Servidor cerrado.", sizeof("Servidor cerrado."), 0, (struct sockaddr *)&cliente, client_len);
                std::cout << "Cerrando el servidor...\n";
                quit = true;
                break;
            default:
                char errormsg[23] = "Comando no soportado ";
                errormsg[22] = buffer[0];
                std::cout << "Comando no soportado " << buffer[0] << std::endl;
                sendto(sd, errormsg, sizeof(errormsg), 0, (struct sockaddr *)&cliente, client_len);
                break;
        }
    }
}