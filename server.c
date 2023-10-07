#include "common.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    assert(argc == 5);

    FILE *arquivo;

    // Abre o arquivo em modo de leitura ("r")
    arquivo = fopen(argv[4], "r");

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    int gabarito[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            fscanf(arquivo, "%d,", &gabarito[i][j]);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%c\t\t", convert(gabarito[i][j]));
        }
        printf("\n");
    }

    struct sockaddr_storage storage;
    server_sockaddr_init(argv[1], argv[2], &storage);

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logexit("socket");
    }

    // Reinicialização de porta
    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
    {
        logexit("setsockopt");
    }

    // Interface para rodar IPv4 e IPv6
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage)))
    {
        logexit("bind");
    }

    if (0 != listen(s, 10))
    {
        logexit("listen");
    }

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    while (1)
    {
        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1)
        {
            logexit("accept");
        }

        // Cliente conectou
        printf("client connected\n");

        // Mensagem
        struct action msg;
        struct action userboard;

        // Contador para verificar se o jogo chegou o fim
        int cont = 16;

        while (1)
        {
            memset(&msg, 0, sizeof(struct action));
            memset(&userboard, 0, sizeof(struct action));
            size_t count = recv(csock, &msg, sizeof(struct action), 0);

            if (msg.type == START)
            {
                userboard.type = STATE;
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        userboard.board[i][j] = OCULTA;
                    }
                }
            }
            else if (msg.type == REVEAL)
            {
                int i = msg.coordinates[0], j = msg.coordinates[1];
                userboard.type = STATE;
                if (gabarito[i][j] == BOMBA)
                {
                    userboard.type = GAME_OVER;
                    for (int k = 0; k < 4; k++)
                    {
                        for (int l = 0; l < 4; l++)
                        {
                            userboard[k][l] = gabarito[k][l];
                        }
                    }
                }
                else
                {
                    cont--;
                    if (cont <= 3)
                    {
                        userboard.type = WIN;
                        for (int k = 0; k < 4; k++)
                        {
                            for (int l = 0; l < 4; l++)
                            {
                                userboard.board[k][l] = gabarito[k][l];
                            }
                        }
                    }
                    else
                    {
                        userboard.board[i][j] = gabarito[i][j];
                    }
                }
            }
            else if (msg.type == FLAG)
            {
                int i = msg.coordinates[0], j = msg.coordinates[1];
                userboard.type = STATE;
                userboard.board[i][j] = CEL_FLAG;
            }
            else if (msg.type == REMOVE_FLAG)
            {
                int i = msg.coordinates[0], j = msg.coordinates[1];
                userboard.type = STATE;
                userboard.board[i][j] = OCULTA;
            }
            else if (msg.type == RESET)
            {
                printf("starting new game\n");
                userboard.type = STATE;
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        userboard.board[i][j] = OCULTA;
                    }
                }
            }
            else if (msg.type == EXIT)
            {
                printf("client disconnected\n");
                close(csock);
                break;
            }

            count = send(csock, &userboard, sizeof(struct action), 0);
            if (count != sizeof(struct action))
            {
                logexit("send");
            }
        }
        close(csock);
    }

    fclose(arquivo);
    exit(EXIT_SUCCESS);
}
