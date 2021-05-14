#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char **argv)
{
	struct addrinfo *result;
	struct addrinfo hints;

	memset((void *)&hints, 0, sizeof(struct addrinfo));

	hints.ai_flags = AI_PASSIVE; //Devolver 0.0.0.0
	hints.ai_family = AF_INET;	 // IPv4
	hints.ai_socktype = SOCK_STREAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (rc != 0)
	{
		std::cout << gai_strerror(rc) << std::endl;
	}

	int sd = socket(result->ai_family, result->ai_socktype, 0);
	if (sd < 0)
	{
		std::cout << gai_strerror(sd) << std::endl;
	}

	//Abrimos el socket
	bind(sd, result->ai_addr, result->ai_addrlen);

	//Tomamos la dirección del socket
	sockaddr servDir = *result->ai_addr;
	socklen_t servDirLen = sizeof(servDir);
	connect(sd, &servDir, servDirLen);

	bool quit = false;
	while(!quit){
		int len = 100;
		char mesg[len];
		std::cin >> mesg;

		//Enviamos el mensaje
		send(sd, mesg, sizeof(mesg), 0);

		//Si el mensaje es Q cerramos el programa
		if (strcmp(mesg, "Q") == 0 || strcmp(mesg, "q") == 0){
			quit = true;
		}

		//Recibimos la respuesta y la imprimimos en pantalla
		char answer[len];
		size_t bytesReceived = recv(sd, answer, sizeof(answer), 0);
		answer[bytesReceived] = '\0';
		std::cout << answer << '\n';
	}
	std::cout << "Cliente cerrado\n";
	freeaddrinfo(result);
	close(sd);
}