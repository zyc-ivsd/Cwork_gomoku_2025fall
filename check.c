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
    Check_side side;//只检查连子数量和两端开放情况
    side.aligned = 1;
    side.one_side_open = 1;
    side.other_side_open = 1;
    int dx = direction[dir][0];
    int dy = direction[dir][1];
    for(int i = 0;i < 5;i ++){
        int x = col + dx;
        int y = row + dy;
        if(x < 0||x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE){
            side.one_side_open = 0;//说明碰到棋盘边界，将一端设为封闭
            break;
        }
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
        if(x < 0||x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE){
            side.other_side_open = 0; //说明碰到棋盘边界，将一端设为封闭
            break;
        }
        else{
            if(gamestate ->board[y][x] == current) side.aligned ++;
            else if(gamestate ->board[y][x] == vs){
                side.other_side_open = 0;
                break;
            }
            else if(gamestate ->board[y][x] == EMPTY) break;
        } 
        }
    return side;          
}
//辅助检查跳活三和冲四
Check_side check_with_empty(GameState *gamestate,int row,int col,int dir){
    CellState current = gamestate ->board[row][col];
    CellState vs = check_vs(gamestate,row,col); 
    Check_side side;
    side.aligned = 1;
    side.one_side_open = 1;
    side.other_side_open = 1;
    int dx = direction[dir][0];
    int dy = direction[dir][1];
    int have_empty = 0;//特殊空格数量
    for(int i = 0;i < 5;i ++){
        int x = col + dx;
        int y = row + dy;
        if(x < 0||x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE){ 
            side.one_side_open  = 0 ;
            break;
        }
        else{
            if(gamestate ->board[y][x] == current) side.aligned ++;
            else if(gamestate ->board[y][x] == vs){
                side.one_side_open = 0;
                break;
            }
            else if(gamestate ->board[y][x] == EMPTY ){
                //检查有无特殊空格，也就是两边都是目标棋子
                if(have_empty == 1) break;//特殊空格数量只记录一次
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
        if(x < 0||x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE){
            side.other_side_open = 0; 
            break;
        }
        else{
            if(gamestate ->board[y][x] == current) side.aligned ++;
            else if(gamestate ->board[y][x] == vs){
                side.one_side_open = 0;
                break;
            }
            else if(gamestate ->board[y][x] == EMPTY ){
                if(have_empty == 1) break;
                if(gamestate ->board[y + dy][x + dx] == current){
                    have_empty = 1;
                    side.aligned ++;
                }

            }
        } 
        }
    return side;
}

//辅助检查连活三
Check_side check_with_empty2(GameState *gamestate,int row,int col,int dir){
    Check_side side;
    CellState current = gamestate->board[row][col];
    CellState vs = check_vs(gamestate,row,col);
    int has_empty = 0;
    side.aligned = 1;
    side.one_side_open = 1;
    side.other_side_open = 1;
    int dx = direction[dir][0];
    int dy = direction[dir][1];
    for(int i = 0; i < 5;i++){
        int x = col + dx;
        int y = row + dy;
        if(x<0 && x>= BOARD_SIZE && y<0 && y>= BOARD_SIZE){
            side.one_side_open = 0;
            break;
        }
        else{
            if(gamestate->board[y][x] == current) side.aligned ++;
            else if(gamestate->board[y][x] == vs){
                side.one_side_open = 0;
                break;
            }
            else if(gamestate->board[y][x] == EMPTY){
                if(has_empty == 1) break;
                else{
                    if(gamestate->board[y+dy][x+dx] == EMPTY){
                        has_empty = 1;
                        side.aligned ++;
                    }
                }
            }
        }
    }
    for(int i = 0; i < 5;i++){
        int x = col - dx;
        int y = row - dy;
        if(x<0 && x>= BOARD_SIZE && y<0 && y>= BOARD_SIZE){
            side.other_side_open = 0;
            break;
        }
        else{
            if(gamestate->board[y][x] == current) side.aligned ++;
            else if(gamestate->board[y][x] == vs){
                side.other_side_open = 0;
                break;
            }
            else if(gamestate->board[y][x] == EMPTY){
                if(has_empty == 1) break;
                else{
                    if(gamestate->board[y-dy][x-dx] == EMPTY){
                        has_empty = 1;
                        side.aligned ++;
                    }
                }
            }
        }
    }
    return side;
}

//检查在当前位置落下棋子是否形成活三
int check_open_three(GameState *gamestate,int row,int col,int dir){
    int num_three = 0;
    Check_side side = check_with_empty2(gamestate,row,col,dir);
    if(side.aligned == 4 && side.one_side_open && side.other_side_open){
        num_three ++;
    }//用于检查连活三 _XXX_ _ 
    Check_side side_with_empty = check_with_empty(gamestate,row,col,dir);
    if(side_with_empty.aligned == 4 && side_with_empty.one_side_open && side_with_empty.other_side_open){
        num_three ++;
    }//用于检查跳活三,仅包含一个空格的连续序列数量为4 _XX_X_
    
    return num_three;
}


// 检查在当前位置落下棋子是否形成活四
int check_open_four(GameState *gamestate,int row,int col,int dir){
    
    Check_side side = check_side(gamestate,row,col,dir);
    if(side.aligned == 4 && side.one_side_open && side.other_side_open){
        return 1;
    }   //_XXXX_
    
    return 0;
}

//检查在当前位置落下棋子是否形成冲四
int check_four(GameState *gamestate,int row,int col,int dir){
    int num_four = 0;
    Check_side side = check_side(gamestate,row,col,dir);
    if(side.aligned == 4 && (side.one_side_open ^ side.other_side_open)){
        num_four ++;
    } // OXXXX_  |XXXX_
    Check_side side_with_empty = check_with_empty(gamestate,row,col,dir);
    if(side_with_empty.aligned == 5){
        num_four ++;
    }  // XX_XX  XXX_X
    
    return num_four;
}

//检查同一方向的冲四数目是否大于1
int check_oneline_four(GameState *gamestate,int row ,int col,int dir){
    int check_1 = 1;
    int check_2 = 1;
    int check_3 = 1;
    int check_4 = 1;
    CellState current = gamestate->board[col][row];
    CellState vs = check_vs(gamestate,row,col);
    //双冲四情况，最可能是中间那颗落子造成
    int chess[9] = {-1,-1,-1,-1,1,-1,-1,-1,-1};
    int check1[9] = {1,1,1,0,1,0,1,1,1};
    int check2[9] = {-1,1,0,1,1,1,0,1,-1};
    int check3[9] = {-1,1,1,0,1,1,0,1,1};
    int dx = direction[dir][0];
    int dy = direction[dir][1];
    for(int i = 1; i <= 4;i++){
        int x = col + dx;
        int y = row + dy;
        if(x<0 && x >= BOARD_SIZE && y<0 && y>= BOARD_SIZE) break;
        else {
            if(gamestate->board[y][x]==current) chess[4 + i] = 1;
            else if(gamestate->board[y][x]==vs){
                chess[4 + i] = 2;
                break;
            }
            else chess[4 + i] = 0;
        }
    }
    for(int i = 1; i <= 4;i++){
        int x = col - dx;
        int y = row - dy;
        if(x<0 && x >= BOARD_SIZE && y<0 && y>= BOARD_SIZE) break;
        else {
            if(gamestate->board[y][x]==current) chess[4 - i] = 1;
            else if(gamestate->board[y][x]==vs){
                chess[4 - i] = 2;
                break;
            }
            else chess[4 - i] = 0;
        }
    }
    for(int i = 0;i < 9;i++){
        if(chess[i] != check1[i]){
            check_1 = 0;
            break;
        }
    }
    if(check_1 == 1) return 1;

    for(int i = 1;i < 8;i ++){
        if(chess[i] != check2[i]){
            check_2 = 0;
            break;
        }
    }
    if(check_2 == 1) return 1;
    for(int i = 1;i < 9;i++){
        if(chess[i] != check3[i]){
            check_3 = 0;
            break;
        }
    }
    if(check_3 == 1) return 1;
    for(int i = 1;i < 9;i++){
        if(chess[i-1]!= check3[i]){
            check_4 = 0;
            break;
        }
    }
    return check_4 == 1;
}

//检查在当前位置落下棋子是否形成双四
int check_double_four(GameState *gamestate,int row,int col){
    int num_four = 0;
    for(int dir = 0;dir < 4;dir ++){
        if(check_four(gamestate,row,col,dir) == 1) num_four ++;
        if(check_open_four(gamestate,row,col,dir) == 1) num_four ++;
        if(check_oneline_four(gamestate,row,col,dir) == 1) return 1;
    }  //检查四个不同方向的四的数目
    //检查同一方向的冲四数目 XXX_X_XXX  XX_XX_XX  X_XXX_X
    
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
//除此之外实现功能：检查双三，双四，长连,以及混合禁手
int ban(GameState *gameState, int row ,int col) {
    if(check_overline(gameState,row,col)) return 1;
    int multi = 0;
    for(int dir = 0;dir < 4;dir++){
        if(check_four(gameState,row,col,dir) == 1) multi++;
        if(check_oneline_four(gameState,row,col,dir)==1) return 1;
        if(check_open_four(gameState,row,col,dir)==1)multi++;
        if(check_open_three(gameState,row,col,dir)==1)multi++;

    }
    return multi >= 2;
}
