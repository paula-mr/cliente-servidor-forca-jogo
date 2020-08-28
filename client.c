#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

int createSocket(char **argv, struct sockaddr_storage *storage) {
	if (addrparse(argv[1], argv[2], storage) != 0) {
		printf("Argumentos passados incorretos. Necessário especificar endereço e porta.");
        exit(EXIT_FAILURE);
	}
	
	printf("%d\n", storage->ss_family);
	int sock = socket(storage->ss_family, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Erro ao inicializar socket.");
		exit(EXIT_FAILURE);
	}

	return sock;
}

struct sockaddr* connectSocket(int sock, struct sockaddr_storage storage) {
	struct sockaddr* address = (struct sockaddr *)(&storage);
	if (connect(sock, address, sizeof(storage)) != 0) {
		printf("Erro ao conectar ao servidor.");
		exit(EXIT_FAILURE);
	}

	char addrstr[BUFSZ];
	addrtostr(address, addrstr, BUFSZ);
	printf("Conectado em %s\n", addrstr);

	return address;
}

int receiveAcknowledgmentMessage(int sock) {
	char buffer[BUFSZ];

	memset(buffer, 0, BUFSZ);
	printf("Waiting for ack message\n");
	size_t count = recv(sock, buffer, BUFSZ, 0);

	printf("%s\n", buffer);
	printf("received %u bytes\n", count);

	int typeMessage = buffer[0] - '0';
	int wordSize = 0;
	for (int i=1; i<count-1; i++) {
		wordSize *= 10;
		wordSize += buffer[i] - '0';
	}

	printf("Received message type %d with word size %d\n", typeMessage, wordSize);

	return wordSize;
}

void printWord(char* word, int wordSize) {
	for (int i=0; i<wordSize; i++) {
		printf("%c ", word[i]);
	}
	printf("\n");
}

char guessLetter(int sock) {
	char buffer[BUFSZ];

	memset(buffer, 0, BUFSZ);
	printf("\ndigite a letra> ");
	char letter = getchar();
	buffer[0] = '1';
	buffer[1] = letter;
	buffer[2] = '\0';

	size_t count = send(sock, buffer, 3, 0);
	if (count != 3) {
		logexit("send");
	}

	return letter;
}

int receiveAnswer(int sock, char letter, char* word) {
	char buffer[BUFSZ];
	int occurrences[BUFSZ];

    memset(occurrences, 0, BUFSZ);
	memset(buffer, 0, BUFSZ);

	printf("Waiting for answer\n");
	size_t count = recv(sock, buffer, BUFSZ, 0);

	printf("%s\n", buffer);
	printf("received %u bytes\n", count);

	int typeMessage = buffer[0] - '0';
	if (count < 3) {
		if (typeMessage == 4) {
			for (int i=0; i<strlen(word); i++) {
				if (word[i] == '_')
					word[i] = letter;
			}
		}
	} else {
		int letterOccurrences = buffer[1];

		for (int i=0; i<letterOccurrences; i++) {
			int occurrence = buffer[i+2];
			word[occurrence] = letter;
		}
	}

	return typeMessage;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Argumentos passados incorretos. Necessário especificar endereço e porta.");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_storage storage;
	int sock = createSocket(argv, &storage);
	connectSocket(sock, storage);

	int wordSize = receiveAcknowledgmentMessage(sock);
	char word[BUFSZ];
	for(int i=0; i<wordSize; i++) {
		word[i] = '_';
	}
	word[wordSize] = '\0';

	printf("Guess the word!\n");
	printWord(word, wordSize);

	int typeMessage = 1;
	while (typeMessage != 4) {
		char letter = guessLetter(sock);
		receiveAnswer(sock, letter, word);
		printWord(word, wordSize);
	}

	printf("You won!");

	close(sock);
	exit(EXIT_SUCCESS);
}
