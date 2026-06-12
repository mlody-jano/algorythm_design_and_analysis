#include "Game.h"

/**
 * private static method of @class Game to determine the depth of minimax search based on board size
 * @param n size of board
 * @returns maximum depth of minimax search for given board size
 * * in general, deeper search is more accurate, but also more computationally expensive
 * the maximum number of moves in a game is n^2, so the increase in computation cost is quadratic with size of board
 * DISCLAIMER: these values were determined as a say-so, based on intuition; they can be changed to either increase accuracy (AI plays better, but take more time to make a move) or decrease it (AI plays worse, but is faster)
 */
int Game::defaultDepthForSize(int n) {
    if (n <= 3) { return 9; }   // full search for 3×3
    if (n == 4) { return 5; }
    if (n == 5) { return 4; }
    return 3;               // 6×6 and larger: shallow search
}

/**
 * parametrized constructor of @class Game
 * initializes the board with given size, sets the first turn to player and determines the maximum depth of minimax search
 */
Game::Game(int boardSize, int maxDepth) : board_(boardSize), turn_(Turn::Player), maxDepth_(maxDepth_ < 1 ? defaultDepthForSize(boardSize) : maxDepth) {}

/**
 * public method of @class Game to reset the game state
 * @param boardSize size of board to reset to
 * reinitializes the board with given size
 */
void Game::reset(int boardSize, int maxDepth) {
    board_    = Board(boardSize);
    turn_     = Turn::Player;
    maxDepth_ = (maxDepth_ < 1 ? defaultDepthForSize(boardSize) : maxDepth);
}

/**
 * public method of @class Game to make a move for player
 * @param row target row of move
 * @param col target column of move
 * @returns true if move was succesfull, otherwise false
 */
bool Game::playerMove(int row, int col) {
    if (turn_ != Turn::Player)                          { return false; }
    if (board_.checkGameState() != GameState::Playing)  { return false; }
    if (board_.getCell(row, col) != Cell::Empty)        { return false; }

    board_.makeMove(row, col, Cell::X);
    turn_ = Turn::AI;
    return true;
}

/**
 * public method of @class Game to make a move for AI
 * @returns @class Move object representing the move made by AI
 */
Move Game::aiMove() {
    Move m = AI::getBestMove(board_, maxDepth_);
    if (m.row != -1) { board_.makeMove(m.row, m.col, Cell::O); }
    turn_ = Turn::Player;
    return m;
}