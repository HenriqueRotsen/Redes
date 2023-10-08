#ifndef MY_GAME_H
#define MY_GAME_H

#include "common.h"

struct Board
{
    int board[BOARD_N_ROWS][BOARD_N_COLS];
};


// Tabuleiro no qual o usuario irá interagir
extern struct Board game_board; 

// Resposta do tabuleiro
extern struct Board board_answer;

void load_game(struct Board board);
int start_game();
int reset_game();

struct Board get_game_board();
struct Board get_review_board();

// Retorno de do_action
// Status do Jogo
// Win 1
// Lose 2
// 
// Menssagem de Erro
// revelar célula fora do range do tabuleiro error:                 invalid cell                                        10
// comando inexistente error:                                       command not found                                   20
// revela uma célula já revelada error:                             cell already revealed                               30
// flag em uma célula já marcada error:                             cell already has a flag                             40
// flag em uma célula revelada error:                               cannot insert flag in revealed cell                 50
int do_action(struct game_action act);

// revela uma celula
int reveal_move(int coordinates[2]);
// // Coloca uma flag
int set_flag(int coordinates[2]);
// Remove uma flag
int remove_flag(int coordinates[2]);

// Checa se o jogador ganhou o jogo
int check_win();

#endif