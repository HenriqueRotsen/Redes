#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

// Operation types
#define NEW_CONECTION 1
#define NEW_POST 2
#define LIST_TOPICS 3
#define TOPIC_SUBSCRIPTION 4
#define DISCONECT 5
#define TOPIC_UNSUBSCRIBE 6

struct BlogOperation
{
    int client_id;
    int operation_type;
    int server_response;
    char topic[50];
    char content[2048];
};

void logexit(const char *msg);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);
