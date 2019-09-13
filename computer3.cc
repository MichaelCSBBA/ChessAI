#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include "enums.h"
#include "move.h"
#include "computer3.h"

using namespace std;


Move Computer3::getMove(GameState &g, istream &in, Colour colour, int turn) {
    vector<pair<int, int>> coords;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            coords.emplace_back(make_pair(i, j)); // makes a vector of all possible 64 coordinates
        }
    }
    
    random_shuffle(coords.begin(), coords.end());  // randomizes the vector
    Move backup{colour, -1, -1, -1, -1, turn, PieceType::NoneType};

    const int NO_MOVE = -142; // arbitrary large negative number
    const int CHECKMATE_PRIORITY = 68;  // arbitrary large number to override all other priorities
    int capPriority = 0; // priority for captures
    int nmePriority = 0; // priority used in predicting the best move for the enemy to take
    int priorityDiff = NO_MOVE; // difference on capture priority and enemy priority
    Colour oColour = colour == Colour::White ? Colour::Black : Colour::White; // opposite colour to 'colour'
    int initCV = g.pieceColourValue(oColour);  // initial Colour Values for the enemy

    for (auto coord : coords) {
        if (!g.hasPiece(coord.first, coord.second) || 
                g.getPiece(coord.first, coord.second)->colour != colour) {
            continue; // ensures that the square is a piece of 'colour'
        }
        
        auto moves = g.getPotentialMoves(coord.first, coord.second); // Get all moves for our current piece
        random_shuffle(moves.begin(), moves.end()); // randomizes moves
        for (auto move : moves) { // loop through all potential moves
            move.turn = turn;
            MoveResult result = g.move(move); // simulate 'move'
            if (result == MoveResult::Checkmate) {
                return move;
            }
            else if (result != MoveResult::Error) {
                nmePriority = 0; 
                for(auto ocoord : coords) {
                    if(!g.hasPiece(ocoord.first, ocoord.second) ||
                        g.getPiece(ocoord.first, ocoord.second)->colour == colour) {
                        continue; // ensures ocoord contains an enemy piece
                    }
                    auto omoves = g.getPotentialMoves(ocoord.first, ocoord.second); // gets all enemy moves
                    random_shuffle(omoves.begin(), omoves.end()); // randomizes enemy moves
                    int initNmeCV = g.pieceColourValue(colour); // initial Colour Value of 'colour' pieces
                    for(auto omove : omoves) { // loop through all potential enemy moves, given move
                        omove.turn = turn + 1;
                        MoveResult nmeMr = g.move(omove); // simulate enemy move
                        int tempNmeCV = g.pieceColourValue(colour); // get the new Colour Values of 'colour' pieces

                        if(nmeMr == MoveResult::Error) continue;

                        g.undoLast(); // undos the simulated enemy move
                        
                        // Assign any special case values to nmeMr
                        if(nmeMr == MoveResult::Checkmate) {
                            nmePriority = CHECKMATE_PRIORITY;
                            break;
                        }
                        if(nmeMr == MoveResult::Check) nmePriority += 2;

                        // update nmePriority to be the largest possible difference of NmeCV
                        nmePriority = initNmeCV - tempNmeCV > nmePriority ? 
                                        initNmeCV - tempNmeCV : nmePriority;
                        
                    }
                } // we now have nmePriority, the worst case value the enemy could move
                
                
                int tempCV = g.pieceColourValue(oColour); // get the new Colour Values of enemy pieces

                // assign capPriority to be the difference of Colour Values, with any special cases added
                capPriority = initCV - tempCV;
                if(result == MoveResult::Check) capPriority += 2; 

                // update priorityDiff the be the largest possible difference of capture and enemy priorities
                if(capPriority - nmePriority > priorityDiff) {
                    priorityDiff = capPriority - nmePriority;
                    backup = move;
                }

                g.undoLast(); // undo out simulated move
            }
        }
    }
    return backup;
}
