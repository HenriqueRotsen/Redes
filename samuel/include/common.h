#ifndef MY_COMMON_H
#define MY_COMMON_H

/*************************************************/
/* Used for the server and client                */
/*************************************************/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h> // struct addrinfo | getaddrinfo()


/*************************************************/
/* Used in the game                              */
/*************************************************/

#define BOARD_N_ROWS 4
#define BOARD_N_COLS 4

struct action {
    int type;
    int coordinates[2];
    int board[BOARD_N_ROWS][BOARD_N_COLS];
};

struct game_action {
    int type;
    int coordinates[2];
};

void copy_matrix(int origin[BOARD_N_ROWS][BOARD_N_COLS], int dest[BOARD_N_ROWS][BOARD_N_COLS]);
void print_matrix(int board[BOARD_N_ROWS][BOARD_N_COLS]);
void print_board(int board[BOARD_N_ROWS][BOARD_N_COLS]);
void print_action(struct action act);

#endif