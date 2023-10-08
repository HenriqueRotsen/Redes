#ifndef MY_CLIENT_H
#define MY_CLIENT_H

// Referencia
// https://www.ibm.com/docs/en/i/7.5?topic=clients-example-ipv4-ipv6-client

#include "common.h"

int DEBUG = 0;

// Input Variable
char input_addr[100]; // server address
char SERVER_PORT[10]; // Porta Usada pelo Servidor

// Client Varibles
int len, rtnVal;
int serverfd; // socket file descriptor
int clientfd;
char send_buf[80];
char recv_buf[80];
struct sockaddr_in6 server_addr;
struct sockaddr_in6 client_addr;
struct addrinfo hints, *res=NULL;
socklen_t client_length; /* length of client structure received on accept */

void arg_parsing(int argc, char *argv[]);
void interrupt_handler (int signum);

struct action process_game_action(struct action act);
int get_command_code(char *command);
struct action get_client_input_and_validate(int currBoard[BOARD_N_ROWS][BOARD_N_COLS]);
struct action parse_action(char *input);
int isActionValid(struct action act, int currBoard[BOARD_N_ROWS][BOARD_N_COLS]);


#endif