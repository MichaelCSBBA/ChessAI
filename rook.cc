#include <cmath>
#include <memory>
#include "enums.h"
#include "move.h"
#include "message.h"
#include "piece.h"
#include "rook.h"

using namespace std;

Rook::Rook(Colour colour, bool hasMoved, int lastTwoSquareTurn) : 
    Piece(PieceType::Rook, colour, hasMoved, lastTwoSquareTurn) {}

bool Rook::isPotentialMove(Move move) const {
    return move.startCol == move.endCol || move.startRow == move.endRow;
}

char Rook::getChar() const { 
    return colour == Colour::White ? 'R' : 'r'; 
}

std::unique_ptr<Piece> Rook::clone() const {
    return make_unique<Rook>(colour, hasMoved, lastTwoSquareTurn);
}

Status Rook::getIntermediateStatus(const Message &message, const Piece &other) const {
    switch (message.type) {
        case MessageType::PieceMove:
            return other.type == PieceType::NoneType ? Status::Continue : Status::Error;
        case MessageType::CastleRookReply: return Status::Continue;
        default: return Status::Error;
    }
}

Status Rook::getEndpointStatus(const Message &message, const Piece &other) const {
    switch (message.type) {
        case MessageType::PieceMove: 
            return (other.type == PieceType::NoneType || other.colour != colour) ?
                Status::Success : Status::Error;
        case MessageType::CastleRookReply: return Status::Success;
        default: return Status::Error;
    }
}
