#include "common.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

struct Topics
{
    char topic[50];
    int clients_id[10];
    struct Topics *next;
};

struct Conected_Client
{
    int csock;
    int client_id;
};

struct Conected_Client clients[10];

struct Topics *list_head = NULL;
struct Topics *list_tail = NULL;

struct client_data
{
    int csock;
    struct sockaddr_storage storage;
};

int get_csock(int client_id)
{
    for (int i = 0; i < 10; i++)
    {
        if (client_id == clients[i].client_id)
        {
            return clients[i].csock;
        }
    }
    return 0;
}

int define_id(int csock)
{
    for (int i = 0; i < 10; i++)
    {
        if (clients[i].client_id == -1)
        {
            clients[i].client_id = i + 1;
            clients[i].csock = csock;
            return i + 1;
        }
    }
    return -1;
}

struct Topics *topic_search(char topic_name[])
{
    struct Topics *ptr = list_head;
    while (ptr != NULL)
    {
        if (strcmp(ptr->topic, topic_name) == 0)
        {
            return ptr;
        }
        ptr = ptr->next;
    }

    struct Topics *new_topic = (struct Topics *)malloc(sizeof(struct Topics));
    memset(new_topic->clients_id, -1, 10 * sizeof(int));
    strcpy(new_topic->topic, topic_name);
    new_topic->next = NULL;

    // Caso seja o primeiro elemento
    if (list_head == NULL)
    {
        list_head = new_topic;
        list_tail = new_topic;
    }
    else
    {
        list_tail->next = new_topic;
        list_tail = new_topic;
    }

    return new_topic;
}

void debugLog(struct BlogOperation op)
{
    printf("\n##############\n");
    printf("client_id: %d\n", op.client_id);
    printf("operation_type: %d\n", op.operation_type);
    printf("server_response: %d\n", op.server_response);
    printf("topic: %s\n", op.topic);
    printf("content: %s\n", op.content);
    printf("\n##############\n\n");
}

void *client_thread(void *data)
{
    struct client_data *cdata = (struct client_data *)data;
    struct BlogOperation operation;
    while (1)
    {
        memset(&operation, 0, sizeof(struct BlogOperation));
        size_t count = recv(cdata->csock, &operation, sizeof(struct BlogOperation), 0);

        //debugLog(operation);

        if (operation.operation_type == NEW_CONECTION)
        {
            operation.client_id = define_id(cdata->csock);
            operation.operation_type = NEW_CONECTION;
            operation.server_response = 1;
            strcpy(operation.topic, "");
            strcpy(operation.content, "");

            printf("client %d connected\n", operation.client_id);

            count = send(cdata->csock, &operation, sizeof(struct BlogOperation), 0);
            if (count != sizeof(struct BlogOperation))
            {
                logexit("send");
            }
        }
        else if (operation.operation_type == TOPIC_SUBSCRIPTION)
        {
            strcpy(operation.content, "");
            operation.server_response = 1;
            struct Topics *topic = topic_search(operation.topic);

            // Assumir que o cliente nao esta inscrito
            int sub = 0;

            for (int i = 0; i < 10; i++)
            {
                if (topic->clients_id[i] == operation.client_id)
                {
                    sub = 1;
                    strcpy(operation.content, "error: already subscribed\n");
                    break;
                }
            }
            if (sub == 0)
            {
                for (int i = 0; i < 10; i++)
                {
                    if (topic->clients_id[i] == -1)
                    {
                        topic->clients_id[i] = operation.client_id;
                        printf("client %d subscribed to %s\n", operation.client_id, operation.topic);
                        break;
                    }
                }
            }

            count = send(cdata->csock, &operation, sizeof(struct BlogOperation), 0);
            if (count != sizeof(struct BlogOperation))
            {
                logexit("send");
            }
        }
        else if (operation.operation_type == TOPIC_UNSUBSCRIBE)
        {
            operation.server_response = 1;
            struct Topics *topic = topic_search(operation.topic);
            for (int i = 0; i < 10; i++)
            {
                if (topic->clients_id[i] == operation.client_id)
                {
                    topic->clients_id[i] = -1;
                    break;
                }
            }

            count = send(cdata->csock, &operation, sizeof(struct BlogOperation), 0);
            if (count != sizeof(struct BlogOperation))
            {
                logexit("send");
            }
        }
        else if (operation.operation_type == NEW_POST)
        {
            int num_clients_subs = 0;
            int subscribed_clients[10];
            memset(subscribed_clients, -1, 10 * sizeof(int));

            operation.server_response = 1;
            struct Topics *topic = topic_search(operation.topic);

            // Descobrindo todos os clientes inscritos no topico
            for (int i = 0; i < 10; i++)
            {
                if (topic->clients_id[i] != -1)
                {
                    subscribed_clients[num_clients_subs] = topic->clients_id[i];
                    num_clients_subs++;
                }
            }

            // Assumir que o cliente atual nao esta inscrito
            int sub = 0;

            for (int i = 0; i < 10; i++)
            {
                if (topic->clients_id[i] == operation.client_id)
                {
                    sub = 1;
                    break;
                }
            }
            if (sub == 0)
            {
                for (int i = 0; i < 10; i++)
                {
                    if (topic->clients_id[i] == -1)
                    {
                        topic->clients_id[i] = operation.client_id;
                        break;
                    }
                }
            }

            printf("new post added in %s by %02d\n", operation.topic, operation.client_id);
            printf("%s\n", operation.content);

            // Enviar mensagem para todos os clientes incritos
            int aux_csock;
            for (int i = 0; i < num_clients_subs; i++)
            {
                aux_csock = get_csock(subscribed_clients[i]);
                count = send(aux_csock, &operation, sizeof(struct BlogOperation), 0);
                if (count != sizeof(struct BlogOperation))
                {
                    logexit("send");
                }
            }
        }
        else if (operation.operation_type == LIST_TOPICS)
        {
            operation.server_response = 1;
            struct Topics *ptr = list_head;
            if (ptr == NULL)
            {
                strcpy(operation.content, "no topics available");
            }
            else
            {
                while (ptr != NULL)
                {
                    if (ptr->next == NULL)
                    {
                        strcat(operation.content, ptr->topic);
                    }
                    else
                    {
                        strcat(operation.content, ptr->topic);
                        strcat(operation.content, ";");
                    }
                    ptr = ptr->next;
                }
            }
            count = send(cdata->csock, &operation, sizeof(struct BlogOperation), 0);
            if (count != sizeof(struct BlogOperation))
            {
                logexit("send");
            }
        }
        else if (operation.operation_type == DISCONECT)
        {
            // Desinscrevendo o cliente de todos os topicos
            struct Topics *ptr = list_head;
            while (ptr != NULL)
            {
                for (int i = 0; i < 10; i++)
                {
                    if (ptr->clients_id[i] == operation.client_id)
                    {
                        ptr->clients_id[i] = -1;
                        break;
                    }
                }
                ptr = ptr->next;
            }

            // Liberando uma vaga nos clientes conectados
            for (int i = 0; i < 10; i++)
            {
                if (clients[i].client_id == operation.client_id)
                {
                    clients[i].client_id = -1;
                    clients[i].csock = 0;
                    break;
                }
            }
            
            printf("client %d disconnected\n", operation.client_id);

            close(cdata->csock);
            pthread_exit(EXIT_SUCCESS);
        }
    }
    pthread_exit((void*)EXIT_FAILURE);
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

    // Iniciando os clientes com -1
    memset(clients, -1, 10 * sizeof(struct Conected_Client));

    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1)
        {
            logexit("accept");
        }

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
