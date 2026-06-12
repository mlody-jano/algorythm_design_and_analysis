#pragma once
#include <vector>
#include <optional>

/**
 * @enum @class Cell
 * represents the state of cell
 */
enum class Cell { Empty, X, O };

/**
 * @enum @class GameState
 * represents the state of game
 */
enum class GameState { Playing, XWins, OWins, Draw };

/**
 * @struct Move
 * represents a valid player turn
 */
struct Move {
    int row, col;
};

/**
 * @class Board
 * represents the main structure in which game is being played
 * uses parametrized constructor to pass along the size of board
 */
class Board {
public:
    explicit            Board(int);

    void                makeMove(int, int, Cell);
    void                undoMove(int, int);
    Cell                getCell(int, int)                                           const;
    int                 getSize()                                                   const { return size_; }
    bool                isFull()                                                    const;
    GameState           checkGameState()                                            const;
    std::vector<Move>   getLegalMoves()                                             const;

private:
    int                             size_;
    std::vector<std::vector<Cell>>  grid_;

    std::optional<Cell> checkLine(const std::vector<std::pair<int,int>>&)           const;
    std::optional<Cell> getWinner()                                                 const;
};
