#include "common.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

void *client_thread(void *data)
{
	int s = *(int *)data;
	// Mensagem
	struct BlogOperation operation;
	while (1)
	{
		recv(s, &operation, sizeof(struct BlogOperation), 0);
		if (operation.operation_type == NEW_POST)
		{
			printf("new post added in %s by %02d\n", operation.topic, operation.client_id);
			printf("%s", operation.content);
		}
		else if (operation.operation_type == LIST_TOPICS)
		{
			printf("%s\n", operation.content);
		}
		else if (operation.operation_type == TOPIC_SUBSCRIPTION)
		{
			printf("%s", operation.content);
		}
	}
}

int main(int argc, char **argv)
{
	struct sockaddr_storage storage;
	addrparse(argv[1], argv[2], &storage);

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1)
	{
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage)))
	{
		logexit("connect");
	}

	// Mensagem
	struct BlogOperation operation;

	// Primeira mensagem que o cliente envia
	operation.client_id = 0;
	operation.operation_type = NEW_CONECTION;
	operation.server_response = 0;
	strcpy(operation.topic, "");
	strcpy(operation.content, "");

	// Envia
	size_t count = send(s, &operation, sizeof(struct BlogOperation), 0);
	if (count != sizeof(struct BlogOperation))
	{
		logexit("send");
	}

	recv(s, &operation, sizeof(struct BlogOperation), 0);
	int id = operation.client_id;

	pthread_t tid;
	pthread_create(&tid, NULL, client_thread, (void *)&s);

	char aux[2048];
	while (1)
	{
		memset(&operation, 0, sizeof(struct BlogOperation));

		// Vê as próximas ações do cliente
		memset(&aux, 0, 2048);
		scanf("%s", aux);

		if (strcmp(aux, "publish") == 0)
		{
			operation.client_id = id;
			operation.operation_type = NEW_POST;
			operation.server_response = 0;

			// Descarta o "in"
			memset(&aux, 0, 2048);
			scanf("%s", aux);

			memset(&aux, 0, 2048);
			scanf("%s", aux);
			strcpy(operation.topic, aux);

			// Descarta o \n
			memset(&aux, 0, 2048);
			fgets(aux, sizeof(aux), stdin);

			memset(&aux, 0, 2048);
			fgets(aux, sizeof(aux), stdin);
			strcpy(operation.content, aux);

			size_t count = send(s, &operation, sizeof(struct BlogOperation), 0);
			if (count != sizeof(struct BlogOperation))
			{
				logexit("send");
			}
		}
		else if (strcmp(aux, "subscribe") == 0)
		{
			operation.client_id = id;
			operation.operation_type = TOPIC_SUBSCRIPTION;
			operation.server_response = 0;

			memset(&aux, 0, 2048);
			scanf("%s", aux);
			strcpy(operation.topic, aux);
			strcpy(operation.content, "");

			size_t count = send(s, &operation, sizeof(struct BlogOperation), 0);
			if (count != sizeof(struct BlogOperation))
			{
				logexit("send");
			}
		}
		else if (strcmp(aux, "unsubscribe") == 0)
		{
			operation.client_id = id;
			operation.operation_type = TOPIC_UNSUBSCRIBE;
			operation.server_response = 0;

			memset(&aux, 0, 2048);
			scanf("%s", aux);
			strcpy(operation.topic, aux);
			strcpy(operation.content, "");

			size_t count = send(s, &operation, sizeof(struct BlogOperation), 0);
			if (count != sizeof(struct BlogOperation))
			{
				logexit("send");
			}
		}
		else if (strcmp(aux, "list") == 0)
		{
			operation.client_id = id;
			operation.operation_type = LIST_TOPICS;
			operation.server_response = 0;
			strcpy(operation.topic, "");
			strcpy(operation.content, "");
			size_t count = send(s, &operation, sizeof(struct BlogOperation), 0);
			if (count != sizeof(struct BlogOperation))
			{
				logexit("send");
			}
		}
		else if (strcmp(aux, "exit") == 0)
		{
			operation.client_id = id;
			operation.operation_type = DISCONECT;
			operation.server_response = 0;
			strcpy(operation.topic, "");
			strcpy(operation.content, "");
			size_t count = send(s, &operation, sizeof(struct BlogOperation), 0);
			if (count != sizeof(struct BlogOperation))
			{
				logexit("send");
			}
			close(s);
			exit(EXIT_SUCCESS);
		}
	}
	exit(EXIT_FAILURE);
}