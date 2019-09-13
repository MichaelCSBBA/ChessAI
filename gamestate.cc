#include <vector>
#include <stdexcept>
#include <iostream>
#include <memory>
#include "move.h"
#include "message.h"
#include "square.h"
#include "observer.h"
#include "piece.h"
#include "enums.h"
#include "king.h"
#include "queen.h"
#include "rook.h"
#include "bishop.h"
#include "knight.h"
#include "pawn.h"
#include "none.h"
#include "gamestate.h"

using namespace std;


GameState::GameState() {
    doSetup();
    for (int j = 0; j < 8; ++j) {
        for (int i : vector<int>{0, 1, 6, 7}) {
            Colour colour = i < 5 ? Colour::White : Colour::Black;
            if (i == 1 || i == 6) {
                addPiece(make_unique<Pawn>(colour), i, j);
            } else {
                switch (j) {
                    case 0:
                    case 7: addPiece(make_unique<Rook>(colour), i, j); break;
                    case 1:
                    case 6: addPiece(make_unique<Knight>(colour), i, j); break;
                    case 2:
                    case 5: addPiece(make_unique<Bishop>(colour), i, j); break;
                    case 3: addPiece(make_unique<Queen>(colour), i, j); break;
                    case 4: addPiece(make_unique<King>(colour), i, j);
                }
            }
        }
    }
}

GameState::GameState(vector<vector<unique_ptr<Piece>>> pieces) {
    doSetup();
    // Assuming here that we're have an 8x8 board
    for(int i=0; i<8; ++i) {
        for(int j=0; j<8; ++j) {
            board[i][j]->addPiece(pieces[i][j]->clone());
        }
    }
}

void GameState::doSetup() {
    // Initializing all squares
    for(int i=0; i<8; ++i) {
        vector<shared_ptr<Square>> temp;
        for(int j=0; j<8; ++j) {
            temp.emplace_back(make_shared<Square>(i, j, *this));
        }
        board.emplace_back(temp);
    }

    // Link squares to neighbours
    for(int i=0; i<8; ++i) {
        for(int j=0; j<8; ++j) {
            for(int r=i-2; r<=i+2; ++r) {
                for(int c=j-2; c<=j+2; ++c) {
                    if(r>=0 && r<8 && c>=0 && c<8 &&
                        (((abs(r-i)==2 || abs(c-j)==2) && ((r-i)+(c-j))%2!=0) ||
                        (abs(r-i)<=1 && abs(c-j)<=1))) {
                        if (r != i || c != j) {
                            board[i][j]->attach(board[r][c]);
                        }
                    }
                }
            }
        }
    }
}

bool GameState::sendCheckRelays(Colour c) {

 // go over everysquare, sets incheck and prevent check to false for each sqaure

    int row_len = 8;
    int col_len = 8;

    int kingCol = 0;
    int kingRow = 0;

    for (int i = 0; i < row_len; i++) {
        for (int j = 0; j < col_len; j++) {
            board[i][j]->inCheck = false;
            board[i][j]->preventCheck = false;
            if(board[i][j]->piece->type==PieceType::King && board[i][j]->piece->colour==c) {
                kingCol = j;
                kingRow = i;
            }
        }
    }

    vector<Message> relays;
    auto p = getPiece(kingRow, kingCol);
    Move default_move = Move{c,kingRow,kingCol,-1,-1,-1,PieceType::NoneType};
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::NW, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::N, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::NE, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::W, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::E, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::SW, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::SE, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::S, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::L1, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::L2, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::L3, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::L4, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::L5, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::L6, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::L7, default_move, p->clone()});
    relays.emplace_back(Message{MessageType::CheckRelay, Direction::L8, default_move, p->clone()});
    
    
    for (int i = 0; i < 16; i++) {
        board[kingRow][kingCol]->notifyObservers(relays[i]);
    }
    return board[kingRow][kingCol]->getInCheck();
}

bool GameState::isValidIndex(int row, int col) const {
    return row >= 0 && col >= 0 && row < 8 && col < 8;
}

bool GameState::hasPiece(int row, int col) const {
    if (!isValidIndex(row, col)) {
        throw invalid_argument("");
    }
    return board[row][col]->hasPiece();
}

unique_ptr<Piece> GameState::getPiece(int row, int col) const {
    if (!isValidIndex(row, col)) {
        throw invalid_argument("");
    }
    return board[row][col]->getPiece();
}

