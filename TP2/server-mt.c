#include "common.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

struct client_data
{
    int csock;
    struct sockaddr_storage storage;
};

void *client_thread(void *data, int clientId)
{
    struct client_data *cdata = (struct client_data *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&cdata->storage);
    struct BlogOperation operation;
    while (1)
    {
        memset(&operation, 0, sizeof(struct BlogOperation));
        size_t count = recv(cdata->csock, &operation, sizeof(struct BlogOperation), 0);

        if (operation.operation_type == NEW_CONECTION)
        {
            operation.client_id = clientId;
            operation.operation_type = NEW_CONECTION;
            operation.server_response = 1;
            strcpy(operation.topic, "");
            strcpy(operation.content, "");
        }
        else if (operation.operation_type == TOPIC_SUBSCRIPTION)
        {
            // Ver ser esse topico ja nao existe
        }

        count = send(cdata->csock, &operation, sizeof(struct BlogOperation), 0);
        if (count != sizeof(struct BlogOperation))
        {
            logexit("send");
        }
    }
    close(cdata->csock);
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{

    struct sockaddr_storage storage;
    server_sockaddr_init(argv[1], argv[2], &storage);

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
    {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage)))
    {
        logexit("bind");
    }

    if (0 != listen(s, 10))
    {
        logexit("listen");
    }

    int numClient = 0;
    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        numClient++;

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1)
        {
            logexit("accept");
        }

        // Cliente conectou
        printf("client %d connected\n", numClient);

        struct client_data *cdata = malloc(sizeof(*cdata));
        if (!cdata)
        {
            logexit("malloc");
        }
        cdata->csock = csock;
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }

    exit(EXIT_SUCCESS);
}
