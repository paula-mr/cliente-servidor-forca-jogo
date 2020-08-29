#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

#define BUFSZ 1024
#define ACKNOWLEDGMENT_MESSAGE 1
#define GUESS_MESSAGE 2
#define ANSWER_MESSAGE 3
#define END_MESSAGE 4

int parseAddress(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void printAddress(const struct sockaddr *addr);

int initializeSocketAddress(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);