void GameState::addPiece(unique_ptr<Piece> piece, int row, int col) {
    if (!isValidIndex(row, col)) {
        throw invalid_argument("");
    }
    board[row][col]->addPiece(std::move(piece));
}

void GameState::removePiece(int row, int col) {
    if (!isValidIndex(row, col)) {
        throw invalid_argument("");
    }
    board[row][col]->removePiece();
}

bool GameState::isValidBoard() {
    int black_king_present = 0;
    int white_king_present = 0;
    bool pawn_placement = true;
    int row_length = 8;
    int col_length = 8;
    for (int i = 0; i < row_length; i++) {
        for (int j = 0; j < col_length; j++) {
            auto p = board[i][j]->getPiece();
            if (p->type == PieceType::King) {
                if (p->colour == Colour::Black) {
                    black_king_present++;
                }
                if (p->colour == Colour::White) {
                    white_king_present++;
                }
            }
        }
    }
    for (int j = 0; j < row_length; j++) {
        auto pw = board[0][j]->getPiece();
        auto pb = board[7][j]->getPiece();
        if (pw->type == PieceType::Pawn || pb->type == PieceType::Pawn) {
            pawn_placement = false;
        }
    }   
    // check kings are not in check
    if (sendCheckRelays(Colour::White) || sendCheckRelays(Colour::Black)) {
        return false;
    }
    //  check number of kings (one of each colour)
    // check positions of pawns
    return (black_king_present == 1 && white_king_present == 1
            && pawn_placement);
}

void GameState::attachObserver(std::shared_ptr<Observer<Info, Message>> o) {
    for (auto &row : board) {
        for (auto &square : row) {
            square->attach(o);
            // notify the observer about the pieces currently on the board
            square->notifyObservers(Message{
                MessageType::PieceChanged, 
                Direction::N,
                Move{Colour::NoColour, -1, -1, -1, -1, -1, PieceType::NoneType},
                square->getPiece()
            });
        }
    }
}

vector<vector<unique_ptr<Piece>>> GameState::getAllPieces() const {
    int row = 8;
    int col = 8;
    vector<vector<unique_ptr<Piece>>> board_access(row);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            board_access[i].emplace_back(getPiece(i, j));
        }
    }
    return board_access;
}

vector<Move> GameState::getPotentialMoves(int row, int col) const {
    if (!isValidIndex(row, col)) {
        throw invalid_argument("");
    }
    vector <Move> pot_move_vec;
    int row_length = 8;
    int col_length = 8;
    for (int i = 0; i < row_length; i++) {
        for (int j = 0; j < col_length; j++) {
            if (i == row && j == col) {
                continue;
            }
            auto p1 = getPiece(row,col);
            struct Move m{p1->colour, row, col, i, j, 1, PieceType::NoneType};
            if (p1->isPotentialMove(m)) {
                pot_move_vec.emplace_back (m);
            } else if (p1->type == PieceType::Pawn) {
                // if it didn't work, maybe it's a pawn that needs promoting 
                m.promoteTo = PieceType::Queen;
                if (p1->isPotentialMove(m)) {
                    pot_move_vec.emplace_back(m);
                    m.promoteTo = PieceType::Rook;
                    pot_move_vec.emplace_back(m);
                    m.promoteTo = PieceType::Bishop;
                    pot_move_vec.emplace_back(m);
                    m.promoteTo = PieceType::Knight;
                    pot_move_vec.emplace_back(m);
                }
            }
        }
    }
    return pot_move_vec;
}

unique_ptr<GameState> GameState::clone() const {
    return make_unique<GameState>(getAllPieces());
}

void GameState::recordPieceChange(int row, int col, unique_ptr<Piece> piece) {
    if (pieceChanges.size()) {
        pieceChanges.back().emplace_back(PieceChange{row, col, std::move(piece)});
    }
}

void GameState::undoLast() {
    if (moveHistory.size()) {
        moveHistory.pop_back();
    }
    if (pieceChanges.size()) {
        auto &moves = pieceChanges.back();
        for (int i = moves.size() - 1; i >= 0; --i) {
            addPiece(std::move(moves[i].piece), moves[i].row, moves[i].col);
        }
        pieceChanges.pop_back();
    }
}

void GameState::printHistory(ostream &out) {
    for (auto m : moveHistory) {
        out << m; 
    }
}

