#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <vector>
#include <iostream>
#include <memory>
#include "move.h"
#include "square.h"
#include "observer.h"
#include "piece.h"
#include "enums.h"

class GameState {
    std::vector<std::vector<std::shared_ptr<Square>>> board;
    struct PieceChange {
        const int row, col;
        std::unique_ptr<Piece> piece;
    };
    std::vector<std::vector<PieceChange>> pieceChanges;
    std::vector<Move> moveHistory;
    bool sendCheckRelays(Colour c);
    bool isPotentialMove(Move move);
    bool isValidIndex(int row, int col) const;
    void recordPieceChange(int row, int col, std::unique_ptr<Piece> piece);
    void doSetup();
    public:
        GameState();
        GameState(std::vector<std::vector<std::unique_ptr<Piece>>>);
        bool hasPiece(int row, int col) const;
        std::unique_ptr<Piece> getPiece(int row, int col) const;
        void addPiece(std::unique_ptr<Piece> piece, int row, int col);
        void removePiece(int row, int col);
        bool isValidBoard();
        void attachObserver(std::shared_ptr<Observer<Info, Message>> o);
        std::vector<std::vector<std::unique_ptr<Piece>>> getAllPieces() const;
        std::vector<Move> getPotentialMoves(int row, int col) const;
        std::unique_ptr<GameState> clone() const;
        void undoLast();
        void printHistory(std::ostream &out);
        MoveResult move(Move m, bool testPotentialMoves=true);
        int pieceColourValue(Colour colour);
        friend class Square;
};
#endif
