// A main function to start a gomoku game
// 开启一局五子棋游戏的主函数
// All code here can be modified and extended as needed
// 所有代码可根据需要进行修改和扩展

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define board size
// 定义棋盘大小
#define BOARD_SIZE 15

// enum of board cells
// 棋盘格子的状态枚举
typedef enum {
    EMPTY,
    BLACK,
    WHITE
} CellState;

// enum of players
// 玩家枚举
typedef enum {
    PLAYER_BLACK,
    PLAYER_WHITE
} Player;

// enum of game modes
// 游戏模式枚举
typedef enum {
    MODE_PVP, // Player vs Player
    MODE_PVE  // Player vs AI
} GameMode;

// structure of board position
// 棋盘位置结构体
typedef struct {
    int row;
    int col; // column
} Position;

// structure of the game state can be defined here
// 游戏状态结构体
typedef struct {
    CellState board[BOARD_SIZE][BOARD_SIZE];
    Player currentPlayer; // whose turn it is
    Position lastMove; // where the last move made 
}GameState;

// Function to initialize the game state
// 初始化游戏状态的函数
void initializeGame(GameState *gameState) {
    gameState->currentPlayer = PLAYER_BLACK;
    gameState->lastMove.col = -1;    
    gameState->lastMove.row = -1;
//刚开始执行程序的的时候，有莫名其妙的输出：Your last move was A2
//经询问AI，得知是因为没有初始化lastmove，分配的是垃圾值，所以这里给个无效初始化

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            gameState->board[i][j] = EMPTY;
        }
    }
}


// Function to print the board
// Use numbers to represent columns, characters to represent rows
// 打印棋盘的函数。使用数字表示列，字符表示行
void printBoard(GameState *gameState) {
    // Display the last move
    if(gameState->lastMove.col < 0 || gameState->lastMove.row < 0){
        printf("No move now\n");
    }
    else printf("Your last move was: %c%d\n", (char)gameState->lastMove.col + 'A', gameState->lastMove.row + 1);

    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", BOARD_SIZE - i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (gameState->board[BOARD_SIZE-1-i][j] == BLACK) { // is black stone
                if (j == BOARD_SIZE - 1) {
                    if (i == BOARD_SIZE-1-gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("▲\n");
                    else
                        printf("●\n");
                } else {
                    if (i == BOARD_SIZE-1-gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("▲─");
                    else
                        printf("●─");
                }
            }
            else if (gameState->board[BOARD_SIZE-1-i][j] == WHITE) { // is white stone
                if (j == BOARD_SIZE - 1) {
                    if (i == BOARD_SIZE -1-gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("△\n");
                    else
                        printf("◎\n");
                } else {
                    if (i == BOARD_SIZE -1- gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("△─");
                    else
                        printf("◎─");
                }
            } else if (i == 0) { // top row empty
                if (j == 0)                   { printf("┌─");  }
                else if (j == BOARD_SIZE - 1) { printf("┐\n"); }
                else                          { printf("┬─");  }
            } else if (i == BOARD_SIZE - 1) { // bottom row empty
                if (j == 0)                   { printf("└─");  }
                else if (j == BOARD_SIZE - 1) { printf("┘\n"); }
                else                          { printf("┴─");  }
            } else { // middle empty
                if (j == 0)                   { printf("├─");  }
                else if (j == BOARD_SIZE - 1) { printf("┤\n"); }
                else                          { printf("┼─");  }
            }
        }
    }

    printf("  ");
    for (int i = 0; i < BOARD_SIZE; i++)
        printf(" %c", 'A' + i);
    printf("\n");
}


// Function of win checking. 0: no win, 1: black wins, 2: white wins
// 判断胜利的函数。0: 无胜利，1: 黑方胜利，2: 白方胜利
int checkWin(GameState *gameState, int row, int col) {
    // TODO: Implement win checking logic
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


//先手禁手函数,如果落子违反禁手规则那么返回1,否则返回0
int ban(GameState *gameState, int row ,int col) {
    
    return 0;
}


// Function to check valid move
// 检查合法落子的函数,合法返回1,不合法返回0
int isValidMove(GameState *gameState, int row, int col) {
    // TODO: Implement move validation logic
    if(gameState->board[row][col] != EMPTY){
        return 0;
    }
    if(row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE){
        return 0;
    }

    return 1;
}
//优先权函数
int priority(GameState *gameState,int row,int col){
    //检查五连
    if(checkWin(gameState,row,col)){
        return 10000;
    }
    //检查活四
    
}



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
        else {
            for(int i = 0;i < BOARD_SIZE;i++){
                for(int j = 0;j < BOARD_SIZE;j++){
                    
                }
            }
        }



        
        
    }

    return 0;
}
