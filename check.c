#include "board.h"

int checkWin(GameState *gameState, int row, int col) {
    
    CellState current = gameState->board[row][col];
    for(int d = 0; d < 4; d++){      //遍历四个方向
        int dx = direction[d][0];
        int dy = direction[d][1];
        int aligned = 1;   //记录连在一行的棋子个数,当前位置为1

        for(int i = 1;i < 5;i++){
            int x = col + i*dx;
            int y = row + i*dy;

            if(x >= BOARD_SIZE|| x < 0 || y >= BOARD_SIZE|| y <0){
                break;         //处理越界
            }
            else{
                if(gameState->board[y][x] == current){
                    aligned ++;
                    if(aligned == 5){   //检测到五子立即返回
                        return (current == BLACK)? 1:2;
                    }
                }
                else{
                    break;
                }
            }
        }
        for(int i = 1;i < 5;i++){
            int x = col - i*dx;
            int y = row - i*dy;

            if(x < 0 ||x >= BOARD_SIZE || y < 0|| y >= BOARD_SIZE){
                break;         //处理越界
            }
            else{
                if(gameState->board[y][x] == current){
                    aligned ++;
                    if(aligned == 5){
                        return (current == BLACK)? 1:2;
                    }
                }
                else{
                    break;
                }
            }
        }
    }
    return 0;
}


// 检查合法落子的函数,合法返回1,不合法返回0
int isValidMove(GameState *gameState, int row, int col) {
    if(gameState->board[row][col] != EMPTY){
        return 0;
    }
    if(row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE){
        return 0;
    }

    return 1;
}
//检查对手棋色
CellState check_vs(GameState *gamestate,int row,int col){
    CellState current = gamestate ->board[row][col];
    if(current == EMPTY) return EMPTY;
    else if(current == BLACK) return WHITE;
    else return BLACK;
}


//返回检查边界结构体，专门用于检查活四长连等棋型
Check_side check_side(GameState *gamestate,int row,int col,int dir){
    CellState current = gamestate ->board[row][col];
    CellState vs = check_vs(gamestate,row,col); 
    Check_side side;
    side.aligned = 1;
    side.one_side_open = 1;
    side.other_side_open = 1;
    int dx = direction[dir][0];
    int dy = direction[dir][1];
    for(int i = 0;i < 5;i ++){
        int x = col + dx;
        int y = row + dy;
        if(x < 0||x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) break;
        else{
            if(gamestate ->board[y][x] == current) side.aligned ++;
            else if(gamestate ->board[y][x] == vs){
                side.one_side_open = 0;
                break;
            }
            else if(gamestate ->board[y][x] == EMPTY) break;
        } 
        }
    for(int i = 0;i < 5;i ++){
        int x = col - dx;
        int y = row - dy;
        if(x < 0||x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) break;
        else{
            if(gamestate ->board[y][x] == current) side.aligned ++;
            else if(gamestate ->board[y][x] == vs){
                side.one_side_open = 0;
                break;
            }
            else if(gamestate ->board[y][x] == EMPTY) break;
        } 
        }
    return side;          
}
//辅助检查活三和冲四
Check_side check_with_empty(GameState *gamestate,int row,int col,int dir){
    CellState current = gamestate ->board[row][col];
    CellState vs = check_vs(gamestate,row,col); 
    Check_side side;
    side.aligned = 1;
    side.one_side_open = 1;
    side.other_side_open = 1;
    int dx = direction[dir][0];
    int dy = direction[dir][1];
    int have_empty = 0;
    for(int i = 0;i < 5;i ++){
        int x = col + dx;
        int y = row + dy;
        if(x < 0||x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) break;
        else{
            if(gamestate ->board[y][x] == current) side.aligned ++;
            else if(gamestate ->board[y][x] == vs){
                side.one_side_open = 0;
                break;
            }
            else if(gamestate ->board[y][x] == EMPTY ){
                if(have_empty == 1) break;
                if(gamestate ->board[y + dy][x + dx] == current && 
                gamestate -> board[y - dy][x - dx] == current && have_empty == 0){
                    have_empty = 1;
                    side.aligned ++;
                }

            }
        } 
        }
    for(int i = 0;i < 5;i ++){
        int x = col - dx;
        int y = row - dy;
        if(x < 0||x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) break;
        else{
            if(gamestate ->board[y][x] == current) side.aligned ++;
            else if(gamestate ->board[y][x] == vs){
                side.one_side_open = 0;
                break;
            }
            else if(gamestate ->board[y][x] == EMPTY ){
                if(have_empty == 1) break;
                if(gamestate ->board[y + dy][x + dx] == current && 
                gamestate -> board[y - dy][x - dx] == current && have_empty == 0){
                    have_empty = 1;
                    side.aligned ++;
                }

            }
        } 
        }
    return side;
}



//检查在当前位置落下棋子是否形成活三
int check_open_three(GameState *gamestate,int row,int col,int dir){
    
    Check_side side = check_side(gamestate,row,col,dir);
    if(side.aligned == 3 && side.one_side_open && side.other_side_open){
        return 1;
    }
    Check_side side_with_empty = check_with_empty(gamestate,row,col,dir);
    if(side_with_empty.aligned == 4 && side_with_empty.one_side_open && side_with_empty.other_side_open){
        return 1;
    }
    
    return 0;
}


// 检查在当前位置落下棋子是否形成活四
int check_open_four(GameState *gamestate,int row,int col,int dir){

    Check_side side = check_side(gamestate,row,col,dir);
    if(side.aligned == 4 && side.one_side_open && side.other_side_open){
        return 1;
    }
    
    return 0;
}

//检查在当前位置落下棋子是否形成冲四
int check_four(GameState *gamestate,int row,int col,int dir){

    Check_side side = check_side(gamestate,row,col,dir);
    if(side.aligned == 4 && (side.one_side_open ^ side.other_side_open)){
        return 1;
    }
    Check_side side_with_empty = check_with_empty(gamestate,row,col,dir);
    if(side_with_empty.aligned == 5){
        return 1;
    }
    
    return 0;
}


//检查在当前位置落下棋子是否形成双四
int check_double_four(GameState *gamestate,int row,int col){
    int num_four = 0;
    for(int dir = 0;dir < 4;dir ++){
        if(check_four(gamestate,row,col,dir) == 1) num_four ++;
        if(check_open_four(gamestate,row,col,dir) == 1) num_four ++;
    }
    return num_four >= 2;
    
}


//检查在当前位置落下棋子是否形成双三
int check_double_three(GameState *gamestate,int row,int col){
    int num_three = 0;
    for(int dir = 0;dir < 4;dir ++){
        if(check_open_three(gamestate,row,col,dir) == 1) num_three ++;
    }
    return num_three >= 2;
}

//检查是否长连
int check_overline(GameState *gamestate,int row,int col){
    for(int dir = 0;dir < 4;dir ++){
        Check_side side = check_side(gamestate,row,col,dir);
        if(side.aligned >= 6){
            return 1;
        }
    }   
    return 0;
}



//先手禁手函数,如果落子违反禁手规则那么返回1,否则返回0
//简化禁手：判断活三时，不考虑在下一步成为活四是不是禁手
//除此之外实现功能：检查双三，双四，长连
int ban(GameState *gameState, int row ,int col) {
    if(check_double_three(gameState,row,col)) return 1;
    if(check_double_four(gameState,row,col)) return 1;
    if(check_overline(gameState,row,col)) return 1;
    return 0;
}
