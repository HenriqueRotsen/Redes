#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char convert(int n)
{
	char c;
	if (n == BOMBA)
	{
		c = '*';
	}
	else if (n == OCULTA)
	{
		c = '-';
	}
	else if (n == CEL_FLAG)
	{
		c = '>';
	}
	else if (n == SEM_BOMA_VIZ)
	{
		c = 0;
	}
	else
	{
		c = (char)n;
	}
	return c;
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
	struct action msg;
	char aux[100];

	memset(&msg, 0, sizeof(struct action));
	memset(&aux, 0, 100);

	scanf("%s", aux);
	if (strcmp(aux, "start") == 0)
	{
		msg.type = START;
	}
	else if (strcmp(aux, "exit") == 0)
	{
		msg.type = EXIT;
	}

	size_t count = send(s, &msg, sizeof(struct action), 0);
	if (count != sizeof(struct action))
	{
		logexit("send");
	}

	if (msg.type == EXIT)
	{
		close(s);
		return 0;
	}

	memset(&msg, 0, sizeof(struct action));

	// Deu start

	while (1)
	{
		// Evitar erros no aux
		memset(&aux, 0, 100);

		count = recv(s, &msg, sizeof(struct action), 0);
		if (count == 0) // Teste para ver se a conexão acabou
		{
			break;
		}

		// Vê se ganhou ou perdeu
		if (msg.type == WIN)
		{
			printf("YOU WIN!\n");
		}
		if (msg.type == GAME_OVER)
		{
			printf("GAME OVER!\n");
		}

		// Mostre o jogo atualizado
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				printf("%c\t\t", convert(msg.board[i][j]));
			}
			printf("\n");
		}

		// Vê as próximas ações do jogador
		char tipo[20];
		fgets(aux, sizeof(aux), stdin);
		sscanf(aux, "%6s", tipo);

		printf("%6s", tipo);
		if (strcmp(aux, "reveal") == 0)
		{
			msg.type = REVEAL;
			int i, j;

			scanf("%d,%d", &i, &j);
			if (0 <= i && i <= 3 && 0 <= j && j <= 3)
			{
				if (msg.board[i][j] != '-')
				{
					msg.coordinates[0] = i;
					msg.coordinates[1] = j;
					size_t count = send(s, &msg, sizeof(struct action), 0);
					if (count != sizeof(struct action))
					{
						logexit("send");
					}
				}
				else
				{
					printf("error: cell already revealed\n");
				}
			}
			else
			{
				printf("error: invalid cell\n");
			}
		}
		else if (strcmp(aux, "flag") == 0)
		{
			msg.type = FLAG;
			int i, j;

			scanf("%d,%d", &i, &j);
			if (0 <= i && i <= 3 && 0 <= j && j <= 3)
			{
				if (msg.board[i][j] != '-')
				{
					if (msg.board[i][j] != '>')
					{
						msg.coordinates[0] = i;
						msg.coordinates[1] = j;
						size_t count = send(s, &msg, sizeof(struct action), 0);
						if (count != sizeof(struct action))
						{
							logexit("send");
						}
					}
					else
					{
						printf("error: cell already has a flag\n");
					}
				}
				else
				{
					printf("error: cannot insert flag in revealed cell\n");
				}
			}
			else
			{
				printf("error: invalid cell\n");
			}
		}
		else if (strcmp(aux, "remove_flag") == 0)
		{
			msg.type = REMOVE_FLAG;
			int i, j;

			scanf("%d,%d", &i, &j);
			if (0 <= i && i <= 3 && 0 <= j && j <= 3)
			{
				msg.coordinates[0] = i;
				msg.coordinates[1] = j;
				size_t count = send(s, &msg, sizeof(struct action), 0);
				if (count != sizeof(struct action))
				{
					logexit("send");
				}
			}
			else
			{
				printf("error: invalid cell\n");
			}
		}
		else if (strcmp(aux, "reset") == 0)
		{
			msg.type = RESET;
			size_t count = send(s, &msg, sizeof(struct action), 0);
			if (count != sizeof(struct action))
			{
				logexit("send");
			}
		}
		else if (strcmp(aux, "exit") == 0)
		{
			msg.type = EXIT;
			size_t count = send(s, &msg, sizeof(struct action), 0);
			if (count != sizeof(struct action))
			{
				logexit("send");
			}
		}
		else
		{
			printf("error: command not found\n");
		}
	}
	close(s);

	exit(EXIT_SUCCESS);
}