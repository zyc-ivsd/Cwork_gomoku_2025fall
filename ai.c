#include "board.h"
#include "check.h"
#include "ai.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define SEARCH_DEPTH 10
#define MAX_SCORE 1000000
#define MIN_SCORE -1000000
#define MaxCandidate 30
#define MinScore 100
#define TIME_LIMIT 15.0  // 每步最大思考时间15秒

// 函数声明
int evaluate_position(GameState *gamestate, int row, int col, Player player);
int calculate_threat_score(GameState *state, int row, int col, Player opponent);
int calculate_attack_score(GameState *state, int row, int col, Player player);
int generate_position(GameState *gamestate, Position candidates[], Player aiplayer);
Position bestMove(GameState *state, Player aiplayer);
int alphaBetaSearch(GameState *state, int depth, int alpha, int beta, Player player, int isMaximizing, clock_t start_time);
double get_elapsed_time(clock_t start, clock_t end);
int evaluate_board_state(GameState *state, Player ai_player, Player human_player);
int has_opponent_threat(GameState *state, int row, int col, Player opponent);

// 主要AI决策函数
Position bestMove(GameState *state, Player aiplayer) {
    clock_t start_time = clock();
    
    printf("AI正在思考中\n");
    
    Position move;
    Position candidates[MaxCandidate];
    int candidate_num = generate_position(state, candidates, aiplayer);
    
    // 如果没有找到候选位置，则返回中心位置
    if (candidate_num <= 0) {
        move.row = BOARD_SIZE / 2;
        move.col = BOARD_SIZE / 2;
        clock_t end_time = clock();
        printf("AI落子位置: %c%d\n", 'A' + move.col, move.row + 1);
        printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
        return move;
    }
    
    CellState ai_color = (aiplayer == PLAYER_BLACK) ? BLACK : WHITE;
    CellState human_color = (aiplayer == PLAYER_BLACK) ? WHITE : BLACK;
    Player human_player = (aiplayer == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK;
    
    // 优先级1：检查是否有立即获胜的走法
    for (int i = 0; i < candidate_num && i < 15; i++) {
        int row = candidates[i].row;
        int col = candidates[i].col;
        
        if (state->board[row][col] != EMPTY) {
            continue;
        }
        
        state->board[row][col] = ai_color;
        if (checkWin(state, row, col)) {
            state->board[row][col] = EMPTY;
            move.row = row;
            move.col = col;
            clock_t end_time = clock();
            printf("AI落子位置: %c%d (必胜)\n", 'A' + move.col, move.row + 1);
            printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
            return move;
        }
        state->board[row][col] = EMPTY;
    }
    
    // 优先级2：检查是否需要防守对手五连（最后一步获胜）
    for (int i = 0; i < candidate_num && i < 15; i++) {
        int row = candidates[i].row;
        int col = candidates[i].col;
        
        if (state->board[row][col] != EMPTY) {
            continue;
        }
        
        state->board[row][col] = human_color;
        if (checkWin(state, row, col)) {
            state->board[row][col] = EMPTY;
            move.row = row;
            move.col = col;
            clock_t end_time = clock();
            printf("AI落子位置: %c%d (防守必胜)\n", 'A' + move.col, move.row + 1);
            printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
            return move;
        }
        state->board[row][col] = EMPTY;
    }
    
    // 优先级3：计算棋局整体得分，决定防守优先或进攻优先
    int board_state = evaluate_board_state(state, aiplayer, human_player);
    int defense_priority = (board_state < 0);  // 如果对手威胁大，优先防守
    
    // 优先级4：在防守优先的情况下，检查对手是否有活三或冲四
    if (defense_priority) {
        for (int i = 0; i < candidate_num && i < 15; i++) {
            int row = candidates[i].row;
            int col = candidates[i].col;
            
            if (state->board[row][col] != EMPTY) {
                continue;
            }
            
            // 临时放置对手棋子，检查威胁
            state->board[row][col] = human_color;
            int threat_count = 0;
            
            // 检查对手活三
            for (int dir = 0; dir < 4; dir++) {
                if (check_open_three(state, row, col, dir)) {
                    threat_count++;
                }
            }
            
            // 检查对手冲四
            for (int dir = 0; dir < 4; dir++) {
                if (check_four(state, row, col, dir)) {
                    threat_count += 2;  // 冲四权重更高
                }
            }
            
            // 检查对手活四（最高威胁）
            for (int dir = 0; dir < 4; dir++) {
                if (check_open_four(state, row, col, dir)) {
                    threat_count += 5;
                }
            }
            
            state->board[row][col] = EMPTY;
            
            // 如果对手在此处形成活三、冲四或活四，必须防守
            if (threat_count > 0) {
                move.row = row;
                move.col = col;
                clock_t end_time = clock();
                printf("AI落子位置: %c%d (防守威胁: %d)\n", 'A' + move.col, move.row + 1, threat_count);
                printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
                return move;
            }
        }
    }
    
    // 优先级5：检查AI自己的进攻机会（活三、冲四、活四）
    int best_attack_idx = -1;
    int best_attack_score = -1;
    
    for (int i = 0; i < candidate_num && i < 15; i++) {
        int row = candidates[i].row;
        int col = candidates[i].col;
        
        if (state->board[row][col] != EMPTY) {
            continue;
        }
        
        state->board[row][col] = ai_color;
        int attack_score = 0;
        
        // 检查AI自己的活四（最高优先）
        for (int dir = 0; dir < 4; dir++) {
            if (check_open_four(state, row, col, dir)) {
                attack_score += 5000;
            }
        }
        
        // 检查AI自己的冲四
        for (int dir = 0; dir < 4; dir++) {
            if (check_four(state, row, col, dir)) {
                attack_score += 2000;
            }
        }
        
        // 检查AI自己的活三
        for (int dir = 0; dir < 4; dir++) {
            if (check_open_three(state, row, col, dir)) {
                attack_score += 800;
            }
        }
        
        state->board[row][col] = EMPTY;
        
        if (attack_score > best_attack_score) {
            best_attack_score = attack_score;
            best_attack_idx = i;
        }
    }
    
    // 如果找到了好的进攻位置，优先考虑
    if (best_attack_idx != -1 && best_attack_score > 500) {
        move = candidates[best_attack_idx];
        clock_t end_time = clock();
        printf("AI落子位置: %c%d (进攻: %d)\n", 'A' + move.col, move.row + 1, best_attack_score);
        printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
        return move;
    }
    
    // 优先级6：使用Alpha-Beta搜索进行深层评估
    int best_score = MIN_SCORE;
    int best_idx = -1;
    
    for (int i = 0; i < candidate_num && i < 15; i++) {
        int row = candidates[i].row;
        int col = candidates[i].col;
        
        if (state->board[row][col] != EMPTY) {
            continue;
        }
        
        state->board[row][col] = ai_color;
        state->lastMove.row = row;
        state->lastMove.col = col;
        
        int score = alphaBetaSearch(state, SEARCH_DEPTH - 1, MIN_SCORE, MAX_SCORE, 
                                   aiplayer, 0, start_time);
        
        state->board[row][col] = EMPTY;
        
        // 检查是否超时
        if (get_elapsed_time(start_time, clock()) >= TIME_LIMIT) {
            printf("AI search timeout!\n");
            break;
        }
        
        if (score > best_score) {
            best_score = score;
            best_idx = i;
        }
    }
    
    // 如果没有找到合适的位置，选择第一个可用的空位
    if (best_idx == -1) {
        for (int i = 0; i < candidate_num; i++) {
            int row = candidates[i].row;
            int col = candidates[i].col;
            if (state->board[row][col] == EMPTY) {
                best_idx = i;
                break;
            }
        }
        
        if (best_idx == -1) {
            move.row = BOARD_SIZE / 2;
            move.col = BOARD_SIZE / 2;
            clock_t end_time = clock();
            printf("AI落子位置: %c%d\n", 'A' + move.col, move.row + 1);
            printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
            return move;
        }
    }
    
    move = candidates[best_idx];
    clock_t end_time = clock();
    printf("AI落子位置: %c%d\n", 'A' + move.col, move.row + 1);
    printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
    return move;
}

// Alpha-beta剪枝搜索算法，支持10层深度
int alphaBetaSearch(GameState *state, int depth, int alpha, int beta, Player aiplayer, int isMaximizing, clock_t start_time) {
    // 检查是否超时
    if (get_elapsed_time(start_time, clock()) >= TIME_LIMIT) {
        return 0;
    }
    
    if (depth == 0) {
        // 到达搜索深度，返回静态评估值
        Position lastMove = state->lastMove;
        return evaluate_position(state, lastMove.row, lastMove.col, 
                                isMaximizing ? aiplayer : (aiplayer == PLAYER_BLACK ? PLAYER_WHITE : PLAYER_BLACK));
    }
    
    CellState ai_color = (aiplayer == PLAYER_BLACK) ? BLACK : WHITE;
    CellState enemy_color = (aiplayer == PLAYER_BLACK) ? WHITE : BLACK;
    CellState current_color = isMaximizing ? ai_color : enemy_color;
    Player current_player = isMaximizing ? aiplayer : (aiplayer == PLAYER_BLACK ? PLAYER_WHITE : PLAYER_BLACK);
    
    Position candidates[MaxCandidate];
    int candidate_num = generate_position(state, candidates, current_player);
    
    if (candidate_num == 0) {
        return 0;
    }
    
    // 检查是否有立即获胜的位置
    for (int i = 0; i < candidate_num && i < 10; i++) {
        int row = candidates[i].row;
        int col = candidates[i].col;
        
        if (state->board[row][col] != EMPTY) {
            continue;
        }
        
        state->board[row][col] = current_color;
        if (checkWin(state, row, col)) {
            state->board[row][col] = EMPTY;
            return isMaximizing ? MAX_SCORE - depth : MIN_SCORE + depth;
        }
        state->board[row][col] = EMPTY;
    }
    
    // 限制搜索的候选点数量
    int limit = (candidate_num > 10) ? 10 : candidate_num;
    
    if (isMaximizing) {
        int max_eval = MIN_SCORE;
        
        for (int i = 0; i < limit; i++) {
            int row = candidates[i].row;
            int col = candidates[i].col;
            
            if (state->board[row][col] != EMPTY) {
                continue;
            }
            
            state->board[row][col] = current_color;
            state->lastMove.row = row;
            state->lastMove.col = col;
            
            int eval = alphaBetaSearch(state, depth - 1, alpha, beta, aiplayer, 0, start_time);
            
            state->board[row][col] = EMPTY;
            
            if (get_elapsed_time(start_time, clock()) >= TIME_LIMIT) {
                return max_eval;
            }
            
            max_eval = (eval > max_eval) ? eval : max_eval;
            alpha = (max_eval > alpha) ? max_eval : alpha;
            
            if (beta <= alpha) {
                break;  // Beta剪枝
            }
        }
        
        return max_eval;
    } else {
        int min_eval = MAX_SCORE;
        
        for (int i = 0; i < limit; i++) {
            int row = candidates[i].row;
            int col = candidates[i].col;
            
            if (state->board[row][col] != EMPTY) {
                continue;
            }
            
            state->board[row][col] = current_color;
            state->lastMove.row = row;
            state->lastMove.col = col;
            
            int eval = alphaBetaSearch(state, depth - 1, alpha, beta, aiplayer, 1, start_time);
            
            state->board[row][col] = EMPTY;
            
            if (get_elapsed_time(start_time, clock()) >= TIME_LIMIT) {
                return min_eval;
            }
            
            min_eval = (eval < min_eval) ? eval : min_eval;
            beta = (min_eval < beta) ? min_eval : beta;
            
            if (beta <= alpha) {
                break;  // Alpha剪枝
            }
        }
        
        return min_eval;
    }
}

// 计算整个棋局的状态得分
// 返回值：正数表示AI优势，负数表示对手优势
int evaluate_board_state(GameState *state, Player ai_player, Player human_player) {
    int ai_score = 0;
    int human_score = 0;
    
    CellState ai_color = (ai_player == PLAYER_BLACK) ? BLACK : WHITE;
    CellState human_color = (ai_player == PLAYER_BLACK) ? WHITE : BLACK;
    
    // 扫描整个棋盘，评估AI和对手的得分
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state->board[i][j] == ai_color) {
                // 评估AI棋子的强度
                for (int dir = 0; dir < 4; dir++) {
                    if (check_open_three(state, i, j, dir)) ai_score += 100;
                    if (check_four(state, i, j, dir)) ai_score += 200;
                    if (check_open_four(state, i, j, dir)) ai_score += 500;
                }
            } else if (state->board[i][j] == human_color) {
                // 评估对手棋子的威胁
                for (int dir = 0; dir < 4; dir++) {
                    if (check_open_three(state, i, j, dir)) human_score += 100;
                    if (check_four(state, i, j, dir)) human_score += 200;
                    if (check_open_four(state, i, j, dir)) human_score += 500;
                }
            }
        }
    }
    
    return ai_score - human_score;
}

