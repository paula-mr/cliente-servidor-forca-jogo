#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

int main(int argc, char **argv) {
	if (argc < 3) {
        printf("Argumentos passados incorretos. Necessário especificar tipo e porta.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_storage storage;
	if (server_sockaddr_init(argv[1], argv[2], &storage) != 0) {
		printf("Argumentos passados incorretos. Necessário especificar tipo de endereço e porta.");
        exit(EXIT_FAILURE);
	}

	int sock = socket(storage.ss_family, SOCK_STREAM, 0);
	if (socket == -1) {
		printf("Erro ao inicializar socket.");
		exit(EXIT_FAILURE);
	}

    struct sockaddr *address = (struct sockaddr *)(&storage);
    if (bind(sock, address, sizeof(storage)) != 0) {
        printf("Erro ao realizar bind do servidor.");
		exit(EXIT_FAILURE);
    }

    if (listen(sock, 10) != 0) {
        printf("Erro ao realizar escutar requisições.");
		exit(EXIT_FAILURE);
    }

    char addrstr[BUFSZ];
	addrtostr(address, addrstr, BUFSZ);
	printf("Bound em %s\n", addrstr);

    while(1) {
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddress = (struct sockaddr *)(&clientStorage);
        socklen_t clientAddressLength = sizeof(cstorage);

        int clientSocket = accept(sock, clientAddress, &clientAddressLength);

        if (clientSocket == -1) {
            printf("Erro ao conectar com o cliente.");
		    exit(EXIT_FAILURE);
        }

        char clientAddrstr[BUFSZ];
        addrtostr(clientAddress, clientAddrstr, BUFSZ);
        printf("Bound em %s\n", clientAddrstr);

        char buffer[BUFSZ];
        memset(buffer, 0, BUFSZ);
        size_t count = recv(clientSocket, buffer, BUFSZ, 0);
        printf(buffer);
    }

	exit(EXIT_SUCCESS);
}