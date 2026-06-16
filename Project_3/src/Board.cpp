#include "../include/Board.h"

/**
 * parametrized constructor of @class Board
 * initializes the grid with given size and fills it with empty cells
 */
Board::Board(int size) : size_(size), grid_(size, std::vector<Cell>(size, Cell::Empty)) {}

/**
 * public method of @class Board to make a move
 * @param row target row of move
 * @param col target column of move
 * @param player represents which player is taking up the spot (X or O)
 * sets the cell at given coordinates to the symbol of player
 */
void Board::makeMove(int row, int col, Cell player) { grid_[row][col] = player; }

/**
 * public method of @class Board to undo a move
 * @param row target row of move
 * @param col target column of move
 * sets the cell at given coordinates back to empty
 */
void Board::undoMove(int row, int col) { grid_[row][col] = Cell::Empty; }

/**
 * public method of @class Board to get the state of cell at given coordinates
 * @param row target row of cell
 * @param col target column of cell
 * @return Cell at given coordinates (Empty, X or O)
 */
Cell Board::getCell(int row, int col) const { return grid_[row][col]; }

/**
 * public method of @class Board to check if the board is full
 * @returns true if no cells are empty, otherwise false
 */
bool Board::isFull() const {
    for (auto& row : grid_)
        for (auto& cell : row)
            if (cell == Cell::Empty) return false;
    return true;
}

/**
 * private method of @class Board to check if a line (row, column or diagonal) is occupied by the same player's symbol
 * @param coords vector of pairs of coordinates representing the line to check
 * @returns symbol of player occupying the line, or std::nullopt if the line is not occupied by a single player
 */
std::optional<Cell> Board::checkLine(const std::vector<std::pair<int,int>>& coords) const {
    Cell first = grid_[coords[0].first][coords[0].second];
    if (first == Cell::Empty) { return std::nullopt; }
    for (auto& [r, c] : coords)
        if (grid_[r][c] != first) { return std::nullopt; }
    return first;
}

/**
 * private method of @class Board to check if there is a winner in current board state
 * @returns symbol of player who won, or std::nullopt if there is no winner
 * firstly, we check all rows for winning lane
 * then we check all columns
 * finally, we check two diagonals
 * if any of these lines was not a winning line, we return std::nullopt
 */
std::optional<Cell> Board::getWinner() const {

    /** first we check all rows */
    for (int r{0}; r < size_; ++r) {
        std::vector<std::pair<int,int>> coords;
        for (int c{0}; c < size_; ++c) { coords.push_back({r, c}); }
        if (auto w = checkLine(coords)) { return w; }
    }
    /** then we check all columns */
    for (int c{0}; c < size_; ++c) {
        std::vector<std::pair<int,int>> coords;
        for (int r{0}; r < size_; ++r) { coords.push_back({r, c}); }
        if (auto w = checkLine(coords)) { return w; }
    }
    /** then the first diagonal */
    {
        std::vector<std::pair<int,int>> coords;
        for (int i{0}; i < size_; ++i) { coords.push_back({i, i}); }
        if (auto w = checkLine(coords)) { return w; }
    }
    /** lastly the second diagonal */
    {
        std::vector<std::pair<int,int>> coords;
        for (int i{0}; i < size_; ++i) { coords.push_back({i, size_ - 1 - i}); }
        if (auto w = checkLine(coords)) { return w; }
    }
    return std::nullopt;
}

/**
 * public method of @class Board to check the state of game
 * @returns @class GameState object representing the state of game (Playing, XWins, OWins or Draw)
 */
GameState Board::checkGameState() const {
    if (auto w = getWinner()) { return (*w == Cell::X) ? GameState::XWins : GameState::OWins; }
    if (isFull()) { return GameState::Draw; }
    return GameState::Playing;
}

/**
 * public method of @class Board to get all legal moves in current board state
 * @returns vector of @class Move objects representing all empty cells on the board
 * iterates through all cells on the board, and if finds an empty cell, adds it coordinates to the result vector
 */
std::vector<Move> Board::getLegalMoves() const {
    std::vector<Move> moves;
    for (int r{0}; r < size_; ++r)
        for (int c{0}; c < size_; ++c)
            if (grid_[r][c] == Cell::Empty) { moves.push_back({r, c}); }
    return moves;
}
