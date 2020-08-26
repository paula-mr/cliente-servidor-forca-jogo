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
        printf("Argumentos passados incorretos. Necessário especificar endereço e porta.");
        exit(EXIT_FAILURE);
    }

	struct sockaddr_storage storage;
	if (addrparse(argv[1], argv[2], &storage) != 0) {
		printf("Argumentos passados incorretos. Necessário especificar endereço e porta.");
        exit(EXIT_FAILURE);
	}

	int sock = socket(storage.ss_family, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Erro ao inicializar socket.");
		exit(EXIT_FAILURE);
	}

	struct sockaddr *address = (struct sockaddr *)(&storage);
	if (connect(sock, address, sizeof(storage)) != 0) {
		printf("Erro ao conectar ao servidor.");
		exit(EXIT_FAILURE);
	}

	char addrstr[BUFSZ];
	addrtostr(address, addrstr, BUFSZ);
	printf("Conectado em %s\n", addrstr);

	char buffer[BUFSZ];

	unsigned total = 0;
	memset(buffer, 0, BUFSZ);
	while(1) {
		printf("Waiting for ack message\n");
		size_t count = recv(sock, buffer + total, BUFSZ - total, 0);
		if (count == 0) {
			printf("Conexão fechada.\n");
			break;
		}
		total += count;
	}

	printf("%s\n", buffer);
	printf("received %u bytes\n", total);

	int typeMessage = buffer[0] - '0';
	int wordSize = 0;
	for (int i=1; i<total-1; i++) {
		wordSize *= 10;
		wordSize += buffer[i] - '0';
	}

	printf("Received message type %d with word size %d\n", typeMessage, wordSize);
	printf("Guess the word!\n");

	for (int i=0; i<wordSize; i++) {
		printf("_ ");
	}
	printf("\n");

	while (typeMessage != 4) {
		memset(buffer, 0, BUFSZ);
		printf("digite a letra> ");
		fgets(buffer, BUFSZ-1, stdin);
		size_t count = send(sock, buffer, strlen(buffer)+1, 0);
		if (count != strlen(buffer)+1) {
			logexit("send");
		}
	}

	close(sock);
	exit(EXIT_SUCCESS);
}