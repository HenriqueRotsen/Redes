#ifndef MY_SERVER_H
#define MY_SERVER_H

#include "common.h"
#include "game.h"

int DEBUG = 0;

// Input Variable
char filename[1000];
int IP_version; // 0 -> IPv4, 1 -> IPv6 
int SERVER_PORT; // Porta Usada pelo Servidor


// Server Varibles
int len, rc;
int serverfd; // socket file descriptor
int clientfd;
char send_buf[80];
char recv_buf[80];
struct sockaddr_in6 server_addr;
struct sockaddr_in6 client_addr;
socklen_t client_length; /* length of client structure received on accept */

void arg_parsing(int argc, char *argv[]);
void load_input(char *filename);

struct action receive_client_message(struct action act);
int close_conection();
int send_to_client_board(struct action act);


// Game function
struct action server_reveal_cell(int coor[2]);
struct action server_flag_cell(int coord[2]);
struct action server_remove_flag(int coord[2]);
struct action server_reset_game();
struct action server_game_over();
struct action server_start_game();

// Signal Handler
void interrupt_handler (int signum);

// Apagar
struct action simula_client(struct action act);
int get_command_code(char *command);

#endif
