#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    struct addrinfo* sol; //El struct con la solución
    struct addrinfo hints; //Filtros para la búsqueda
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    memset((void *) &hints, 0, sizeof(struct addrinfo));

    //Obtenemos la información
    int rc = getaddrinfo(argv[1], argv[2], &hints, &sol);

    //Si ha habido algún error se muestra por pantalla
    if(rc!=0){
        std::cerr << "Error: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    //Mostramos todos los posibles servidores de la solución
    for(auto i=sol; i!=nullptr; i = i->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST,
        serv, NI_MAXSERV, NI_NUMERICHOST | AF_UNSPEC);

        std::cout << host << " " << i->ai_family << " " << i->ai_socktype << std::endl;
    }

    freeaddrinfo(sol);

    return 0;
}
