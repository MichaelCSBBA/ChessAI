#ifndef MOVE_H
#define MOVE_H
#include <iostream>
#include "enums.h"

struct Move {
    Colour player;
    int startRow, startCol, endRow, endCol, turn;
    PieceType promoteTo;
    bool operator<(const Move &other) const;
};

std::ostream &operator<<(std::ostream &out, const Move &m);
#endif
