#include "board.h"

//实现三个检查函数：检查胜利，检查非法落子，检查禁手

int checkWin(GameState *gameState, int row, int col);

int isValidMove(GameState *gameState, int row, int col);


int ban(GameState *gameState, int row ,int col);