#ifndef AI_H
#define AI_H

#include "board.h"
#include "check.h"
#include <time.h>

//AI下棋所需要的函数
//Part 1:evaluate 
int evaluate_position(GameState *gamestate,int row,int col,Player player);

int getpattenscore(GameState *state,int row,int col,Player player);

//Part 2:candidate 
int generate_position(GameState *gamestate,Position candidates[],Player aiplayer);

//Part 3:search
int alphaBetaSearch(GameState *state, int depth, int alpha, int beta, Player player, int isMaximizing, clock_t start_time);

//Part 4:decide
Position bestMove(GameState *state,Player aiplayer);

// 时间测量函数
double get_elapsed_time(clock_t start, clock_t end);

#endif