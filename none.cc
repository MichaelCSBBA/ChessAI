#include <memory>
#include "enums.h"
#include "move.h"
#include "message.h"
#include "piece.h"
#include "none.h"

using namespace std;

None::None(bool hasMoved, int lastTwoSquareTurn) : 
    Piece(PieceType::NoneType, Colour::NoColour, hasMoved, lastTwoSquareTurn) {}

bool None::isPotentialMove(Move move) const {
    return false;
}

char None::getChar() const { 
    return ' ';
}

std::unique_ptr<Piece> None::clone() const {
    return make_unique<None>(hasMoved, lastTwoSquareTurn);
}

Status None::getIntermediateStatus(const Message &message, const Piece &other) const {
    return Status::Error; 
}

Status None::getEndpointStatus(const Message &message, const Piece &other) const {
    return Status::Error;
}
