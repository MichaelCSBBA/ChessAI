#include <iostream>
#include "move.h"
#include "enums.h"

using namespace std;

bool Move::operator<(const Move &other) const {
    return false;
}

ostream &operator<<(ostream &out, const Move &m) {
    out << "Turn " << m.turn;
    if (m.player == Colour::White) {
        out << " (white): ";
    } else {
        out << " (black): ";
    }
    out << (char)('a' + m.startCol) << (m.startRow + 1) << ' ' 
        << (char)('a' + m.endCol) << (m.endRow + 1);
    switch (m.promoteTo) {
        case PieceType::Queen: out << " Q"; break;
        case PieceType::Rook: out << " R"; break;
        case PieceType::Bishop: out << " B"; break;
        case PieceType::Knight: out << " N"; break;
    }
    out << endl;
    return out;
}
