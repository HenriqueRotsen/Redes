#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

// Definiçoes de células
#define BOMBA -1
#define OCULTA -2
#define CEL_FLAG -3
#define SEM_BOMA_VIZ 0

// Definiçoes de comandos
#define START 0
#define REVEAL 1
#define FLAG 2
#define STATE 3
#define REMOVE_FLAG 4
#define RESET 5
#define WIN 6
#define EXIT 7
#define GAME_OVER 8

struct action
{
    int type;
    int coordinates[2];
    int board[4][4];
};

void logexit(const char *msg);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);
