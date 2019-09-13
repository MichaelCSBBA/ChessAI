#include <cmath>
#include <memory>
#include "enums.h"
#include "move.h"
#include "message.h"
#include "piece.h"
#include "pawn.h"

using namespace std;

Pawn::Pawn(Colour colour, bool hasMoved, int lastTwoSquareTurn) :
    Piece(PieceType::Pawn, colour, hasMoved, lastTwoSquareTurn) {}

bool Pawn::isPotentialMove(Move move) const {
    if (abs(move.startCol - move.endCol) > 1) {
        return false;
    }
    if (move.promoteTo == PieceType::King || move.promoteTo == PieceType::Pawn) {
        return false;
    }
    int rowOffset = move.endRow - move.startRow;
    int firstRow, offsetSign, lastRow;
    if (colour == Colour::White) {
        firstRow = 1;
        offsetSign = 1;
        lastRow = 6;
    } else {
        firstRow = 6;
        offsetSign = -1;
        lastRow = 1;
    }
    if ((move.startRow == firstRow && rowOffset == 2 * offsetSign && 
            move.startCol == move.endCol) || rowOffset == offsetSign) {
        //if on last rank, must promote
        return (move.startRow == lastRow) == (move.promoteTo != PieceType::NoneType);
    }
    return false;
}

bool Pawn::isPotentialCapture(Move move) const {
    return isPotentialMove(move) && abs(move.startCol - move.endCol) == 1;
}

MessageType Pawn::classifyMove(Move move) const {
    return isPotentialCapture(move) ? 
        MessageType::PawnCapture : MessageType::PieceMove;
}

char Pawn::getChar() const { 
    return colour == Colour::White ? 'P' : 'p'; 
}

std::unique_ptr<Piece> Pawn::clone() const {
    return make_unique<Pawn>(colour, hasMoved, lastTwoSquareTurn);
}

Status Pawn::getIntermediateStatus(const Message &message, const Piece &other) const {
    if (message.type == MessageType::PieceMove) {
        return other.type == PieceType::NoneType ? Status::Continue : Status::Error;
    }
    return Status::Error;
}

Status Pawn::getEndpointStatus(const Message &message, const Piece &other) const {
    int lastRow;
    if (colour == Colour::White) {
        lastRow = 6;
    } else {
        lastRow = 1;
    }
    Status success = message.move.startRow == lastRow ? 
        Status::Promote : Status::Success;
    switch (message.type) {
        case MessageType::PieceMove: 
            return other.type == PieceType::NoneType ? success : Status::Error;
        case MessageType::PawnCapture:
            return other.type == PieceType::NoneType ? Status::CheckEnPassant : 
                (other.colour == colour ? Status::Error : success);
        case MessageType::EnPassantRelay:
            return (other.type == PieceType::Pawn && 
                other.lastTwoSquareTurn == message.move.turn - 1 &&
                other.colour != colour) ? success : Status::Error;
        default: return Status::Error;
    }
}
