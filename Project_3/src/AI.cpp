#include "AI.h"
#include <limits>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>


/**
 * declaration of @var INF to correctly assign values that were not yet found in miniMax algorithm eg. alfa and beta values
 */
static const int INF = std::numeric_limits<int>::max() / 2;

/**
 * private heurestic method of @class AI used for calculating the score of line
 * @param aiCount represents the number of AI symbols in line
 * @param playerCount represents the number of player symbols in line
 * for each symbol in line the score is multiplied by 10, and then returned
 * for AI we return the @var score, and for player we return @var -score, because the AI is trying to minimize the maximal gain of player
 * if there are both symbols of player and AI, then this line is blocked and shouldnt be calculated, ie. win in that line is not possible
 * @returns score of line, where positive values are good for AI and negative values are good for player
 */
int AI::scoreLine(int aiCount, int playerCount) {
    if (aiCount > 0 && playerCount > 0) return 0;  // blocked line
    if (aiCount > 0) {
        int score{1};
        for (int i{0}; i < aiCount; ++i) { score *= 10; }
        return score;
    }
    if (playerCount > 0) {
        int score{1};
        for (int i{0}; i < playerCount; ++i) { score *= 10; }
        return -score;
    }
    return 0;
}

/**
 * private heurestic method of @class AI for evaluation of board
 * @param board represents the current state of game
 * at first, we calculate the score of each row and column
 * then we calculate the score of two diagonals
 * the total score is returned at the end
 * @returns total score of board, where positive values are good for AI and negative values are good for player
 */
int AI::evaluate(const Board& board) {
    int n     = board.getSize();
    int total{0};

    /** 
     * helper lambda function to calculate score of line
     * counts the number of symbols in line, and calls @fn scoreLine() to calculate score
     */
    auto addLine = [&](const std::vector<std::pair<int,int>>& coords) {
        int ai{0}, player{0};
        for (auto& [r, c] : coords) {
            Cell cell = board.getCell(r, c);
            if (cell == Cell::O) { ++ai; }
            else if (cell == Cell::X) { ++player; }
        }
        total += scoreLine(ai, player);
    };

    /** calculating rows and columns */
    for (int r{0}; r < n; ++r) {
        std::vector<std::pair<int,int>> row, col;
        for (int i{0}; i < n; ++i) { row.push_back({r,i}); col.push_back({i,r}); }
        addLine(row);
        addLine(col);
    }
    /** calculating diagonals */
    std::vector<std::pair<int,int>> diag1, diag2;
    for (int i{0}; i < n; ++i) { diag1.push_back({i, i}); diag2.push_back({i, n - 1 - i}); }
    addLine(diag1);
    addLine(diag2);

    return total;
}

/**
 * private method of @class AI implementing minimax algorithm with alpha-beta pruning
 * @param board represents current state of game
 * @param depth represents depth of search
 * @param maxDepth represents maximum depth of search, determined by board size; deeper search is more accurate but also more computationally expensive
 * @param alpha represents the best score that the maximizer currently can guarantee at that level or above
 * @param beta represents the best score that the minimizer currently can guarantee at that level or above
 * @param maximising represents whether we are currently maximizing or minimizing player
 * the function returns the best score that the current player can achieve from the given board state, assuming that the opponent also plays optimally
 */
int AI::minimax(Board& board, int depth, int maxDepth, int alpha, int beta, bool maximising, long long& nodesVisited) {
    ++nodesVisited;
    GameState state = board.checkGameState();

    // Terminal: win/loss scored relative to remaining depth (prefer faster wins)
    if (state == GameState::OWins) { return  10000 - depth; }
    if (state == GameState::XWins) { return -10000 + depth; }
    if (state == GameState::Draw)  { return 0; }

    // Depth limit: use heuristic
    if (depth >= maxDepth) { return evaluate(board); }

    auto moves = board.getLegalMoves();

    if (maximising) {
        int best = -INF;
        for (auto& m : moves) {
            board.makeMove(m.row, m.col, Cell::O);
            int score = minimax(board, depth + 1, maxDepth, alpha, beta, false, nodesVisited);
            board.undoMove(m.row, m.col);
            best  = std::max(best, score);
            alpha = std::max(alpha, best);
            if (beta <= alpha) { break; }   // β cut-off
        }
        return best;
    } else {
        int best = INF;
        for (auto& m : moves) {
            board.makeMove(m.row, m.col, Cell::X);
            int score = minimax(board, depth + 1, maxDepth, alpha, beta, true, nodesVisited);
            board.undoMove(m.row, m.col);
            best = std::min(best, score);
            beta = std::min(beta, best);
            if (beta <= alpha) { break; }   // α cut-off
        }
        return best;
    }
}

/**
 * public method of @class AI to get the best move for current board state
 * iterates through all legal moves, simulates the move, and calls @fn minimax to evaluate the resulting board state
 * the move with the highest score is returned at the end
 */
Move AI::getBestMove(Board& board, int maxDepth) {
    int       bestScore    = -INF;
    Move      bestMove{-1, -1};
    long long nodesVisited{0};
 
    /** begin timer for time measurement of minmax algorithm */
    auto startTime = std::chrono::high_resolution_clock::now();
 
    for (auto& m : board.getLegalMoves()) {
        board.makeMove(m.row, m.col, Cell::O);
        int score = minimax(board, 1, maxDepth, -INF, INF, false, nodesVisited);
        board.undoMove(m.row, m.col);
        if (score > bestScore) { bestScore = score; bestMove  = m; }
    }
 
    /** end timer and calculate time in ms */
    auto    endTime  = std::chrono::high_resolution_clock::now();
    auto    elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    double  elapsedMs = elapsedUs / 1000.0;
 
    // nodes per second (avoid division by zero for instant moves)
    double  nodesPerSec = (elapsedUs > 0) ? (static_cast<double>(nodesVisited) / elapsedUs) * 1'000'000.0 : 0.0;
 
    /** print AI move statistics */
    std::cout << "┌─ AI move statistics ────────────────────┐\n";
    std::cout << "│  Best move   : ["
              << bestMove.row << ", " << bestMove.col << "]\n";
    std::cout << "│  Search time : "
              << std::fixed << std::setprecision(3) << elapsedMs << " ms\n";
    std::cout << "│  Nodes visited: "
              << nodesVisited << "\n";
    std::cout << "│  Throughput  : "
              << std::fixed << std::setprecision(0) << nodesPerSec << " nodes/s\n";
    std::cout << "└─────────────────────────────────────────┘\n";
 
    return bestMove;
}