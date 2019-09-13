#ifndef PLAYER_H
#define PLAYER_H
#include <iostream>
#include <memory>
#include "enums.h"
#include "move.h"
#include "gamestate.h"

class Player {
    public:
        virtual Move getMove(GameState &g, std::istream &in, Colour colour, int turn);
        virtual ~Player();
};
#endif
