#ifndef ROOK_H
#define ROOK_H
#include <memory>
#include "enums.h"
#include "message.h"
#include "move.h"
#include "piece.h"

struct Rook : public Piece {
    Rook(Colour colour, bool hasMoved=false, int lastTwoSquareTurn=-1);
    bool isPotentialMove(Move move) const override;
    char getChar() const override;
    std::unique_ptr<Piece> clone() const override;
    Status getIntermediateStatus(const Message &message, const Piece &other) const override;
    Status getEndpointStatus(const Message &message, const Piece &other) const override;
};
#endif
