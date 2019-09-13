#ifndef SQUARE_H
#define SQUARE_H
#include <iostream>
#include <memory>
#include "subject.h"
#include "observer.h"
#include "enums.h"
#include "message.h"
#include "piece.h"
#include "info.h"

class GameState;

class Square : public Subject<Info, Message>, public Observer<Info, Message> {
    const int row, col;
    std::unique_ptr<Piece> piece;
    bool inCheck, preventCheck;
    GameState &game;
    Direction getDirectionFromCoords(
        int startRow, int startCol, int endRow, int endCol) const;
    Direction getOppositeDirection(Direction dir) const;
    std::unique_ptr<Piece> makePromotedPiece(PieceType promoteTo, Colour colour);
    public:
        Square(int row, int col, GameState &game);
        bool hasPiece() const;
        std::unique_ptr<Piece> getPiece() const;
        void addPiece(std::unique_ptr<Piece> piece);
        void removePiece();
        bool getInCheck() const;
        bool getPreventCheck() const;
        void setInCheck(bool inCheck);
        void setPreventCheck(bool preventCheck);
        void notify(Subject<Info, Message> &whoFrom, const Message& m) override;
        Info getInfo() const override;
        friend class GameState;
};
#endif