MoveResult GameState::move(Move m, bool testPotentialMoves) {
    int row = m.startRow;
    int col = m.startCol;
    // check for bounds, colour, potential move
    if (!isValidIndex(row, col) || !isValidIndex(m.endRow, m.endCol) || 
        (row == m.endRow && col == m.endCol) || m.player != board[row][col]->piece->colour) {
        return MoveResult::Error;
    }
    auto p = getPiece (m.startRow, m.startCol);
    if (!p->isPotentialMove(m)) {
        return MoveResult::Error;
    }

    Direction n_direction = board[0][0]->getDirectionFromCoords(m.startRow, m.startCol, m.endRow, m.endCol);
    MessageType n_message = p->classifyMove (m);
    Message msg = {n_message, n_direction, m, p->clone()};
    msg.piece->hasMoved = true;

    // check that the king is not in check before castling
    if (n_message == MessageType::CastleKing) {
        if (sendCheckRelays(m.player)) {
            return MoveResult::Error; 
        }
    }

    moveHistory.emplace_back(m);
    pieceChanges.emplace_back(vector<PieceChange>());
    removePiece(m.startRow, m.startCol);

    // check for move legality using observer pattern, error will be thrown if illegal
    try {
        board[row][col]->notifyObservers(msg);
    }
    catch(invalid_argument&) {
        undoLast();
        return MoveResult::Error;
    }

    // if the current player is in check at the end of a move, the move is illegal
    if(sendCheckRelays(m.player)) {
        undoLast();
        return MoveResult::Error;
    }
    
    if(!testPotentialMoves) return MoveResult::Other;

    Colour opp = m.player == Colour::White ? Colour::Black : Colour::White;
    int kingRow = 0;
    int kingCol = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) { // Retrieve the enemy king
            if(board[i][j]->piece->type==PieceType::King && board[i][j]->piece->colour==opp) {
                kingCol = j;
                kingRow = i;
            }
        }
    }
    
    if(sendCheckRelays(opp)) { // If enemy is now in check
        // Copy over preventCheck values since they might be unset later
        vector<vector<bool>> preventCheckValues(8, vector<bool>(8, false));
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                preventCheckValues[i][j] = board[i][j]->getPreventCheck();
            }
        }
        vector<Move> moves = getPotentialMoves(kingRow, kingCol);
        for(auto mov : moves) { // Check King moves first, for efficiency
            if(move(mov, false) != MoveResult::Error) {
                undoLast();
                return MoveResult::Check;
            }
        }
        // check all other possible piece moves which end on a preventCheck square
        // i.e. one which is between or on the checking piece
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if(board[i][j]->piece->colour==opp && board[i][j]->piece->type!=PieceType::King) {
                    moves = getPotentialMoves(i, j);
                    for(auto mov : moves) {
                        if(preventCheckValues[mov.endRow][mov.endCol]) {
                            if (move(mov, false) != MoveResult::Error) {
                                undoLast();
                                return MoveResult::Check;
                            }
                        }
                    }
                }
            }
        }
        // If there doesn't exist any moves to prevent check
        return MoveResult::Checkmate;
    }

    // if not in check, still check possible moves to determine stalemate
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) { 
            if(board[i][j]->piece->colour==opp) {
                vector<Move> moves = getPotentialMoves(i, j);
                for (auto mov : moves) {
                    if (move(mov, false) != MoveResult::Error) {
                        undoLast();
                        return MoveResult::Other;
                    }
                }
            }
        }
    }
    return MoveResult::Stalemate;
}

int GameState::pieceColourValue(Colour colour) {
    int row_len = 8;
    int col_len = 8;
    int position_value = 0;
    for (int i = 0; i < row_len; ++i) {
        for (int j = 0; j < col_len; ++j) {
            auto temp_piece = getPiece (i, j);
            if (temp_piece->colour == colour) {
                if (temp_piece->type == PieceType::Queen) {
                    position_value += 9;
                } else if (temp_piece->type == PieceType::Rook) {
                    position_value += 5;
                } else if (temp_piece->type == PieceType::Bishop) {
                    // classically, the bishop has value 3, but modern analysis places it higher
                    position_value += 4;
                } else if (temp_piece->type == PieceType::Knight) {
                    position_value += 3;
                } else if (temp_piece->type == PieceType::Pawn) {
                    position_value += 1;
                } else { // king and no piece case
                    position_value += 0;
                }
            } else {
                continue;
            }
        }
    }
    return position_value;
}
