#include <iostream>
#include <cmath>
#include <cstdint>

const int BOARD_ROW = 6;
const int BOARD_COL = 7;
const int INAROW = 4;
const int BOARD_SIZE = BOARD_ROW * BOARD_COL;

class ConnectFourAgent
{
private:
    int AI_PIECE;
    int HUMAN_PIECE;

    // Evaluation board weights
    const int EVALUATING_BOARD[BOARD_ROW][BOARD_COL] = {{3, 4, 5, 7, 5, 4, 3},
                                                        {4, 6, 8, 10, 8, 6, 4},
                                                        {5, 7, 11, 13, 11, 7, 5},
                                                        {5, 7, 11, 13, 11, 7, 5},
                                                        {4, 6, 8, 10, 8, 6, 4},
                                                        {3, 4, 5, 7, 5, 4, 3}};

    // Winning patterns
    const uint64_t GAME_OVER[69] = {15, 2113665, 16843009, 2130440, 30, 4227330, 33686018, 4260880, 60, 8454660, 67372036, 8521760, 120, 16909320, 134744072, 17043520, 33818640, 67637280, 135274560, 1920, 270549120, 2155905152, 272696320, 3840, 541098240, 4311810304, 545392640, 7680, 1082196480, 8623620608, 1090785280, 15360, 2164392960, 17247241216, 2181570560, 4328785920, 8657571840, 17315143680, 245760, 34630287360, 275955859456, 34905128960, 491520, 69260574720, 551911718912, 69810257920, 983040, 138521149440, 1103823437824, 139620515840, 1966080, 277042298880, 2207646875648, 279241031680, 554084597760, 1108169195520, 2216338391040, 31457280, 62914560, 125829120, 251658240, 4026531840, 8053063680, 16106127360, 32212254720, 515396075520, 1030792151040, 2061584302080, 4123168604160};

    const uint64_t IS_LEGAL_MOVE_MASK = (1 << BOARD_COL) - 1;
    void list_to_bitboard(const int listboard[BOARD_SIZE], uint64_t bitboard[2])
    {
        bitboard[0] = 0; // played
        bitboard[1] = 0; // player
        for (int n = 0; n < BOARD_SIZE; n++)
        {
            if (listboard[n] != 0)
            {
                bitboard[0] |= (1ULL << n);
                if (listboard[n] == AI_PIECE)
                {
                    bitboard[1] |= (1ULL << n);
                }
            }
        }
    }

