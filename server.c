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

    int csock = accept(s, caddr, &caddrlen);
    if (csock == -1)
    {
        logexit("accept");
    }

    // Cliente conectou
    printf("client connected\n");

    // Mensagem
    struct action msg;

    while (1)
    {

        memset(&msg, 0, sizeof(struct action));
        size_t count = recv(csock, &msg, sizeof(struct action), 0);

        if (msg.type == START)
        {
            msg.type = STATE;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    msg.board[i][j] = OCULTA;
                }
            }
        }
        else if (msg.type == REVEAL)
        {
            int i = msg.coordinates[0], j = msg.coordinates[1];
            int cont = 0;
            msg.type = WIN;
            if (gabarito[i][j] == BOMBA)
            {
                msg.type = GAME_OVER;
                for (int k = 0; k < 4; k++)
                {
                    for (int l = 0; l < 4; l++)
                    {
                        msg.board[k][l] = gabarito[k][l];
                    }
                }
            }
            else
            {
                // ver se ganhou
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (msg.board[i][j] == OCULTA)
                        {
                            cont++;
                            
                        }
                    }
                }
                msg.board[i][j] = gabarito[i][j];
            }
        }
        else if (msg.type == FLAG)
        {
            int i = msg.coordinates[0], j = msg.coordinates[1];
            msg.type = STATE;
            msg.board[i][j] = CEL_FLAG;
        }
        else if (msg.type == REMOVE_FLAG)
        {
            int i = msg.coordinates[0], j = msg.coordinates[1];
            msg.type = STATE;
            msg.board[i][j] = OCULTA;
        }
        else if (msg.type == RESET)
        {
            printf("starting new game\n");
            msg.type = STATE;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    msg.board[i][j] = OCULTA;
                }
            }
        }
        else if (msg.type == EXIT)
        {
            printf("client disconnected\n");
            close(csock);
            break;
        }

        count = send(csock, &msg, sizeof(struct action), 0);
        if (count != sizeof(struct action))
        {
            logexit("send");
        }
    }

    fclose(arquivo);
    exit(EXIT_SUCCESS);
}