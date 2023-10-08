#include "game.h"
// Tabuleiro no qual o usuario irá interagir
struct Board game_board; 
// Resposta do tabuleiro
struct Board board_answer;

struct Board get_game_board() {
    struct Board board_copy = game_board;
    return board_copy;
}

struct Board get_review_board() {
    struct Board board_copy = board_answer;
    return board_copy;
}

void load_game(struct Board board) {
    copy_matrix(board.board, board_answer.board);
}

int start_game() {
    for (int i = 0; i < BOARD_N_ROWS; i++) {
        for (int j = 0; j < BOARD_N_COLS; j++) {
            game_board.board[i][j] = -2;
        }
    }
    return 0;
}

int reset_game() {
    for (int i = 0; i < BOARD_N_ROWS; i++) {
        for (int j = 0; j < BOARD_N_COLS; j++) {
            game_board.board[i][j] = -2;
        }
    }
    return 0;
}

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
int do_action(struct game_action act) {
    int status;
    switch (act.type)
    {
    // reveal
    case 1:
        status = reveal_move(act.coordinates);
        break;
    // Coloca uma flag
    case 2:
        status = set_flag(act.coordinates);
        break;
    // Remove uma flag
    case 4:
        status = remove_flag(act.coordinates);
        break;
    // Reseta o Jogo
    case 5:
        status = reset_game();
        break;
    
    default:
        status = 20;
        break;
    }

    return status;
}

// Checa se o jogador ganhou o jogo
// Se o jogador ganhou              Win             1
// Ainda não ganhou         Haven't won yet         0
int check_win() {
    int numNonBombCell = 0;
    for (int i = 0; i < BOARD_N_ROWS; i++) {
        for (int j = 0; j < BOARD_N_COLS; j++) {
            if (board_answer.board[i][j] >= 0) {
                numNonBombCell++;
            } 
        }
    }

    int numRevealedCell = 0;
    for (int i = 0; i < BOARD_N_ROWS; i++) {
        for (int j = 0; j < BOARD_N_COLS; j++) {
            if (game_board.board[i][j] >= 0) numRevealedCell++;
        }
    }

    if (numNonBombCell == numRevealedCell) return 1;
    return 0;
}

// Revela uma celula
// Status do Jogo
// Valid move  0
// Win         10    
// Lose        20
// Erros que podem aparecer
// revelar célula fora do range do tabuleiro error:                 invalid cell                                        -110
// revela uma célula já revelada error:                             cell already revealed                               -130
int reveal_move(int coordinates[2]) {
    // // revelar célula fora do range do tabuleiro error
    // if (coordinates[0] > BOARD_N_ROWS || coordinates[1] > BOARD_N_COLS) return 10;

    // // cell already revealed
    // if (game_board.board[coordinates[0]][coordinates[1]] >= 0 ) return 30;

    // cell has a bomb
    // Game-Over -> code: 8
    if (board_answer.board[coordinates[0]][coordinates[1]] == -1 ) return 8;

    game_board.board[coordinates[0]][coordinates[1]] = board_answer.board[coordinates[0]][coordinates[1]];

    // Win -> code: 6
    if (check_win()) return 6;

    // status that everything is right
    return 3;
}

// // Coloca uma flag
// Erros que podem aparecer
// revelar célula fora do range do tabuleiro error:                 invalid cell                                        10
// flag em uma célula já marcada error:                             cell already has a flag                             40
// flag em uma célula revelada error:                               cannot insert flag in revealed cell                 50
int set_flag(int coordinates[2]) {
    // revelar célula fora do range do tabuleiro error
    // if (coordinates[0] > BOARD_N_ROWS || coordinates[1] > BOARD_N_COLS) return 1;

    // // cell already has a flag
    // if (game_board.board[coordinates[0]][coordinates[1]] == -3 ) return 4;

    // // cannot insert flag in revealed cell
    // if (game_board.board[coordinates[0]][coordinates[1]] >= 0 ) return 5;

    game_board.board[coordinates[0]][coordinates[1]] = -3;

    return 3;
}


// Remove uma flag
// revelar célula fora do range do tabuleiro error:                 invalid cell                                        10
int remove_flag(int coordinates[2]) {
    // revelar célula fora do range do tabuleiro error
    // if (coordinates[0] > BOARD_N_ROWS || coordinates[1] > BOARD_N_COLS) return 1;

    game_board.board[coordinates[0]][coordinates[1]] = -2;
    return 3;
}