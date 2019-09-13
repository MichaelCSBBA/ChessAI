#ifndef COMPUTER2_H
#define COMPUTER2_H
#include <iostream>
#include <memory>
#include "enums.h"
#include "move.h"
#include "gamestate.h"
#include "player.h"

class Computer2 : public Player {
    int getNumOppositeColourPieces(GameState &g, Colour colour) const;
    public:
        Move getMove(GameState &g, std::istream &in, Colour colour, int turn) override;
};
#endif