// 计算对手在该位置的威胁得分
int calculate_threat_score(GameState *state, int row, int col, Player opponent) {
    CellState opponent_color = (opponent == PLAYER_BLACK) ? BLACK : WHITE;
    int threat_score = 0;
    
    state->board[row][col] = opponent_color;
    
    for (int dir = 0; dir < 4; dir++) {
        if (check_open_four(state, row, col, dir)) {
            threat_score += 10000;  // 活四威胁最大
        } else if (check_four(state, row, col, dir)) {
            threat_score += 3000;   // 冲四威胁
        } else if (check_open_three(state, row, col, dir)) {
            threat_score += 800;    // 活三威胁
        }
    }
    
    state->board[row][col] = EMPTY;
    return threat_score;
}

// 计算AI在该位置的进攻得分
int calculate_attack_score(GameState *state, int row, int col, Player player) {
    CellState player_color = (player == PLAYER_BLACK) ? BLACK : WHITE;
    int attack_score = 0;
    
    state->board[row][col] = player_color;
    
    for (int dir = 0; dir < 4; dir++) {
        if (check_open_four(state, row, col, dir)) {
            attack_score += 10000;
        } else if (check_four(state, row, col, dir)) {
            attack_score += 3000;
        } else if (check_open_three(state, row, col, dir)) {
            attack_score += 800;
        }
    }
    
    state->board[row][col] = EMPTY;
    return attack_score;
}

