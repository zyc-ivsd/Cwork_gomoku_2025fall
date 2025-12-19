#include "board.h"
#include "check.h"
//AI下棋所需要的函数
//Part 1:evaluate 
int evaluate_position(GameState *gamestate,int row,int col,CellState Player);
int evaluate_threat(GameState *gamestate,int row,int col,CellState Player);



//Part 2:candidate 

int generate_position(GameState *gamestate,Position candidates[],CellState Player);



//Part 3:search




//Part 4:decide
Position decide(GameState *gamestate);

