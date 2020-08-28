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

int createSocket(char **argv) {
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
        printf("Erro ao escutar requisições.");
		exit(EXIT_FAILURE);
    }

    char addrstr[BUFSZ];
	addrtostr(address, addrstr, BUFSZ);
	printf("Bound em %s\n", addrstr);

    return sock;
}

int connectToClientSocket(int sock) {
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

    return clientSocket;
}

void sendAcknowledgmentMessage(int clientSocket) {
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
}

void sendFinalMessage(int clientSocket) {
    char buffer[BUFSZ];
    memset(buffer, 0, BUFSZ);
    buffer[0] = 4;

    size_t count = send(clientSocket, buffer, 1, 0);

    if (count != 1) {
        logexit("final");
    }
}

int sendGuessAnswer(char letter, int sock, char* filledWord) {
    int countOccurrences = 0;
    char buffer[BUFSZ];
    memset(buffer, 0, BUFSZ);

    buffer[0] = 3;

    for (int i=0; i<strlen(WORD); i++) {
        if (WORD[i] == letter) {
            filledWord[i] = letter;
            buffer[countOccurrences+2] = i;
            countOccurrences++;
        }
    }

    buffer[1] = countOccurrences;
    buffer[countOccurrences+2] = '\0';

    printf("Occurrences: %d\n", countOccurrences);
    printf("%s\n", buffer);

    int result = strcmp(WORD, filledWord);

    if (result == 0) {
        sendFinalMessage(clientSocket);
        return 1;
    } 
    size_t count = send(sock, buffer, countOccurrences+3, 0);
    if (count != countOccurrences+3) {
        logexit("send");
    }

    return 0;
}

int receiveLetter(int clientSocket, char* filledWord) {
    char buffer[BUFSZ];

    size_t count = 0;
    memset(buffer, 0, BUFSZ);
    
    count = recv(clientSocket, buffer, BUFSZ, 0);

    if (count > 0 && count < 100) {
        printf("%s\n", buffer);
        printf("received %u bytes\n", count);

        int typeMessage = buffer[0] - '0';
        char letter = buffer[1];

        printf("Received message type %d with letter %c\n", typeMessage, letter);
    
        return sendGuessAnswer(letter, clientSocket, filledWord);
    }
}

int main(int argc, char **argv) {
	if (argc < 3) {
        printf("Argumentos passados incorretos. Necessário especificar tipo e porta.");
        exit(EXIT_FAILURE);
    }

    int sock = createSocket(argv);

    while(1) {
        int clientSocket = connectToClientSocket(sock);
        
        sendAcknowledgmentMessage(clientSocket);

        int isWordComplete = 0;
        printf("Waiting for user guess\n");

        char filledWord[BUFSZ];
        memset(filledWord, 0, BUFSZ);
        filledWord[strlen(WORD)] = '\0';

        while (!isWordComplete) {
            isWordComplete = receiveLetter(clientSocket, filledWord);
        }

        close(clientSocket);
    }

	exit(EXIT_SUCCESS);
}
