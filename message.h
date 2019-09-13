#ifndef MESSAGE_H
#define MESSAGE_H
#include <memory>
#include "enums.h"
#include "move.h"

struct Piece;

struct Message {
    MessageType type;
    Direction direction;
    Move move;
    std::unique_ptr<Piece> piece;
    Message clone() const;
};
#endif
