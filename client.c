#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int createSocket(char **argv, struct sockaddr_storage *storage);
struct sockaddr* connectSocket(int sock, struct sockaddr_storage storage);
int receiveAcknowledgmentMessage(int sock);
char guessLetter(int sock);
int receiveAnswer(int sock, char letter, char* word);
void initializeWord(char* word, int wordSize);
void printWord(char* word, int wordSize);

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
	initializeWord(word, wordSize);

	printf("Adivinhe a palavra!\n");
	printWord(word, wordSize);

	int typeMessage = 1;
	while (typeMessage != 4) {
		char letter = guessLetter(sock);
		typeMessage = receiveAnswer(sock, letter, word);
		printWord(word, wordSize);
	}

	printf("Você ganhou!\n");

	close(sock);
	exit(EXIT_SUCCESS);
}

int createSocket(char **argv, struct sockaddr_storage *storage) {
	if (parseAddress(argv[1], argv[2], storage) != 0) {
		printf("Argumentos passados incorretos. Necessário especificar endereço e porta.");
        exit(EXIT_FAILURE);
	}
	
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

	printAddress(address);

	return address;
}

int receiveAcknowledgmentMessage(int sock) {
	char buffer[2];
	memset(buffer, 0, 2);
	
	printf("Esperando por mensagem de confirmação.\n");
	size_t count = recv(sock, buffer, 2, 0);

	int typeMessage = buffer[0];
	if (count != 2 || typeMessage != ACKNOWLEDGMENT_MESSAGE) {
		printf("Erro ao receber mensagem de confirmação.\n")
		exit(EXIT_FAILURE);
	}

	int wordSize = buffer[1];
	return wordSize;
}

char guessLetter(int sock) {
	char buffer[2];

	memset(buffer, 0, 2);
	printf("\nDigite a letra: ");
	char letter = getchar();
	buffer[0] = GUESS_MESSAGE;
	buffer[1] = letter;

	size_t count = send(sock, buffer, 2, 0);
	if (count != 2) {
		printf("Erro ao enviar letra de palpite.");
		exit(EXIT_FAILURE);
	}

	return letter;
}

int receiveAnswer(int sock, char letter, char* word) {
	char buffer[BUFSZ];

	memset(buffer, 0, BUFSZ);

	size_t count = recv(sock, buffer, BUFSZ, 0);

	int typeMessage = buffer[0];

	if (count < 3) {
		if (typeMessage == 4) {
			for (int i=0; i<strlen(word); i++) {
				if (word[i] == '_')
					word[i] = letter;
			}
		} else {
			printf("Palavra não possui a letra %c!", letter);
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

void printWord(char* word, int wordSize) {
	for (int i=0; i<wordSize; i++) {
		printf("%c ", word[i]);
	}
	printf("\n");
}

void initializeWord(char* word, int wordSize) {
	for(int i=0; i<wordSize; i++) {
		word[i] = '_';
	}
	word[wordSize] = '\0';
}