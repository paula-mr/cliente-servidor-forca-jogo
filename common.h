#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

#define BUFSZ 1024

int parseAddress(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void printAddress(const struct sockaddr *addr);

int initializeSocketAddress(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);
