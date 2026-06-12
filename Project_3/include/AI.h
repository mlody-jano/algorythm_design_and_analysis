#pragma once
#include "Board.h"

/**
 * @class AI
 * represents the opposing player ie. the computer
 */
class AI {
public:
    static Move getBestMove(Board&, int);

private:
    static int  evaluate(const Board&);
    static int  scoreLine(int, int);

    static int  minimax(Board&, int, int, int, int, bool);
};
