#ifndef COMPUTER3_H
#define COMPUTER3_H
#include <iostream>
#include <memory>
#include "enums.h"
#include "move.h"
#include "gamestate.h"
#include "player.h"

class Computer3 : public Player {
    public:
        Move getMove(GameState &g, std::istream &in, Colour colour, int turn) override;
};
#endif
