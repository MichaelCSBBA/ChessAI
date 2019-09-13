#include <iostream>
#include <algorithm>
#include <utility>
#include <memory>
#include "enums.h"
#include "move.h"
#include "computer4.h"

using namespace std;

const float MAX_SCORE = 10000;

// minDepth is the depth to which all moves are considered
// only forcing/forced moves are considered up to maxDepth (currently, checks)
Computer4::Computer4(int minDepth, int maxDepth) : minDepth{minDepth}, maxDepth{maxDepth},
    killerMoves{vector<Move>(maxDepth, Move{Colour::White, -1, -1, -1, -1, -1, PieceType::NoneType})} {}

float Computer4::evaluateBoard(GameState &g, Colour colour) const {
    Colour other = colour == Colour::White ? Colour::Black : Colour::White;
    return g.pieceColourValue(colour) - g.pieceColourValue(other);
}

// updates values of alpha and beta, depending on whether the player is maximizing or not
// if the currently considered move is better, update the return value and bestMove
void Computer4::updateAlphaBeta(float returnValue, Move move, float &alpha, float &beta,
        float &value, Move &bestMove, bool isMaximizing) const {
    if (isMaximizing) {
        if (returnValue > value) {
            value = returnValue;
            bestMove = move;
        }
        alpha = max(alpha, value);
    } else {
        if (returnValue < value) {
            value = returnValue;
            bestMove = move;
        }
        beta = min(beta, value);
    }
}

// minmax + alpha beta search
std::pair<Move, float> Computer4::alphaBetaSearch(GameState &g, int depth, float alpha,
        float beta, Colour colour, bool isMaximizing, bool isForcing) {
    Move bestMove = Move{colour, -1, -1, -1, -1, turn + depth, PieceType::NoneType};
    // stop searching and return the board evaluation
    if ((depth == minDepth && !isForcing) || (depth == maxDepth && isForcing)) {
        return make_pair(bestMove, evaluateBoard(g, colour));
    }
    float value = MAX_SCORE;
    if (isMaximizing) {
        value = -value;
    }
    Colour other = colour == Colour::White ? Colour::Black : Colour::White;
    // use a "killer move" if one exists
    // i.e. those which have previously terminated alpha beta pruning, at a given depth
    vector<pair<float, Move>> moves;
    moves.emplace_back(make_pair(0, killerMoves[depth]));
    float originalEval = evaluateBoard(g, colour);
    // go over all potential moves and rank them by value of capture
    // used as a heuristic to speed up alpha beta pruning
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (!g.hasPiece(i, j) || g.getPiece(i, j)->colour != colour) {
                continue;
            }
            float baseEval = 0;
            // to prevent aimless moves, we favour pawn advances, all else being equal
            if (g.getPiece(i, j)->type == PieceType::Pawn) {
                baseEval -= 0.5;
            }
            auto unsortedMoves = g.getPotentialMoves(i, j);
            for (Move move : unsortedMoves) {
                if (g.hasPiece(move.endRow, move.endCol)) {
                    auto endPiece = g.getPiece(move.endRow, move.endCol);
                    if (endPiece->colour != colour) {
                        switch (endPiece->type) {
                            case PieceType::Queen: 
                                moves.emplace_back(make_pair(1 + baseEval, move)); 
                                break;
                            case PieceType::Rook:
                                moves.emplace_back(make_pair(2 + baseEval, move)); 
                                break;
                            case PieceType::Bishop:
                                moves.emplace_back(make_pair(3 + baseEval, move));
                                break;
                            case PieceType::Knight:
                                moves.emplace_back(make_pair(4 + baseEval, move)); 
                                break;
                            case PieceType::Pawn:
                                moves.emplace_back(make_pair(5 + baseEval, move)); 
                                break;
                        }
                    } else {
                        moves.emplace_back(make_pair(7 + baseEval, move));
                    }
                } else {
                    moves.emplace_back(make_pair(6 + baseEval, move));
                }
            }
        }
    }
    sort(moves.begin() + 1, moves.end());
    // simulate each move and perform recursive search
    for (pair<float, Move> movePair : moves) {
        Move move = movePair.second;
        move.player = colour;
        move.turn = turn + depth;
        MoveResult result = g.move(move);
        switch (result) {
            case MoveResult::Checkmate: {
                g.undoLast();
                // checkmate evaluation is very high or very low (depending on the player)
                // subtract depth so we prefer faster checkmates
                float returnValue = MAX_SCORE - depth;
                if (!isMaximizing) {
                    returnValue = -returnValue;
                }
                updateAlphaBeta(returnValue, move, alpha, beta, value,
                    bestMove, isMaximizing);
                killerMoves[depth] = move;
                return make_pair(move, value);
            }
            case MoveResult::Error: continue;
            case MoveResult::Stalemate: {
                // stalemate evaluation is 0
                g.undoLast();
                updateAlphaBeta(0, move, alpha, beta, value, bestMove, 
                    isMaximizing);
                if (alpha >= beta) {
                    killerMoves[depth] = move;
                    return make_pair(bestMove, value);
                }
                continue;
            }
        }
        bool isForcingMove = false;
        // a forcing move is a check by the maximizing player, or a response to the check
        if (isForcing || depth == minDepth - 2) {
            isForcingMove = result == MoveResult::Check;
        }
        if (isForcing && !isForcingMove && isMaximizing) {
            g.undoLast();
            continue;
        }
        float returnValue = alphaBetaSearch(g, depth + 1, alpha, beta, other,
            !isMaximizing, isForcingMove || isForcing).second;
        g.undoLast();
        updateAlphaBeta(returnValue, move, alpha, beta, value, bestMove,
            isMaximizing);
        if (alpha >= beta) {
            killerMoves[depth] = move;
            return make_pair(bestMove, value);
        }
    }
    return make_pair(bestMove, value);
}

Move Computer4::getMove(GameState &g, istream &in, Colour colour, int turn) {
    float alpha = -MAX_SCORE;
    float beta = MAX_SCORE;
    this->turn = turn;
    return alphaBetaSearch(g, 0, alpha, beta, colour, true).first;
}
