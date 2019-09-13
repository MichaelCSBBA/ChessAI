#include <memory>
#include <iostream>
#include "enums.h"
#include "move.h"
#include "player.h"

using namespace std;

Move Player::getMove(GameState &g, istream &in, Colour colour, int turn) {
    string start, end;
    in >> start >> end;
    if (start.length() < 2 || end.length() < 2) {
        throw iostream::failure("");
    }
    // use ascii conversion to get row and col from "a1", for example
    int startCol = start[0] - 'a';
    int startRow = start[1] - '1';
    int endCol = end[0] - 'a';
    int endRow = end[1] - '1';
    PieceType piece = PieceType::NoneType;
    string promote;
    // parse the piece to promote, if one exists
    if (in >> promote) {
        if (promote.length() < 1) {
            throw iostream::failure("");
        }
        switch (promote[0]) {
            case 'Q': piece = PieceType::Queen; break;
            case 'R': piece = PieceType::Rook; break;
            case 'B': piece = PieceType::Bishop; break;
            case 'N': piece = PieceType::Knight;
        }
    }
    return Move{colour, startRow, startCol, endRow, endCol, turn, piece};
}

Player::~Player() {}
