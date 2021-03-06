#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define VERSION "v4"
#define WORD "COMUNICACAO"

int createSocket(char **argv);
int connectToClientSocket(int sock);
void sendAcknowledgmentMessage(int clientSocket);
void sendFinalMessage(int clientSocket);
char receiveLetter(int clientSocket);
int sendGuessAnswer(int clientSocket, char letter, char *filledWord);
void initializeWord(char *word);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Argumentos passados incorretos. Necessário especificar a porta.");
        exit(EXIT_FAILURE);
    }

    int sock = createSocket(argv);

    while (1)
    {
        printf("Esperando cliente se conectar.\n");
        int clientSocket = connectToClientSocket(sock);

        sendAcknowledgmentMessage(clientSocket);

        printf("Esperando pelo palpite do usuário.\n");

        char filledWord[BUFSZ];
        initializeWord(filledWord);

        int isWordComplete = 0;
        while (!isWordComplete)
        {
            char letter = receiveLetter(clientSocket);
            isWordComplete = sendGuessAnswer(clientSocket, letter, filledWord);
        }

        close(clientSocket);
        printf("Conexão fechada com o cliente.\n\n");
    }

    exit(EXIT_SUCCESS);
}

int createSocket(char **argv)
{
    struct sockaddr_storage storage;
    if (initializeSocketAddress(VERSION, argv[1], &storage) != 0)
    {
        printf("Argumentos passados incorretos. Necessário especificar tipo de endereço e porta.");
        exit(EXIT_FAILURE);
    }

    int sock = socket(storage.ss_family, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Erro ao inicializar socket.");
        exit(EXIT_FAILURE);
    }

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0)
    {
        printf("Erro ao configurar socket.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr *address = (struct sockaddr *)(&storage);
    if (bind(sock, address, sizeof(storage)) != 0)
    {
        printf("Erro ao realizar bind do servidor.");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1) != 0)
    {
        printf("Erro ao escutar requisições.");
        exit(EXIT_FAILURE);
    }

    printAddress(address);

    return sock;
}

int connectToClientSocket(int sock)
{
    struct sockaddr_storage clientStorage;
    struct sockaddr *clientAddress = (struct sockaddr *)(&clientStorage);
    socklen_t clientAddressLength = sizeof(clientStorage);

    int clientSocket = accept(sock, clientAddress, &clientAddressLength);

    if (clientSocket == -1)
    {
        printf("Erro ao conectar com o cliente.");
        exit(EXIT_FAILURE);
    }

    printAddress(clientAddress);

    return clientSocket;
}

void sendAcknowledgmentMessage(int clientSocket)
{
    char buffer[2];
    buffer[0] = ACKNOWLEDGMENT_MESSAGE;
    buffer[1] = strlen(WORD);

    size_t count = send(clientSocket, buffer, strlen(buffer), 0);
    if (count != strlen(buffer))
    {
        printf("Erro ao mandar mensagem de confirmação.");
        exit(EXIT_FAILURE);
    }
}

void sendFinalMessage(int clientSocket)
{
    char buffer[1];
    memset(buffer, 0, 1);
    buffer[0] = END_MESSAGE;

    printf("Palavra adivinhada!\n");

    size_t count = send(clientSocket, buffer, 1, 0);
    if (count != 1)
    {
        printf("Erro ao mandar mensagem de finalização.");
        exit(EXIT_FAILURE);
    }
}

int sendGuessAnswer(int clientSocket, char letter, char *filledWord)
{
    char buffer[BUFSZ];
    memset(buffer, 0, BUFSZ);

    int countOccurrences = 0;
    for (int i = 0; i < strlen(WORD); i++)
    {
        if (WORD[i] == letter)
        {
            filledWord[i] = letter;
            buffer[countOccurrences + 2] = i;
            countOccurrences++;
        }
    }

    int result = strcmp(WORD, filledWord);
    if (result == 0)
    {
        sendFinalMessage(clientSocket);
        return 1;
    }

    buffer[0] = ANSWER_MESSAGE;
    buffer[1] = countOccurrences;

    size_t count = send(clientSocket, buffer, countOccurrences + 2, 0);
    if (count != countOccurrences + 2)
    {
        printf("Erro ao enviar ocorrências da letra.");
        exit(EXIT_FAILURE);
    }

    return 0;
}

char receiveLetter(int clientSocket)
{
    char buffer[2];
    memset(buffer, 0, 2);

    size_t count = recv(clientSocket, buffer, 2, 0);

    int typeMessage = buffer[0];
    if (count != 2 || typeMessage != GUESS_MESSAGE)
    {
        printf("Erro ao receber letra a ser testada");
        exit(EXIT_FAILURE);
    }

    char letter = buffer[1];

    return letter;
}

void initializeWord(char *word) {
    memset(word, 0, BUFSZ);
    word[strlen(WORD)] = '\0';
}