#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <memory>
#include <string>
#include <sstream>
#include "info.h"
#include "message.h"
#include "observer.h"
#include "move.h"
#include "enums.h"
#include "piece.h"
#include "king.h"
#include "queen.h"
#include "rook.h"
#include "bishop.h"
#include "knight.h"
#include "pawn.h"
#include "none.h"
#include "player.h"
#include "computer1.h"
#include "computer2.h"
#include "computer3.h"
#include "computer4.h"
#include "graphicsdisplay.h"
#include "textdisplay.h"
#include "gamestate.h"

using namespace std;

unique_ptr<Player> getPlayer(string option) {
    if (option == "computer1") {
        return make_unique<Computer1>();
    } else if (option == "computer2") {
        return make_unique<Computer2>();
    } else if (option == "computer3") {
        return make_unique<Computer3>();
    } else if (option == "computer4") {
        return make_unique<Computer4>();
    }
    return make_unique<Player>();
}

Colour getOppositeColour(Colour colour) {
    return colour == Colour::White ? Colour::Black : Colour::White;
}

// handle all end-of-game related things, including updating scores and printing winners
void updateOutcome(Colour colour, float &whiteScore, float &blackScore, bool resigned=false) {
    if (colour == Colour::White) {
        if (resigned) {
            cout << "White wins!" << endl;
        } else {
            cout << "Checkmate! White wins!" << endl;
        }
        whiteScore++;
    } else if (colour == Colour::Black) {
        if (resigned) {
            cout << "Black wins!" << endl;
        } else {
            cout << "Checkmate! Black wins!" << endl;
        }
        blackScore++;
    } else {
        cout << "Stalemate!" << endl;
        whiteScore += 0.5;
        blackScore += 0.5;
    }
}

// convert "a1" to row=0, col=0, for example
void parseSquares(string square, int &row, int &col) {
    if (square.length() < 2) {
        throw invalid_argument("");
    }
    //ascii conversion
    col = square[0] - 'a';
    row = square[1] - '1';
}

// parse and return a piece
unique_ptr<Piece> getPiece(string option) {
    if (option.length() < 1) {
        throw invalid_argument("");
    }
    char c = option[0];
    Colour colour = (c >= 'A' && c <= 'Z') ? Colour::White : Colour::Black;
    if (colour == Colour::Black) {
        //convert to upper case
        c += ('A' - 'a');
    }
    switch (c) {
        case 'K': return make_unique<King>(colour);
        case 'Q': return make_unique<Queen>(colour);
        case 'R': return make_unique<Rook>(colour);
        case 'B': return make_unique<Bishop>(colour);
        case 'N': return make_unique<Knight>(colour);
        case 'P': return make_unique<Pawn>(colour);
        default: throw invalid_argument("");
    }
}

enum Mode { InGame, GameOver, Setup };

