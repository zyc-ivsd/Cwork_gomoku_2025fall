#include "board.h"

int checkWin(GameState *gameState, int row, int col) {
    
    CellState current = gameState->board[row][col];
    //设置四个方向的查找方向向量，确保横向，竖向斜向都能被查找
    int direction[4][2] = {
        {1,0},
        {0,1},
        {1,1},
        {1,-1}
    };
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





//先手禁手函数,如果落子违反禁手规则那么返回1,否则返回0
int ban(GameState *gameState, int row ,int col) {
    
    return 0;
}
