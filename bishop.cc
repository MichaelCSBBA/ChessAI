#include <cmath>
#include <memory>
#include "enums.h"
#include "move.h"
#include "message.h"
#include "piece.h"
#include "bishop.h"

using namespace std;

Bishop::Bishop(Colour colour, bool hasMoved, int lastTwoSquareTurn) : 
    Piece(PieceType::Bishop, colour, hasMoved, lastTwoSquareTurn) {}

bool Bishop::isPotentialMove(Move move) const {
    return abs(move.startRow - move.endRow) == abs(move.startCol - move.endCol);
}

char Bishop::getChar() const { 
    return colour == Colour::White ? 'B' : 'b'; 
}

std::unique_ptr<Piece> Bishop::clone() const {
    return make_unique<Bishop>(colour, hasMoved, lastTwoSquareTurn);
}

Status Bishop::getIntermediateStatus(const Message &message, const Piece &other) const {
    if (message.type == MessageType::PieceMove) {
        return (other.type == PieceType::NoneType) ? Status::Continue : Status::Error;
    }
}

Status Bishop::getEndpointStatus(const Message &message, const Piece &other) const {
    if (message.type == MessageType::PieceMove) {
        return (other.type == PieceType::NoneType || other.colour != colour) ?
            Status::Success : Status::Error;
    }
    return Status::Error;
}
