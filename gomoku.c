// A main function to start a gomoku game
#include "board.h"
#include "check.h"


// Main function to start the game
// 主函数，开启游戏
int main(int argc, char *argv[]) {
    printf("Welcome to Gomoku!\n");

    // Initialize the game state
    // 初始化游戏状态
    GameState gameState;
    initializeGame(&gameState);
    GameMode gameMode = MODE_PVP;
    Player personPlayer;//人类玩家，仅用于pve模式
    Player aiPlayer; //设置ai对手

    // Handle input before starting the game loop
    // 在开始游戏循环前处理输入
    char input[100];

    // Default to Player vs Player
    // 默认选择玩家对玩家模式
    int mode = 1;

    // TODO: Use argc and argv to set up game mode. Default to Player vs Player.
    // 可以选择使用 argc 和 argv 来设置游戏模式，默认玩家对玩家模式。也可以选择其他方式输入。

    // 设置方式1：命令行输入
    if(argc > 1){
        if(strcmp(argv[1],"1") == 0){
            gameMode = MODE_PVP;
            mode = 1;
        }
        else if(strcmp(argv[1],"2") == 0){
            gameMode = MODE_PVE;
            mode = 2;
        }
    }
    if(argc > 2 && mode == 2){  //有多个参数，并且pve模式
        if(strcmp(argv[2],"1") == 0){
            //人类先手，下黑棋
            gameState.currentPlayer = PLAYER_BLACK;
            personPlayer = PLAYER_BLACK;
            aiPlayer = PLAYER_WHITE;
        }
        else if(strcmp(argv[2],"2") == 0){
            //ai先手，下黑棋
            gameState.currentPlayer = PLAYER_BLACK;
            personPlayer = PLAYER_WHITE;
            aiPlayer = PLAYER_BLACK;
        }
    }
    //设置方式2：交互式输入
    if(argc<=1){
        int read;
        printf("请选择对战模式:(1:PVP,2:PVE)");
        fgets(input,sizeof(input),stdin);
        sscanf(input,"%d",&read);
        if(read == 2){
            gameMode = MODE_PVE;
            mode = 2;
        }
    }


    if (mode == 1) {
        printf("You selected Player vs Player mode.\n");
    } else {
        printf("You selected Player vs AI mode.\n");
        printf("Who goes first? (1: You, 2: AI): ");
        // TODO: Use stdin to ask who goes first
        // 可以选择使用标准输入来询问谁先手。
        int first;
        fgets(input,sizeof(input),stdin);
        sscanf(input,"%d",&first);
        if(first == 1){
            gameState.currentPlayer = PLAYER_BLACK;
            personPlayer = PLAYER_BLACK;
            aiPlayer = PLAYER_WHITE;
            printf("你先手,你使用黑棋\n");
        }
        else if(first == 2){
            gameState.currentPlayer = PLAYER_BLACK;
            personPlayer = PLAYER_WHITE;
            aiPlayer = PLAYER_BLACK;
            printf("ai先手,你使用白棋\n");
        }
    }

    // Start the game loop here
    // 在这里开始游戏主体的循环
    
    while (1) {
        // TODO: Implement game loop, input handling, move validation, win checking, etc.
        // 实现游戏循环、输入处理、落子合法性检查、胜利判断等功能。
        printBoard(&gameState);
        if(gameMode == MODE_PVP){
            if(gameState.currentPlayer == PLAYER_BLACK){
                printf("轮到黑方落子\n");
            }
            else printf("轮到白方落子\n");

        }
        else{       //pve模式
            if(gameState.currentPlayer == personPlayer){
                if(personPlayer == PLAYER_BLACK){
                    printf("玩家(黑方)请落子\n");
                }
                else {
                    printf("玩家(白方)请落子\n");
                }
            }
            else {
                printf("AI正在思考中\n");
            }
        }
        //处理落子
        int row;
        char char_col;
        if(gameMode == MODE_PVP || gameState.currentPlayer == personPlayer){
            printf("请输入你的落子位置:比如:A(col)2(row)");
            fgets(input,sizeof(input),stdin);
            if(sscanf(input,"%c%d",&char_col,&row) == 2){
                int col = char_col - 'A';
                row --;

                if(gameState.currentPlayer == BLACK && ban(&gameState,row,col)){
                    printf("你违反了禁手，黑方负！\n");
                    break;
                }
                else if(!isValidMove(&gameState,row,col)){
                    printf("落子非法！请重新选择位置\n");
                }
                else if(isValidMove(&gameState,row,col)){
                    gameState.lastMove.col = col;
                    gameState.lastMove.row = row;
                    if(gameState.currentPlayer == PLAYER_BLACK){
                        gameState.board[row][col] = BLACK;
                        if(checkWin(&gameState,row,col) == 1){
                            printf("黑方胜利!\n");
                            break;
                        }
                        gameState.currentPlayer = PLAYER_WHITE;
                        
                    }
                    else if(gameState.currentPlayer == PLAYER_WHITE){
                        gameState.board[row][col] = WHITE;
                        if(checkWin(&gameState,row,col) == 2){
                            printf("白方胜利!\n");
                            break;
                        }
                        gameState.currentPlayer = PLAYER_BLACK;
                    }
                }

            }
            else {
                printf("输入格式有误！请重新输入\n");
            }
        }
        //AI下棋部分
        else {
            for(int i = 0;i < BOARD_SIZE;i++){
                for(int j = 0;j < BOARD_SIZE;j++){
                    
                }
            }
        }



        
        
    }

    return 0;
}
