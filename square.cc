#include <iostream>
#include "subject.h"
#include "enums.h"
#include "piece.h"
#include "queen.h"
#include "rook.h"
#include "bishop.h"
#include "knight.h"
#include "message.h"
#include "info.h"
#include "none.h"
#include "square.h"
#include "gamestate.h"

using namespace std;

Square::Square(int row, int col, GameState &game) : row{row}, col{col}, 
    piece{make_unique<None>()}, inCheck{false}, preventCheck{false}, game{game} {}

Direction Square::getDirectionFromCoords(
    int startRow, int startCol, int endRow, int endCol) const {

    if(startRow < endRow) { // North
        if(startCol == endCol + 2 && startRow == endRow - 1) return Direction::L1;
        if(startCol == endCol + 1 && startRow == endRow - 2) return Direction::L2;
        if(startCol == endCol) return Direction::N;
        if(startCol == endCol - 1 && startRow == endRow - 2) return Direction::L3;
        if(startCol == endCol - 2 && startRow == endRow - 1) return Direction::L4;
        if(startCol < endCol) return Direction::NE;
        return Direction::NW;
    }
    else if(startRow == endRow) {
        if(startCol < endCol) return Direction::E;
        return Direction::W;
    }
    else { // South
        if(startCol == endCol - 2 && startRow == endRow + 1) return Direction::L5;
        if(startCol == endCol - 1 && startRow == endRow + 2) return Direction::L6;
        if(startCol == endCol) return Direction::S;
        if(startCol == endCol + 1 && startRow == endRow + 2) return Direction::L7;
        if(startCol == endCol + 2 && startRow == endRow + 1) return Direction::L8;
        if(startCol < endCol) return Direction::SE;
        return Direction::SW;
    }
}

Direction Square::getOppositeDirection(Direction dir) const {
    if(dir == Direction::N) return Direction::S;
    if(dir == Direction::S) return Direction::N;
    if(dir == Direction::NE) return Direction::SW;
    if(dir == Direction::SW) return Direction::NE;
    if(dir == Direction::NW) return Direction::SE;
    if(dir == Direction::SE) return Direction::NW;
    if(dir == Direction::E) return Direction::W;
    if(dir == Direction::W) return Direction::E;
    if(dir == Direction::L1) return Direction::L5;
    if(dir == Direction::L2) return Direction::L6;
    if(dir == Direction::L3) return Direction::L7;
    if(dir == Direction::L4) return Direction::L8;
    if(dir == Direction::L5) return Direction::L1;
    if(dir == Direction::L6) return Direction::L2;
    if(dir == Direction::L7) return Direction::L3;
    if(dir == Direction::L8) return Direction::L4;
}

bool Square::hasPiece() const {
    return piece->type != PieceType::NoneType;
}

unique_ptr<Piece> Square::getPiece() const { 
    return piece->clone();
}

void Square::addPiece(unique_ptr<Piece> piece) {
    game.recordPieceChange(row, col, getPiece());
    this->piece = move(piece);
    Move mv{this->piece->colour, -1, -1, -1, -1, -1, this->piece->type};
    notifyObservers(Message{MessageType::PieceChanged, Direction::N, mv, getPiece()});
}

void Square::removePiece() {
    addPiece(make_unique<None>());
}

bool Square::getInCheck() const {
    return inCheck;
}

bool Square::getPreventCheck() const {
    return preventCheck;
}

void Square::setInCheck(bool inCheck) {
    this->inCheck = inCheck;
}

void Square::setPreventCheck(bool preventCheck) {
    this->preventCheck = preventCheck;
}

unique_ptr<Piece> Square::makePromotedPiece(PieceType promoteTo, Colour colour) {
    switch (promoteTo) {
        case PieceType::Rook: return make_unique<Rook>(colour, true);
        case PieceType::Bishop: return make_unique<Bishop>(colour, true);
        case PieceType::Knight: return make_unique<Knight>(colour, true);
        default: return make_unique<Queen>(colour, true);
    }
}

