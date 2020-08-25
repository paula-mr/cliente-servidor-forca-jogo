#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

int addressParse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);
void addressToString(const struct sockaddr *addr, char *str, size_t strsize);
int initializeServer(const char *proto, const char *portstr, struct sockaddr_storage *storage)