int main() {
    string line;
    float whiteScore = 0, blackScore = 0;
    int turn = 1;
    unique_ptr<GameState> g;
    shared_ptr<TextDisplay> tptr = make_shared<TextDisplay>(8);
    shared_ptr<GraphicsDisplay> gptr = make_shared<GraphicsDisplay>(8);
    unique_ptr<Player> white;
    unique_ptr<Player> black;
    Colour nextMove = Colour::White;
    Mode mode = GameOver;
    bool wasSetup = false;
    while (getline(cin, line)) {
        istringstream ls(line);
        string option;
        ls >> option;
        if (option == "game" && mode == GameOver) {
            // start a new game, possibly with the setup state
            string whitePlayer, blackPlayer;
            ls >> whitePlayer >> blackPlayer;
            white = getPlayer(whitePlayer);
            black = getPlayer(blackPlayer);
            if (!wasSetup) {
                g = make_unique<GameState>();
                g->attachObserver(tptr);
                g->attachObserver(gptr);
                nextMove = Colour::White;
            }
            wasSetup = false;
            turn = 1;
            mode = InGame;
            cout << *tptr;
            gptr->commit();
        } else if (option == "resign" && mode == InGame) {
            // resign a game
            updateOutcome(getOppositeColour(nextMove), whiteScore, blackScore, true);
            mode = GameOver;
        } else if (option == "move" && mode == InGame) {
            // make or parse a move
            Move m{nextMove, -1, -1, -1, -1, turn, PieceType::NoneType};
            try {
                if (nextMove == Colour::White) {
                    m = white->getMove(*g->clone(), ls, nextMove, turn);
                } else {
                    m = black->getMove(*g->clone(), ls, nextMove, turn);
                }
            } catch (ios::failure&) {
                continue;
            }
            if (m.player != nextMove || m.turn != turn) {
                continue;
            }
            // test legality of the move and update the results
            MoveResult result = g->move(m);
            if (result == MoveResult::Checkmate) {
                updateOutcome(nextMove, whiteScore, blackScore);
                mode = GameOver;
            } else if (result == MoveResult::Stalemate) {
                updateOutcome(Colour::NoColour, whiteScore, blackScore);
                mode = GameOver;
            } else if (result == MoveResult::Check) {
                if (nextMove == Colour::White) {
                    cout << "Black is in check." << endl;
                } else {
                    cout << "White is in check." << endl;
                }
            }
            if (result != MoveResult::Error) {
                nextMove = getOppositeColour(nextMove);
                turn++;
                cout << *tptr;
                gptr->commit();
            }
        } else if (option == "undo" && mode == InGame) {
            // undo the last n moves
            int numUndos = 1;
            try {
                ls >> numUndos;
            } catch (ios::failure&) {
                continue;
            }
            if (numUndos > 0 && numUndos < turn) {
                for (int i = 0; i < numUndos; ++i) {
                    g->undoLast();
                }
                turn -= numUndos;
                if (numUndos % 2) {
                    nextMove = getOppositeColour(nextMove);
                }
                cout << *tptr;
                gptr->commit();
            }
        } else if (option == "history" && (mode == InGame || mode == GameOver)) {
            // print the current game's history
            if (!g) {
                continue;
            }
            g->printHistory(cout);
        } else if (option == "suggest" && mode == InGame) {
            // suggest a possible move by using Computer4
            cout << Computer4().getMove(*g->clone(), ls, nextMove, turn);
        } else if (option == "setup" && mode == GameOver) {
            // enter setup mode with the default board
            g = make_unique<GameState>();
            g->attachObserver(tptr);
            g->attachObserver(gptr);
            nextMove = Colour::White;
            wasSetup = true;
            mode = Setup;
            cout << *tptr;
            gptr->commit();
        } else if (mode == Setup) {
            if (option == "+") {
                // parse and add a piece
                try {
                    string piece, square;
                    ls >> piece >> square;
                    int row = 0, col = 0;
                    parseSquares(square, row, col);
                    g->addPiece(getPiece(piece), row, col);
                    cout << *tptr;
                    gptr->commit();
                } catch (invalid_argument&) {} catch (ios::failure&) {}
            } else if (option == "-") {
                // remove a piece
                try {
                    string square;
                    ls >> square;
                    int row = 0, col = 0;
                    parseSquares(square, row, col);
                    g->removePiece(row, col);
                    cout << *tptr;
                    gptr->commit();
                } catch (invalid_argument&) {} catch (ios::failure&) {}
            } else if (option == "=") {
                // set the player to move to white or black
                try {
                    string c;
                    ls >> c;
                    nextMove = c == "white" ? Colour::White : 
                        (c == "black" ? Colour::Black : nextMove);
                } catch (ios::failure&) {}
            } else if (option == "clear") {
                // remove all pieces
                for (int i = 0; i < 8; ++i) {
                    for (int j = 0; j < 8; ++j) {
                        g->removePiece(i, j);
                    }
                }
                cout << *tptr;
                gptr->commit();
            } else if (option == "done") {
                // finish setting up if the board is valid
                if (g->isValidBoard()) {
                    mode = GameOver;
                }
            }
        }
    }
    cout << "Final Score:" << endl;
    cout << setprecision(1);
    cout << "White: " << whiteScore << endl;
    cout << "Black: " << blackScore << endl;
}