// 检查对手是否有威胁（活三、冲四或活四）
int has_opponent_threat(GameState *state, int row, int col, Player opponent) {
    CellState opponent_color = (opponent == PLAYER_BLACK) ? BLACK : WHITE;
    
    state->board[row][col] = opponent_color;
    
    for (int dir = 0; dir < 4; dir++) {
        if (check_open_three(state, row, col, dir) || 
            check_four(state, row, col, dir) || 
            check_open_four(state, row, col, dir)) {
            state->board[row][col] = EMPTY;
            return 1;
        }
    }
    
    state->board[row][col] = EMPTY;
    return 0;
}

// 评估位置函数
int evaluate_position(GameState *gamestate, int row, int col, Player player) {
    CellState ai_color = (player == PLAYER_BLACK) ? BLACK : WHITE;
    CellState human_color = (player == PLAYER_BLACK) ? WHITE : BLACK;
    
    // 评估AI的得分
    int attack_score = calculate_attack_score(gamestate, row, col, player);
    
    // 评估对手的威胁得分
    Player opponent = (player == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK;
    int defense_score = calculate_threat_score(gamestate, row, col, opponent);
    
    // 综合得分：进攻得分 + 防守得分（权重更高）
    return attack_score + (int)(defense_score * 1.2);
}


// 生成候选位置
int generate_position(GameState *gamestate, Position candidates[], Player aiplayer) {
    int count = 0;
    
    // 优先生成靠近现有棋子的位置
    for (int distance = 1; distance <= 2 && count < MaxCandidate; distance++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (gamestate->board[i][j] == EMPTY) {
                    // 检查周围是否有棋子，距离在distance范围内
                    int has_nearby = 0;
                    for (int di = -distance; di <= distance && !has_nearby; di++) {
                        for (int dj = -distance; dj <= distance && !has_nearby; dj++) {
                            if (di == 0 && dj == 0) continue;
                            int ni = i + di;
                            int nj = j + dj;
                            if (ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE && 
                                gamestate->board[ni][nj] != EMPTY) {
                                has_nearby = 1;
                            }
                        }
                    }
                    
                    if (has_nearby) {
                        candidates[count].row = i;
                        candidates[count].col = j;
                        count++;
                        if (count >= MaxCandidate) {
                            return count;
                        }
                    }
                }
            }
        }
    }
    
    return count;
}

// 辅助函数：计算经过的时间
double get_elapsed_time(clock_t start, clock_t end) {
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}