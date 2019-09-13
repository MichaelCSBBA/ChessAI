#include <cmath>
#include <memory>
#include "enums.h"
#include "move.h"
#include "message.h"
#include "piece.h"
#include "knight.h"

using namespace std;

Knight::Knight(Colour colour, bool hasMoved, int lastTwoSquareTurn) : 
    Piece(PieceType::Knight, colour, hasMoved, lastTwoSquareTurn) {}

bool Knight::isPotentialMove(Move move) const {
    int rowDiff = abs(move.startRow - move.endRow);
    int colDiff = abs(move.startCol - move.endCol);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

char Knight::getChar() const { 
    return colour == Colour::White ? 'N' : 'n'; 
}

std::unique_ptr<Piece> Knight::clone() const {
    return make_unique<Knight>(colour, hasMoved, lastTwoSquareTurn);
}

Status Knight::getIntermediateStatus(const Message &message, const Piece &other) const {
    return Status::Error; 
}

Status Knight::getEndpointStatus(const Message &message, const Piece &other) const {
    if (message.type == MessageType::PieceMove) {
        return (other.type == PieceType::NoneType || other.colour != colour) ?
            Status::Success : Status::Error;
    }
    return Status::Error;
}