    void bitboard_to_numpy2d(const uint64_t bitboard[2], int8_t output[BOARD_ROW][BOARD_COL])
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            int row = i / BOARD_COL;
            int col = i % BOARD_COL;
            output[row][col] = 0;
            bool is_played = (bitboard[0] >> i) & 1;
            if (is_played)
            {
                bool player = (bitboard[1] >> i) & 1;
                output[row][col] = player ? AI_PIECE : HUMAN_PIECE;
            }
        }
    }

    bool isRunOutMove(const uint64_t bitboard[2])
    {
        uint64_t mask_board = (1ULL << (BOARD_COL * BOARD_ROW)) - 1;
        return bitboard[0] == mask_board;
    }

    bool isWinning(const uint64_t bitboard[2], int piece)
    {
        for (int i = 0; i < 69; i++)
        {
            if (piece == AI_PIECE)
            {
                if ((bitboard[0] & bitboard[1] & GAME_OVER[i]) == GAME_OVER[i])
                    return true;
            }
            else
            {
                if ((bitboard[0] & ~bitboard[1] & GAME_OVER[i]) == GAME_OVER[i])
                    return true;
            }
        }
        return false;
    }

    int evaluatingFunction(const uint64_t bitboard[2]) {
        int8_t board[BOARD_ROW][BOARD_COL];
        bitboard_to_numpy2d(bitboard, board);
        
        // Check for immediate wins/losses first
        if (isWinning(bitboard, AI_PIECE)) {
            return 1000000; // Very high score for win
        }
        if (isWinning(bitboard, HUMAN_PIECE)) {
            return -1000000; // Very low score for loss
        }
        
        int score = 0;
        
        // 1. Evaluate threat-based scoring
        score += evaluateThreats(board);
        
        // 2. Add center control bonus
        score += evaluateCenter(board);
        
        // 3. Add positional scoring (from your original implementation)
        score += evaluatePosition(board);
        
        return score;
    }
    
    int evaluateThreats(const int8_t board[BOARD_ROW][BOARD_COL]) {
        int score = 0;
        
        // Check horizontal, vertical, and two diagonal directions
        const int dx[4] = {1, 0, 1, 1};
        const int dy[4] = {0, 1, 1, -1};
        
        for (int y = 0; y < BOARD_ROW; y++) {
            for (int x = 0; x < BOARD_COL; x++) {
                for (int dir = 0; dir < 4; dir++) {
                    score += evaluateLine(board, x, y, dx[dir], dy[dir], AI_PIECE);
                    score -= evaluateLine(board, x, y, dx[dir], dy[dir], HUMAN_PIECE);
                }
            }
        }
        
        return score;
    }
    
    int evaluateLine(const int8_t board[BOARD_ROW][BOARD_COL], int x, int y, int dx, int dy, int piece) {
        int opponent = (piece == AI_PIECE) ? HUMAN_PIECE : AI_PIECE;
        
        // Check if this starting position can still form a connect-4
        int count = 0;
        int empty = 0;
        int emptyPos[INAROW];  // Store positions of empty slots
        bool valid = true;
        
        for (int i = 0; i < INAROW; i++) {
            int nx = x + i * dx;
            int ny = y + i * dy;
            
            if (nx < 0 || nx >= BOARD_COL || ny < 0 || ny >= BOARD_ROW) {
                valid = false;
                break;
            }
            
            if (board[ny][nx] == piece) {
                count++;
            } else if (board[ny][nx] == opponent) {
                valid = false;
                break;
            } else {
                // Check if this empty spot is playable (has support or is bottom row)
                if (ny == BOARD_ROW - 1 || board[ny + 1][nx] != 0) {
                    emptyPos[empty++] = i;
                } else {
                    // Not immediately playable
                    valid = false;
                    break;
                }
            }
        }
        
        if (!valid) {
            return 0;
        }
        
        // Score based on number of pieces and empty playable spots
        if (count == 3 && empty == 1) {
            return 1000;  // Immediate winning threat
        } else if (count == 2 && empty == 2) {
            return 100;   // Two pieces with two playable spots
        } else if (count == 1 && empty == 3) {
            return 10;    // One piece with three playable spots
        }
        
        return 1;  // Valid but not threatening line
    }
    
    int evaluateCenter(const int8_t board[BOARD_ROW][BOARD_COL]) {
        int score = 0;
        int center_col = BOARD_COL / 2;
        
        // Prioritize control of the center column
        for (int row = 0; row < BOARD_ROW; row++) {
            if (board[row][center_col] == AI_PIECE) {
                score += 30;
            } else if (board[row][center_col] == HUMAN_PIECE) {
                score -= 30;
            }
        }
        
        // Adjacent columns are also valuable but less so
        for (int row = 0; row < BOARD_ROW; row++) {
            if (center_col > 0) {
                if (board[row][center_col-1] == AI_PIECE) {
                    score += 20;
                } else if (board[row][center_col-1] == HUMAN_PIECE) {
                    score -= 20;
                }
            }
            
            if (center_col < BOARD_COL - 1) {
                if (board[row][center_col+1] == AI_PIECE) {
                    score += 20;
                } else if (board[row][center_col+1] == HUMAN_PIECE) {
                    score -= 20;
                }
            }
        }
        
        return score;
    }
    
    int evaluatePosition(const int8_t board[BOARD_ROW][BOARD_COL]) {
        int score = 0;
        
        // Prefer lower positions as they give more options
        for (int col = 0; col < BOARD_COL; col++) {
            for (int row = 0; row < BOARD_ROW; row++) {
                int position_value = EVALUATING_BOARD[row][col];
                
                if (board[row][col] == AI_PIECE) {
                    score += position_value;
                } else if (board[row][col] == HUMAN_PIECE) {
                    score -= position_value;
                }
            }
        }
        
        return score;
    }
    bool is_legal_move(const uint64_t bitboard[2], int action)
    {
        uint64_t bits = bitboard[0] & IS_LEGAL_MOVE_MASK;
        return ((bits >> action) & 1) == 0;
    }

    void getValidPositions(const uint64_t bitboard[2], int validPositions[BOARD_COL], int &count)
    {
        count = 0;
        const int exploration_order[7] = {3, 2, 4, 1, 5, 0, 6};
        for (int i = 0; i < BOARD_COL; i++)
        {
            int col = exploration_order[i];
            if (is_legal_move(bitboard, col))
            {
                validPositions[count++] = col;
            }
        }
    }

    int get_next_index(const uint64_t bitboard[2], int action)
    {
        for (int row = BOARD_ROW - 1; row >= 0; row--)
        {
            int index = action + (row * BOARD_COL);
            if (((bitboard[0] >> index) & 1) == 0)
            {
                return index;
            }
        }
        return action;
    }

    void dropPiece(const uint64_t bitboard[2], int col, int piece, uint64_t output[2])
    {
        int index = get_next_index(bitboard, col);
        uint64_t mark = (piece == HUMAN_PIECE) ? 0 : 1;
        output[0] = bitboard[0] | (1ULL << index);
        output[1] = bitboard[1] | (mark << index);
    }

    void minimax(const uint64_t bitboard[2], int depth, double alpha, double beta,
                 bool maximizingPlayer, int &bestMove, double &bestScore)
    {
        if (depth == 0)
        {
            bestMove = 0;
            bestScore = evaluatingFunction(bitboard);
            return;
        }
        if (isWinning(bitboard, AI_PIECE))
        {
            bestMove = 0;
            bestScore = INFINITY;
            return;
        }
        if (isWinning(bitboard, HUMAN_PIECE))
        {
            bestMove = 0;
            bestScore = -INFINITY;
            return;
        }
        if (isRunOutMove(bitboard))
        {
            bestMove = 0;
            bestScore = 0;
            return;
        }

        int validPositions[BOARD_COL];
        int count;
        getValidPositions(bitboard, validPositions, count);

        if (maximizingPlayer)
        {
            double maxEval = -INFINITY;
            bestMove = validPositions[0];
            for (int i = 0; i < count; i++)
            {
                int col = validPositions[i];
                uint64_t new_bitboard[2];
                dropPiece(bitboard, col, AI_PIECE, new_bitboard);
                int nextMove;
                double eval;
                minimax(new_bitboard, depth - 1, alpha, beta, false, nextMove, eval);
                if (eval > maxEval)
                {
                    maxEval = eval;
                    bestMove = col;
                }
                alpha = (alpha > maxEval) ? alpha : maxEval;
                if (beta <= alpha)
                    break;
            }
            bestScore = maxEval;
        }
        else
        {
            double minEval = INFINITY;
            bestMove = validPositions[0];
            for (int i = 0; i < count; i++)
            {
                int col = validPositions[i];
                uint64_t new_bitboard[2];
                dropPiece(bitboard, col, HUMAN_PIECE, new_bitboard);
                int nextMove;
                double eval;
                minimax(new_bitboard, depth - 1, alpha, beta, true, nextMove, eval);
                if (eval < minEval)
                {
                    minEval = eval;
                    bestMove = col;
                }
                beta = (beta < minEval) ? beta : minEval;
                if (beta <= alpha)
                    break;
            }
            bestScore = minEval;
        }
    }

public:
    ConnectFourAgent(int mark)
    {
        AI_PIECE = mark;
        HUMAN_PIECE = (AI_PIECE == 1) ? 2 : 1;
    }

    int my_agent(const int board[BOARD_SIZE])
    {
        uint64_t bitboard[2];
        list_to_bitboard(board, bitboard);
        int bestMove;
        double bestScore;
        minimax(bitboard, 11, -INFINITY, INFINITY, true, bestMove, bestScore);
        return bestMove;
    }
};

extern "C"
{
    int call_connect_four_agent(int *board, int size, int mark)
    {
        if (size != BOARD_SIZE)
        {
            return -1; // Error: invalid board size
        }
        ConnectFourAgent agent(mark);
        return agent.my_agent(board);
    }
}
