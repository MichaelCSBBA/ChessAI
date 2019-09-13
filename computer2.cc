#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include "enums.h"
#include "move.h"
#include "computer2.h"

using namespace std;

int Computer2::getNumOppositeColourPieces(GameState &g, Colour colour) const {
    int total = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (g.hasPiece(i, j) && g.getPiece(i, j)->colour != colour) {
                total++;
            }
        }
    }
    return total;
}

Move Computer2::getMove(GameState &g, istream &in, Colour colour, int turn) {
    vector<pair<int, int>> coords;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            coords.emplace_back(make_pair(i, j));
        }
    }
    int initialNumPieces = getNumOppositeColourPieces(g, colour);
    random_shuffle(coords.begin(), coords.end());
    Move backup{colour, -1, -1, -1, -1, turn, PieceType::NoneType};
    for (auto coord : coords) {
        if (!g.hasPiece(coord.first, coord.second) || 
                g.getPiece(coord.first, coord.second)->colour != colour) {
            continue;
        }
        auto moves = g.getPotentialMoves(coord.first, coord.second);
        random_shuffle(moves.begin(), moves.end());
        for (auto move : moves) {
            move.turn = turn;
            MoveResult result = g.move(move);
            if (result == MoveResult::Check || result == MoveResult::Checkmate) {
                return move;
            } else if (result != MoveResult::Error) {
                if (getNumOppositeColourPieces(g, colour) < initialNumPieces) {
                    return move;
                }
                backup = move;
                g.undoLast();
            }
        }
    }
    return backup;
}
