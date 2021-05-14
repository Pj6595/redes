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

	//Abrimos el socket
	bind(sd, result->ai_addr, result->ai_addrlen);

	//Tomamos la dirección del socket
	sockaddr servDir = *result->ai_addr;
	socklen_t servDirLen = sizeof(servDir);

	//Enviamos el mensaje
	sendto(sd, argv[3], 1, 0, &servDir, servDirLen);

	//Recibimos la respuesta
	char answer[80];
	int bytesReceived = recvfrom(sd, answer, sizeof(answer), 0, &servDir, &servDirLen);
	answer[bytesReceived] = '\0';

	std::cout << answer << '\n';
}