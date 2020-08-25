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

	int socket = socket(storage.ss_family, SOCK_STREAM, 0);
	if (socket == -1) {
		printf("Erro ao inicializar socket.");
		exit(EXIT_FAILURE);
	}

	struct sockaddr *address = (struct sockaddr *)(storage);
	if (connect(socket, address, sizeof(storage)) != 0) {
		printf("Erro ao conectar ao servidor.");
		exit(EXIT_FAILURE);
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);
	printf("Conectado em %s\n", addrstr);

	char buffer[BUFSZ];
	memset(buf, 0, BUFSZ);

	unsigned total = 0;
	while(1) {
		count = recv(s, buf + total, BUFSIZE - total, 0);
		if (count == 0) {
			printf("Conexão fechada.")
			break;
		}
		total += count;
	}

	printf("received %u bytes\n", total);
	puts(buffer);

	exit(EXIT_SUCCESS);
}