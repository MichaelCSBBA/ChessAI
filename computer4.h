#ifndef COMPUTER4_H
#define COMPUTER4_H
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "enums.h"
#include "move.h"
#include "gamestate.h"
#include "player.h"

class Computer4 : public Player {
    const int minDepth;
    const int maxDepth;
    std::vector<Move> killerMoves; 
    int turn = -1;
    float evaluateBoard(GameState &g, Colour colour) const;
    void updateAlphaBeta(float returnValue, Move move, float &alpha, float &beta, 
        float &value, Move &bestMove, bool isMaximizing) const;
    std::pair<Move, float> alphaBetaSearch(GameState &g, int depth, float alpha, 
        float beta, Colour colour, bool isMaximizing, bool isForcing=false);
    public:
        Computer4(int minDepth=4, int maxDepth=8);
        Move getMove(GameState &g, std::istream &in, Colour colour, int turn) override;
};
#endif
