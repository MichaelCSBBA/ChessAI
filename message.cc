#include "message.h"
#include "piece.h"

Message Message::clone() const {
    return Message{type, direction, move, piece->clone()};
}
