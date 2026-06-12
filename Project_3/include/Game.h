#pragma once
#include "Board.h"
#include "AI.h"

/**
 * @enum @class Turn
 * representing which players turn it is now
 */
enum class Turn { Player, AI };

/**
 * @class Game
 * represents the logic of game, switching between turns etc.
 */
class Game {
public:
    explicit                Game(int);

    bool                    playerMove(int, int);
    Move                    aiMove();
    Turn                    getTurn()               const { return turn_; }
    GameState               getGameState()          const { return board_.checkGameState(); }
    const Board&            getBoard()              const { return board_; }
    int                     getBoardSize()          const { return board_.getSize(); }
    void                    reset(int);

private:
    Board board_;
    Turn  turn_;
    int   maxDepth_;

    static int              depthForSize(int);
};
