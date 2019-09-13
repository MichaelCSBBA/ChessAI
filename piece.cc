#include "enums.h"
#include "move.h"
#include "piece.h"

Piece::Piece(PieceType type, Colour colour, bool hasMoved, int lastTwoSquareTurn) : 
    type{type}, colour{colour}, hasMoved{hasMoved}, lastTwoSquareTurn{lastTwoSquareTurn} {}

bool Piece::isPotentialCapture(Move move) const {
    return isPotentialMove(move);
}

MessageType Piece::classifyMove(Move move) const {
    return MessageType::PieceMove;
}

bool Piece::operator==(const Piece &other) const {
    return this->type == other.type && 
        this->colour == other.colour && 
        this->hasMoved == other.hasMoved && 
        this->lastTwoSquareTurn == other.lastTwoSquareTurn;
}