void Square::notify(Subject<Info, Message> &whoFrom, const Message &m) {
    // Check if the direction between 'this' Square and whoFrom is the same as the direction in Message m
    bool dir = getDirectionFromCoords(whoFrom.getInfo().row, whoFrom.getInfo().col, row, col) == m.direction;
    if (!dir)
        return; // If 'this' Square is not in the proper direction, exit
    
    bool end = row == m.move.endRow && col == m.move.endCol; // Check if 'this' Square is the destination of m

    // Depending on if 'this' is m's destination, get 'this' Piece's appropriate Status with m
    Status status = end ? m.piece->getEndpointStatus(m, *getPiece())
                        : m.piece->getIntermediateStatus(m, *getPiece());


    switch(m.type) {
        case MessageType::PieceMove: // A "regular" move
            switch(status) {
                case Status::Error:
                    throw invalid_argument("");
                case Status::Success: // Move m's Piece to this Square
                    addPiece(m.piece->clone());
                    if(abs(row - m.move.startRow) == 2 && m.piece->type == PieceType::Pawn) 
                        piece->lastTwoSquareTurn = m.move.turn; // updates value for a Pawn move
                    break;
                case Status::Promote: // Creates the promoted piece at 'this' Square
                    addPiece(makePromotedPiece(m.move.promoteTo, m.move.player));   
                    break;
                case Status::Continue: // Relay the PieceMove message onto 'this' observers
                    notifyObservers(m);
                    break;
            }
            break;

        case MessageType::EnPassantRelay: // A Pawn who was captured En Passant
            switch(status) {
            case Status::Error:
                throw invalid_argument("");
            
            case Status::Success:
                removePiece(); // Remove the captured piece
                break;
            }
            break;

        case MessageType::PawnCapture: // A Pawn's capture (standard or En Passant)
            switch(status) {
                case Status::Error:
                    throw invalid_argument("");

                case Status::Success: // A Pawn's standard capture
                    addPiece(m.piece->clone());
                    break;
                case Status::Promote: // Create a promoted piece at 'this'
                    addPiece(makePromotedPiece(m.move.promoteTo, m.move.player));
                    break;
                case Status::CheckEnPassant: { // A Pawn's capture by En Passant
                    addPiece(m.piece->clone());
                    Message mCopy = m.clone(); // Copy and modify Message to relay onto the adjacent Square
                    mCopy.type = MessageType::EnPassantRelay;
                    mCopy.direction = m.piece->colour==Colour::White ? Direction::S : Direction::N;
                    mCopy.move.endRow = m.piece->colour == Colour::White ? m.move.endRow - 1 :
                        m.move.endRow + 1;
                    notifyObservers(mCopy);
                    break;
                }
            }
            break;

        case MessageType::CastleKing: // Moves the King in a castle
            switch(status) {
                case Status::Error:
                    throw invalid_argument("");

                case Status::Success: { // King is at the desired destination
                    addPiece(m.piece->clone()); // Insert the King at 'this' Square
                    game.sendCheckRelays(m.piece->colour);
                    if (getInCheck()) { // Ensures the destination square does not put King in check
                        throw invalid_argument("");
                    }
                    Message mCopy = m.clone(); // Clone Message, modify to send a relay for Rook to move
                    mCopy.type = MessageType::CastleRookRelay;
                    mCopy.piece = getPiece();
                    mCopy.move.endCol = m.direction == Direction::W ? 0 : 7;
                    notifyObservers(mCopy);
                    break;
                }
                case Status::Continue: { // King is in the process of moving
                    addPiece(m.piece->clone()); // Adds King temporarily for sendCheckRelays to be called
                    game.sendCheckRelays(m.piece->colour);
                    if (getInCheck()) { // Ensures the intermediate squares do not put King in check
                        throw invalid_argument("");
                    }
                    removePiece(); // Removes temporary King placement
                    notifyObservers(m); 
                    break;
                }
            }
            break;

        case MessageType::CastleRookRelay: // Searches for the Rook to move in a Castle
            switch(status) {
                case Status::Error:
                    throw invalid_argument("");
                
                case Status::Success: { // If we find our desired Rook to move
                    Direction opp = getOppositeDirection(m.direction); // Reverse our reply direction
                    int nCol = col == 0 ? col + 3 : col - 2; // Set the appropriate destination for the Rook
                    Move mv = Move{m.move.player, row, col, 
                                    row, nCol, m.move.turn, m.move.promoteTo};
                    auto rook = getPiece(); // Retrieve the Rook on 'this' Square
                    rook->hasMoved = true; 
                    // Send a Reply message to move the Rook
                    notifyObservers(Message{MessageType::CastleRookReply, opp, mv, move(rook)});
                    removePiece(); // Remove Rook from original position
                    break;
                }
                case Status::Continue: // Continue search for Rook
                    notifyObservers(m);
                    break;
            }
            break;

        case MessageType::CastleRookReply: 
            switch(status) {
                case Status::Error:
                    throw invalid_argument("");
                
                case Status::Success: // Add Rook to new position
                    addPiece(m.piece->clone());
                    break;
                case Status::Continue: // "Relay" the Reply message over to the next Square
                    notifyObservers(m);
                    break;
            }
            break;

        case MessageType::CheckRelay: { // Sees if the King is in check
            Colour colour = piece->colour;
            if(colour == Colour::NoColour) { // Relay the message on
                notifyObservers(m);
            }
            else if(colour != m.piece->colour) { // If 'this' Square contains an enemy piece
                Move mv = Move{m.piece->colour, row, col, m.move.startRow, m.move.startCol, m.move.turn, m.move.promoteTo};
                if(piece->isPotentialCapture(mv)) { // If enemy can "caputre" king
                    preventCheck = true;
                    notifyObservers(Message{MessageType::CheckReply, getOppositeDirection(m.direction), mv, getPiece()});
                } else if (piece->type == PieceType::Pawn) { // If enemy is a pawn
                    mv.promoteTo = PieceType::Queen;
                    if (piece->isPotentialCapture(mv)) {
                        preventCheck = true;
                    notifyObservers(Message{MessageType::CheckReply, getOppositeDirection(m.direction), mv, getPiece()});
                    }
                }
            }
            break;
        }
            

        case MessageType::CheckReply: // Setting all subsequent squares appropriately
            if(end) {
                inCheck = true; // Set the King's inCheck value
            }
            else {
                preventCheck = true; // Set the intermediate squares' preventCheck values
                notifyObservers(m);
            }
            break;
    }
}


Info Square::getInfo() const {
    return Info{row, col};
}
