#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include "enums.h"
#include "move.h"
#include "computer1.h"

using namespace std;

Move Computer1::getMove(GameState &g, istream &in, Colour colour, int turn) {
    vector<pair<int, int>> coords;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            coords.emplace_back(make_pair(i, j));
        }
    }
    random_shuffle(coords.begin(), coords.end());
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
            if (result != MoveResult::Error) {
                return move;
            }
        }
    }
}
