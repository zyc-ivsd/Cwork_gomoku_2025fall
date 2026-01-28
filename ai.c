#include "board.h"
#include "check.h"
#include "ai.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define SEARCH_DEPTH 8
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
Position find_critical_defense(GameState *state, Player opponent);
Position find_vcf_winning_move(GameState *state, Player aiplayer, int depth);
int vcf_search(GameState *state, Player aiplayer, Player current_player, int depth);

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
        
        // 如果AI是黑棋，检查禁手：有禁手则不落子
        if (aiplayer == PLAYER_BLACK && ban(state, row, col)) {
            state->board[row][col] = EMPTY;
            continue;  // 禁手，跳过这个位置
        }
        
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
    
    // 优先级1.5：立即检查对手是否有活三或冲四威胁
    // 这是最关键的防守，必须优先于VCF进攻，防止被反杀
    Position critical_defense = find_critical_defense(state, human_player);
    if (critical_defense.row != -1 && critical_defense.col != -1) {
        move = critical_defense;
        clock_t end_time = clock();
        printf("AI落子位置: %c%d (紧急防守)\n", 'A' + move.col, move.row + 1);
        printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
        return move;
    }
    
    // 优先级2：VCF算法检测必赢局面（只检测活四和双活三）
    // 现在VCF会检查对手的威胁，只返回安全的必赢走法
    Position vcf_move = find_vcf_winning_move(state, aiplayer, 8);
    if (vcf_move.row != -1 && vcf_move.col != -1) {
        move = vcf_move;
        clock_t end_time = clock();
        printf("AI落子位置: %c%d (VCF必赢)\n", 'A' + move.col, move.row + 1);
        printf("AI Time: %.2f seconds\n", get_elapsed_time(start_time, end_time));
        return move;
    }
    
    // 优先级3：检查是否需要防守对手五连（最后一步获胜）
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
    
    // 优先级4：使用Alpha-Beta搜索进行深层评估
    int best_score = MIN_SCORE;
    int best_idx = -1;
    
    for (int i = 0; i < candidate_num && i < 15; i++) {
        int row = candidates[i].row;
        int col = candidates[i].col;
        
        if (state->board[row][col] != EMPTY) {
            continue;
        }
        
        state->board[row][col] = ai_color;
        
        // 如果AI是黑棋，检查禁手：有禁手则不落子
        if (aiplayer == PLAYER_BLACK && ban(state, row, col)) {
            state->board[row][col] = EMPTY;
            continue;  // 禁手，跳过这个位置
        }
        
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

// Alpha-beta剪枝搜索算法，支持8层深度
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
        
        // 如果当前是黑棋，检查禁手：有禁手则不落子
        if (current_player == PLAYER_BLACK && ban(state, row, col)) {
            state->board[row][col] = EMPTY;
            continue;  // 禁手，跳过这个位置
        }
        
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
            
            // 如果当前是黑棋，检查禁手：有禁手则不落子
            if (current_player == PLAYER_BLACK && ban(state, row, col)) {
                state->board[row][col] = EMPTY;
                continue;  // 禁手，跳过这个位置
            }
            
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
    
    // 优先生成靠近现有棋子的位置，扩大距离范围到3以确保紧贴对方棋子
    for (int distance = 1; distance <= 3 && count < MaxCandidate; distance++) {
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
                        // 如果AI是黑棋，检查禁手
                        if (aiplayer == PLAYER_BLACK) {
                            gamestate->board[i][j] = BLACK;
                            if (ban(gamestate, i, j)) {
                                // 有禁手，不落子，跳过这个位置
                                gamestate->board[i][j] = EMPTY;
                                continue;
                            }
                            gamestate->board[i][j] = EMPTY;
                        }
                        
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

// VCF算法：简化版本，只检测明确的必赢情况
// 优先级：活四 > 双活三 > 返回-1（让防守机制接手）
// 改进：检查对手的下一步威胁，只返回安全的必赢走法
Position find_vcf_winning_move(GameState *state, Player aiplayer, int depth) {
    Position no_move = {-1, -1};
    CellState ai_color = (aiplayer == PLAYER_BLACK) ? BLACK : WHITE;
    CellState opponent_color = (aiplayer == PLAYER_BLACK) ? WHITE : BLACK;
    Player opponent = (aiplayer == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK;
    
    Position candidates[MaxCandidate];
    int candidate_num = generate_position(state, candidates, aiplayer);
    
    // 第一遍：查找活四（最明确的必赢）
    for (int i = 0; i < candidate_num && i < 25; i++) {
        int row = candidates[i].row;
        int col = candidates[i].col;
        
        if (state->board[row][col] != EMPTY) {
            continue;
        }
        
        state->board[row][col] = ai_color;
        
        // 如果AI是黑棋，检查禁手：有禁手则跳过
        if (aiplayer == PLAYER_BLACK && ban(state, row, col)) {
            state->board[row][col] = EMPTY;
            continue;  // 禁手，跳过这个位置
        }
        
        // 检查四个方向的活四
        int has_open_four = 0;
        for (int dir = 0; dir < 4; dir++) {
            if (check_open_four(state, row, col, dir)) {
                has_open_four = 1;
                break;
            }
        }
        
        if (has_open_four) {
            // 找到活四，但在返回前检查对手是否有致命威胁
            // 更新lastMove，用于后续评估
            state->lastMove.row = row;
            state->lastMove.col = col;
            
            // 检查对手是否有活三、冲四威胁
            Position opponent_threat = find_critical_defense(state, opponent);
            
            state->board[row][col] = EMPTY;
            
            // 如果对手没有活三/冲四威胁，则这个活四是安全的必赢
            if (opponent_threat.row == -1 && opponent_threat.col == -1) {
                return (Position){row, col};  // 活四必赢且安全
            }
            // 如果对手有威胁，暂时跳过这个位置，继续找其他更安全的走法
            continue;
        }
        
        state->board[row][col] = EMPTY;
    }
    
    // 第二遍：查找双活三（两个方向的活三，对手无法同时防守）
    for (int i = 0; i < candidate_num && i < 25; i++) {
        int row = candidates[i].row;
        int col = candidates[i].col;
        
        if (state->board[row][col] != EMPTY) {
            continue;
        }
        
        state->board[row][col] = ai_color;
        
        // 如果AI是黑棋，检查禁手：有禁手则跳过
        if (aiplayer == PLAYER_BLACK && ban(state, row, col)) {
            state->board[row][col] = EMPTY;
            continue;  // 禁手，跳过这个位置
        }
        
        // 计算活三数量
        int open_three_count = 0;
        for (int dir = 0; dir < 4; dir++) {
            if (check_open_three(state, row, col, dir)) {
                open_three_count++;
            }
        }
        
        if (open_three_count >= 2) {
            // 找到双活三，检查对手是否有即刻威胁
            state->lastMove.row = row;
            state->lastMove.col = col;
            
            // 检查对手是否有活三、冲四威胁
            Position opponent_threat = find_critical_defense(state, opponent);
            
            state->board[row][col] = EMPTY;
            
            // 如果对手没有活三/冲四威胁，则这个双活三是安全的必赢
            if (opponent_threat.row == -1 && opponent_threat.col == -1) {
                return (Position){row, col};  // 双活三必赢且安全
            }
            // 否则继续搜索其他走法
            continue;
        }
        
        state->board[row][col] = EMPTY;
    }
    
    // 如果找不到明确的安全必赢，返回-1，让防守机制接手
    return no_move;
}

// VCF搜索函数保留但不使用（备用）
int vcf_search(GameState *state, Player aiplayer, Player current_player, int depth) {
    return 0;  // 简化版本不使用递归搜索
}

// 辅助函数：计算经过的时间
double get_elapsed_time(clock_t start, clock_t end) {
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// 计算对手威胁的数量
int count_opponent_threats(GameState *state, Player opponent) {
    int threat_count = 0;
    CellState opponent_color = (opponent == PLAYER_BLACK) ? BLACK : WHITE;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state->board[i][j] != opponent_color) {
                continue;
            }
            
            // 检查活三
            for (int dir = 0; dir < 4; dir++) {
                if (check_open_three(state, i, j, dir)) {
                    threat_count++;
                }
            }
        }
    }
    
    return threat_count;
}

// 查找关键防守位置：检测对手的活三、冲四和跳四，返回堵住它的位置
// 如果对手有活三、冲四或跳四，立刻返回堵住的位置
// 优先级：活三 > 跳四(XX_XX) > 冲四
Position find_critical_defense(GameState *state, Player opponent) {
    Position no_defense = {-1, -1};
    CellState opponent_color = (opponent == PLAYER_BLACK) ? BLACK : WHITE;
    
    // 第一优先级：查找活三威胁，找到堵住它们的位置
    // 活三 _XXX_ 是最致命的威胁，必须立即堵住
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state->board[i][j] != opponent_color) {
                continue;
            }
            
            // 检查4个方向的活三
            for (int dir = 0; dir < 4; dir++) {
                if (!check_open_three(state, i, j, dir)) {
                    continue;
                }
                
                // 找到活三了，现在找堵住它的位置
                int dx = direction[dir][0];
                int dy = direction[dir][1];
                
                // 对于活三 _XXX_，需要在两端都放置权重
                // 但先返回更容易完成的防守点
                Position defense_pos = {-1, -1};
                
                // 向正方向查找最近的空位
                for (int k = 1; k <= 5; k++) {
                    int x = j + k * dx;
                    int y = i + k * dy;
                    
                    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                        break;
                    }
                    if (state->board[y][x] == EMPTY) {
                        defense_pos.row = y;
                        defense_pos.col = x;
                        break;
                    } else if (state->board[y][x] != opponent_color) {
                        break;
                    }
                }
                
                // 如果找到防守位置，立即返回
                if (defense_pos.row != -1 && defense_pos.col != -1) {
                    return defense_pos;
                }
                
                // 向反方向查找空位
                for (int k = 1; k <= 5; k++) {
                    int x = j - k * dx;
                    int y = i - k * dy;
                    
                    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                        break;
                    }
                    if (state->board[y][x] == EMPTY) {
                        return (Position){y, x};
                    } else if (state->board[y][x] != opponent_color) {
                        break;
                    }
                }
            }
        }
    }
    
    // 第二优先级：查找跳四(XX_XX)威胁，找到堵住它们的位置
    // 对于XX_XX型的跳四，应该优先堵住中间的空位
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state->board[i][j] != opponent_color) {
                continue;
            }
            
            // 检查4个方向的跳四
            for (int dir = 0; dir < 4; dir++) {
                if (!check_jump_four(state, i, j, dir)) {
                    continue;
                }
                
                // 找到跳四了，现在找堵住它的空位
                // 对于XX_XX形式，中间的空位是最关键的
                int dx = direction[dir][0];
                int dy = direction[dir][1];
                
                // 首先尝试找到这个跳四的中间空位
                Position middle_defense = {-1, -1};
                
                // 从当前位置向正方向扫描，找中间的空位
                for (int k = 1; k <= 5; k++) {
                    int x = j + k * dx;
                    int y = i + k * dy;
                    
                    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                        break;
                    }
                    
                    if (state->board[y][x] == EMPTY && middle_defense.row == -1) {
                        // 检查这个空位前后是否都有对方棋子
                        int next_x = x + dx;
                        int next_y = y + dy;
                        if (next_x >= 0 && next_x < BOARD_SIZE && next_y >= 0 && next_y < BOARD_SIZE) {
                            if (state->board[next_y][next_x] == opponent_color) {
                                middle_defense.row = y;
                                middle_defense.col = x;
                                return middle_defense;  // 立即返回中间的空位
                            }
                        }
                    } else if (state->board[y][x] != opponent_color) {
                        break;
                    }
                }
                
                // 如果没找到中间空位，找其他空位
                for (int k = 1; k <= 5; k++) {
                    int x = j + k * dx;
                    int y = i + k * dy;
                    
                    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                        break;
                    }
                    if (state->board[y][x] == EMPTY) {
                        return (Position){y, x};
                    } else if (state->board[y][x] != opponent_color) {
                        break;
                    }
                }
                
                // 向反方向查找空位
                for (int k = 1; k <= 5; k++) {
                    int x = j - k * dx;
                    int y = i - k * dy;
                    
                    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                        break;
                    }
                    if (state->board[y][x] == EMPTY) {
                        return (Position){y, x};
                    } else if (state->board[y][x] != opponent_color) {
                        break;
                    }
                }
            }
        }
    }
    
    // 第三优先级：查找冲四威胁，找到堵住它们的位置
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state->board[i][j] != opponent_color) {
                continue;
            }
            
            // 检查4个方向的冲四
            for (int dir = 0; dir < 4; dir++) {
                if (!check_four(state, i, j, dir)) {
                    continue;
                }
                
                // 找到冲四了，现在找堵住它的位置
                int dx = direction[dir][0];
                int dy = direction[dir][1];
                
                // 向正方向查找空位堵住
                for (int k = 1; k <= 5; k++) {
                    int x = j + k * dx;
                    int y = i + k * dy;
                    
                    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                        break;
                    }
                    if (state->board[y][x] == EMPTY) {
                        return (Position){y, x};
                    } else if (state->board[y][x] != opponent_color) {
                        break;
                    }
                }
                
                // 向反方向查找空位堵住
                for (int k = 1; k <= 5; k++) {
                    int x = j - k * dx;
                    int y = i - k * dy;
                    
                    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                        break;
                    }
                    if (state->board[y][x] == EMPTY) {
                        return (Position){y, x};
                    } else if (state->board[y][x] != opponent_color) {
                        break;
                    }
                }
            }
        }
    }
    
    return no_defense;
}