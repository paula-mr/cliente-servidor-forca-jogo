#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024
#define WORD "COMUNICACAO"

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
	if (sock == -1) {
		printf("Erro ao inicializar socket.");
		exit(EXIT_FAILURE);
	}

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) {
        logexit("setsockopt");
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
        socklen_t clientAddressLength = sizeof(clientStorage);

        int clientSocket = accept(sock, clientAddress, &clientAddressLength);

        if (clientSocket == -1) {
            printf("Erro ao conectar com o cliente.");
		    exit(EXIT_FAILURE);
        }

        char clientAddrstr[BUFSZ];
        addrtostr(clientAddress, clientAddrstr, BUFSZ);
        printf("Bound em %s\n", clientAddrstr);

        printf("sending first message\n");

        char buffer[BUFSZ];
        sprintf(buffer, "%d%u", 1, strlen(WORD));
        printf("%s\n", buffer);
        printf("size: %u\n", strlen(WORD));
        size_t count = send(clientSocket, buffer, strlen(buffer) + 1, 0);
        if (count != strlen(buffer) + 1) {
            logexit("send");
        }
        printf("message sent\n");

        int isWordComplete = 0;
        printf("Waiting for user guess\n");
        while (!isWordComplete) {
            unsigned total = 0;
            count = 0;
            memset(buffer, 0, BUFSZ);
            
            count = recv(clientSocket, buffer, BUFSZ, 0);
            if (count > 0 && count < 100) {
                printf("%s\n", buffer);
                printf("received %u bytes\n", count);

                int typeMessage = buffer[0] - '0';
                char letter = buffer[1];

                printf("Received message type %d with letter %c\n", typeMessage, letter);
            }

        }


        close(clientSocket);
    }

	exit(EXIT_SUCCESS);
}
