#include <cmath>
#include <memory>
#include "enums.h"
#include "move.h"
#include "message.h"
#include "piece.h"
#include "king.h"

using namespace std;

King::King(Colour colour, bool hasMoved, int lastTwoSquareTurn) : 
    Piece(PieceType::King, colour, hasMoved, lastTwoSquareTurn) {}

bool King::isPotentialMove(Move move) const {
    if (abs(move.startCol - move.endCol) <= 1 && 
        abs(move.startRow - move.endRow) <= 1) {
        return true;
    } else if (!hasMoved && move.startRow == move.endRow && 
            abs(move.startCol - move.endCol) == 2) {
        return true;
    }
    return false;
}

MessageType King::classifyMove(Move move) const {
    if (abs(move.startCol - move.endCol) <= 1 &&   
        abs(move.startRow - move.endRow) <= 1) {
        return MessageType::PieceMove;
    }
    return MessageType::CastleKing;
}

char King::getChar() const { 
    return colour == Colour::White ? 'K' : 'k'; 
}

std::unique_ptr<Piece> King::clone() const {
    return make_unique<King>(colour, hasMoved, lastTwoSquareTurn);
}

Status King::getIntermediateStatus(const Message &message, const Piece &other) const {
    if (message.type == MessageType::CastleKing ||
            message.type == MessageType::CastleRookRelay) {
        return other.type == PieceType::NoneType ? Status::Continue : Status::Error;
    }
    return Status::Error;
}

Status King::getEndpointStatus(const Message &message, const Piece &other) const {
    switch (message.type) {
        case MessageType::PieceMove: 
            return (other.type == PieceType::NoneType || other.colour != colour) ?
                Status::Success : Status::Error;
        case MessageType::CastleKing:
            return other.type == PieceType::NoneType ? Status::Success : Status::Error;
        case MessageType::CastleRookRelay:
            return (other.type == PieceType::Rook && other.colour == colour 
                && !other.hasMoved) ? Status::Success : Status::Error;
        default: return Status::Error;
    }
}
