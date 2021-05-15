#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv){
	struct addrinfo* result;
	struct addrinfo hints;

	memset((void *) &hints, 0, sizeof(struct addrinfo));

	hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
	hints.ai_family   = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; //Para TCP

	int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
	if(rc!=0) {
		std::cout << gai_strerror(rc) << std::endl;
	}

	int sd = socket(result->ai_family, result->ai_socktype, 0);
	if(sd<0) {
		std::cout << gai_strerror(sd) << std::endl;
	}

	bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);
	listen(sd, 5); //Comenzamos la escucha

	bool quit = false;
	while (!quit) {
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		//Establecemos la conezión con el cliente
		struct sockaddr cliente;
		socklen_t cliente_len = sizeof(cliente);
		int cliente_sd = accept(sd, (struct sockaddr *) &cliente, &cliente_len);
		
		getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
		printf("Conexión desde Host:%s Puerto:%s\n",host, serv);

		//Hasta que el cliente se desconecte esperamos a que mande un mensaje y se lo mandamos de vuelta
		while(!quit){
			int len = 100;
			char buffer[100];
			int bytesReceived = recv(cliente_sd, buffer, sizeof(buffer), 0);

			//Si el mensaje es Q cerramos el programa, si no mandamos el mensaje
			if (strcmp(buffer, "Q") == 0 || strcmp(buffer, "q") == 0){
				quit = true;
			}
			else send(cliente_sd, buffer, bytesReceived, 0);
		}
		std::cout << "Conexión terminada\n";
		close(cliente_sd);
	}
	freeaddrinfo(result);
	close(sd);
}