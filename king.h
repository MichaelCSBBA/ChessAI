#ifndef KING_H
#define KING_H
#include <memory>
#include "enums.h"
#include "message.h"
#include "move.h"
#include "piece.h"

struct King : public Piece {
    King(Colour colour, bool hasMoved=false, int lastTwoSquareTurn=-1);
    bool isPotentialMove(Move move) const override;
    MessageType classifyMove(Move move) const override;
    char getChar() const override;
    std::unique_ptr<Piece> clone() const override;
    Status getIntermediateStatus(const Message &message, const Piece &other) const override;
    Status getEndpointStatus(const Message &message, const Piece &other) const override;
};
#endif
