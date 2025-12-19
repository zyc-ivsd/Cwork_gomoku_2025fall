#include "board.h"

// Function to initialize the game state
// 初始化游戏状态的函数
void initializeGame(GameState *gameState) {
    gameState->currentPlayer = PLAYER_BLACK;
    gameState->lastMove.col = -1;    
    gameState->lastMove.row = -1;
//刚开始执行程序的的时候，有莫名其妙的输出：Your last move was A2
//经询问AI，得知是因为没有初始化lastmove，分配的是垃圾值，所以这里给个无效初始化

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            gameState->board[i][j] = EMPTY;
        }
    }
}


// Function to print the board
// Use numbers to represent columns, characters to represent rows
// 打印棋盘的函数。使用数字表示列，字符表示行
void printBoard(GameState *gameState) {
    // Display the last move
    if(gameState->lastMove.col < 0 || gameState->lastMove.row < 0){
        printf("No move now\n");
    }
    else printf("Your last move was: %c%d\n", (char)gameState->lastMove.col + 'A', gameState->lastMove.row + 1);

    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", BOARD_SIZE - i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (gameState->board[BOARD_SIZE-1-i][j] == BLACK) { // is black stone
                if (j == BOARD_SIZE - 1) {
                    if (i == BOARD_SIZE-1-gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("▲\n");
                    else
                        printf("●\n");
                } else {
                    if (i == BOARD_SIZE-1-gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("▲─");
                    else
                        printf("●─");
                }
            }
            else if (gameState->board[BOARD_SIZE-1-i][j] == WHITE) { // is white stone
                if (j == BOARD_SIZE - 1) {
                    if (i == BOARD_SIZE -1-gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("△\n");
                    else
                        printf("◎\n");
                } else {
                    if (i == BOARD_SIZE -1- gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("△─");
                    else
                        printf("◎─");
                }
            } else if (i == 0) { // top row empty
                if (j == 0)                   { printf("┌─");  }
                else if (j == BOARD_SIZE - 1) { printf("┐\n"); }
                else                          { printf("┬─");  }
            } else if (i == BOARD_SIZE - 1) { // bottom row empty
                if (j == 0)                   { printf("└─");  }
                else if (j == BOARD_SIZE - 1) { printf("┘\n"); }
                else                          { printf("┴─");  }
            } else { // middle empty
                if (j == 0)                   { printf("├─");  }
                else if (j == BOARD_SIZE - 1) { printf("┤\n"); }
                else                          { printf("┼─");  }
            }
        }
    }

    printf("  ");
    for (int i = 0; i < BOARD_SIZE; i++)
        printf(" %c", 'A' + i);
    printf("\n");
}
