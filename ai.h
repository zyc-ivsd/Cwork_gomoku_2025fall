#include "board.h"
#include "check.h"
//AI下棋所需要的函数
//Part 1:evaluate 
int evaluate_position(GameState *gamestate,int row,int col,Player player);

int getpattenscore(GameState *state,int row,int col,Player player);




//Part 2:candidate 
int generate_position(GameState *gamestate,Position candidates[],Player aiplayer);



//Part 3:search




//Part 4:decide
Position bestMove(GameState *state,Player aiplayer);


