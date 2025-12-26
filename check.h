#include "board.h"

//实现三个检查函数：检查胜利，检查非法落子，检查禁手

int checkWin(GameState *gameState, int row, int col);

int isValidMove(GameState *gameState, int row, int col);

CellState check_vs(GameState *gamestate,int row,int col);

int check_open_three(GameState *gamestate,int row,int col,int dir);

int check_open_four(GameState *gamestate,int row,int col,int dir);

int check_four(GameState *gamestate,int row,int col,int dir);

int check_oneline_four(GameState *gamestate,int row ,int col,int dir);

int check_double_four(GameState *gamestate,int row,int col);

int check_double_three(GameState *gamestate,int row,int col);

int check_overline(GameState *gamestate,int row,int col);

int ban(GameState *gameState, int row ,int col);