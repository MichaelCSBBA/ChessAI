#ifndef PIECE_H
#define PIECE_H
#include <memory>
#include "enums.h"
#include "message.h"
#include "move.h"

struct Piece {
    const PieceType type;
    const Colour colour;
    bool hasMoved;
    int lastTwoSquareTurn;
    Piece(PieceType type, Colour colour, bool hasMoved=false, int lastTwoSquareTurn=-1);
    virtual bool isPotentialMove(Move move) const = 0;
    virtual bool isPotentialCapture(Move move) const;
    virtual MessageType classifyMove(Move move) const;
    virtual char getChar() const = 0;
    virtual std::unique_ptr<Piece> clone() const = 0;
    virtual Status getIntermediateStatus(const Message &message, const Piece &other) const = 0;
    virtual Status getEndpointStatus(const Message &message, const Piece &other) const = 0;
    virtual bool operator==(const Piece &other) const;
    virtual ~Piece() = default;
};
#endif
