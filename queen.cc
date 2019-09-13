#include <cmath>
#include <memory>
#include "enums.h"
#include "move.h"
#include "message.h"
#include "piece.h"
#include "queen.h"

using namespace std;

Queen::Queen(Colour colour, bool hasMoved, int lastTwoSquareTurn) :
    Piece(PieceType::Queen, colour, hasMoved, lastTwoSquareTurn) {}

bool Queen::isPotentialMove(Move move) const {
    if (abs(move.startRow - move.endRow) == abs(move.startCol - move.endCol)) {
        return true;
    }
    return (move.startRow == move.endRow) || (move.startCol == move.endCol);
}

char Queen::getChar() const { 
    return colour == Colour::White ? 'Q' : 'q'; 
}

std::unique_ptr<Piece> Queen::clone() const {
    return make_unique<Queen>(colour, hasMoved, lastTwoSquareTurn);
}

Status Queen::getIntermediateStatus(const Message &message, const Piece &other) const {
    if (message.type == MessageType::PieceMove) {
        return (other.type == PieceType::NoneType) ? Status::Continue : Status::Error;
    }
}

Status Queen::getEndpointStatus(const Message &message, const Piece &other) const {
    if (message.type == MessageType::PieceMove) {
        return (other.type == PieceType::NoneType || other.colour != colour) ?
            Status::Success : Status::Error;
    }
    return Status::Error;
}
