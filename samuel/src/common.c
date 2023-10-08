#include "common.h"

void copy_matrix(int origin[BOARD_N_ROWS][BOARD_N_COLS], int dest[BOARD_N_ROWS][BOARD_N_COLS]) {
    for (int i = 0; i < BOARD_N_ROWS; i++) {
        for (int j = 0; j < BOARD_N_COLS; j++) {
            dest[i][j] = origin[i][j];
        }
    }
}

void print_matrix(int board[BOARD_N_ROWS][BOARD_N_COLS]) {
    for (int i = 0; i < BOARD_N_ROWS; i++) {
        for (int j = 0; j < BOARD_N_COLS; j++) {
            if (j == BOARD_N_COLS - 1) printf("%d\n", board[i][j]);
            else printf("%d\t\t", board[i][j]);
        }
    }
}

void print_board(int board[BOARD_N_ROWS][BOARD_N_COLS]) {
    for (int i = 0; i < BOARD_N_ROWS; i++) {
        for (int j = 0; j < BOARD_N_COLS; j++) {
            switch (board[i][j])
            {
            case -3:
                printf(">");
                break;
            case -2:
                printf("-");
                break;
            case -1:
                printf("*");
                break;
            default:
                printf("%d", board[i][j]);
                break;
            }

            // Se é a ultima coluna
            if (j == BOARD_N_COLS - 1) printf("\n");
            else printf("\t\t");
        }
    }
}

void print_action(struct action act) {
    printf("action: %d\n", act.type);
    printf("coord ->  x: %d, y: %d\n", act.coordinates[0], act.coordinates[1]);
    print_matrix(act.board);
}