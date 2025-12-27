#include "board.h"
#include "check.h"

int evaluate_position(GameState *gamestate,int row,int col,Player player);
int getpattenscore(GameState *state,int row,int col,Player player);
int generate_position(GameState *gamestate,Position candidates[],Player aiplayer);
Position bestMove(GameState *state,Player aiplayer);

Position bestMove(GameState *state,Player aiplayer){
    Position move;
    int bestscore = -1000000;
    int getscore ;
    Position candidates[MaxCandidate];
    int candidate_num = generate_position(state,candidates,aiplayer);
    CellState ai = (aiplayer == PLAYER_BLACK)? BLACK:WHITE;
    CellState vs = (aiplayer == PLAYER_BLACK)? WHITE:BLACK;
    if(candidate_num <= 0) {
        move.row = BOARD_SIZE / 2;
        move.col = BOARD_SIZE / 2;
        return move;
    }

    for(int i = 0; i < candidate_num;i ++){
        int row = candidates[i].row;
        int col = candidates[i].col;
        state->board[row][col] = ai;
        getscore = evaluate_position(state,row,col,aiplayer);
        state->board[row][col] = EMPTY;
        if(getscore > bestscore){
            bestscore  = getscore;
            move.row = row; move.col = col;
        }
    }
    return move;

}

int evaluate_position(GameState *gamestate,int row,int col,Player player){
   CellState aicolor = (player == PLAYER_BLACK)? BLACK:WHITE;
   CellState humancolor = (player == PLAYER_BLACK)? WHITE:BLACK;
   int attackscore = getpattenscore(gamestate,row,col,player);
   gamestate->board[row][col] = humancolor;
   int defensescore = getpattenscore(gamestate,row,col,player);
   gamestate->board[row][col] = aicolor;
   return attackscore + 1.2 * defensescore; 
}
int getpattenscore(GameState *state,int row,int col, Player player){
    CellState color = (player == PLAYER_BLACK)? BLACK:WHITE;
    int score = 0;
    if(checkWin(state,row,col)) return 100000;  //五连加100000

    if(color == BLACK){
        if(ban(state,row,col)) return -100000;   //黑棋下出禁手，负分
    }
    if(check_double_four(state,row,col)) return 100000; //白棋双冲四必胜
    int num_four = 0; int num_three = 0;
    for(int dir = 0;dir < 4;dir ++){
        if(check_open_four(state,row,col,dir)) return 100000; //活四必胜
        if(check_four(state,row,col,dir)){ 
            num_four ++;
            score += 10000;  //记录冲四加分和数量
        }
        if(check_open_three(state,row,col,dir)){
            num_three ++;
            score += 5000; //记录活三加分和数量
        }
        if(check_three(state,row,col,dir)) score += 1000;  //眠三加分
        if(check_two(state,row,col,dir)) score += 500;  //活二加分
    }
    if(num_three >= 2 || num_four > 0 && num_three > 0) return 100000;  //双活三或者四三架，必胜
    return score;
    
}




int generate_position(GameState *gamestate,Position candidates[],Player aiplayer){
    int count = -1;
    CellState aicolor = (aiplayer == PLAYER_BLACK)? BLACK:WHITE;
    int adjcent[BOARD_SIZE][BOARD_SIZE] = {0};
    for(int row=0;row < BOARD_SIZE;row++){
        for(int col = 0;col< BOARD_SIZE;col++){
            if(gamestate->board[row][col] != EMPTY){
                for(int dr = -2;dr <= 2;dr++){
                    for(int dc = -2;dc <= 2;dc++){
                        int nr = row + dr;
                        int nc = col + dc;
                        if(nr>=0 && nr<BOARD_SIZE && nc>=0 && nc<BOARD_SIZE){
                            if(gamestate->board[nr][nc]==EMPTY){
                                adjcent[nr][nc] = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    for(int r = 0;r < BOARD_SIZE;r ++){
        for(int c = 0;c < BOARD_SIZE;c ++){
            if(count >= MaxCandidate - 1) return count;
            if(adjcent[r][c] == 1){
                ++ count;
                candidates[count].row = r;
                candidates[count].col = c;
            }
        }
    }

    if(count == -1){
        candidates[0].row = BOARD_SIZE / 2;
        candidates[0].col = BOARD_SIZE / 2;
        return 1;
    }

    return 0;
}












