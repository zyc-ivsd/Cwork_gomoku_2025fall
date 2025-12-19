#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//在这里实现棋盘的定义，写出所需的结构体，棋盘初始化函数，打印棋盘函数

// define board size
// 定义棋盘大小
#define BOARD_SIZE 15

// enum of board cells
// 棋盘格子的状态枚举
typedef enum {
    EMPTY,
    BLACK,
    WHITE
} CellState;

// enum of players
// 玩家枚举
typedef enum {
    PLAYER_BLACK,
    PLAYER_WHITE
} Player;

// enum of game modes
// 游戏模式枚举
typedef enum {
    MODE_PVP, // Player vs Player
    MODE_PVE  // Player vs AI
} GameMode;

// structure of board position
// 棋盘位置结构体
typedef struct {
    int row;
    int col; // column
} Position;

// structure of the game state can be defined here
// 游戏状态结构体
typedef struct {
    CellState board[BOARD_SIZE][BOARD_SIZE];
    Player currentPlayer; // whose turn it is
    Position lastMove; // where the last move made 
}GameState;

//定义检查边界结构体，专门用于检查活三活四等棋型
typedef struct {
    int aligned;
    int one_side_open;
    int other_side_open;
}Check_side;



void initializeGame(GameState *gameState);
void printBoard(GameState *gameState);