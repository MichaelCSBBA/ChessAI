#include <memory>
#include <vector>
#include "piece.h"
#include "graphicsdisplay.h"
#include "observer.h"
#include "subject.h"
#include "info.h"
#include "message.h"
#include "window.h"

using namespace std;

GraphicsDisplay::GraphicsDisplay(int n) : gridSize{n}, window{make_unique<Xwindow>()}, 
        pieceChanges{vector<PieceChange>()} {
    window->fillRectangle(0, 0, 500, 500, Xwindow::Burlywood);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if ((i + j) % 2) {
                drawRectangle(i, j);
            }
        }
    }
}

void GraphicsDisplay::drawRectangle(int row, int col) {
    float gridLength = 500.0 / gridSize;
    int x = int(gridLength * col);
    int y = 500 - int(gridLength * row);
    int s = int(gridLength);
    int c = (row + col) % 2 ? Xwindow::Burlywood : Xwindow::Sienna;
    window->fillRectangle(x, y-gridLength, s, s, c);
}

void GraphicsDisplay::drawPiece(int row, int col, const unique_ptr<Piece> &piece) {
    if (piece->type == PieceType::NoneType) {
        drawRectangle(row, col);
    } else {
        drawRectangle(row, col);
        float gridLength = 500.0 / gridSize;
        int x = int(gridLength * col) + 25;
        int y = 500 - int(gridLength * row) + 35;
        int c = piece->colour == Colour::White ? Xwindow::White : Xwindow::Black;
        window->drawString(x, y-gridLength, string(1, piece->getChar()), c);
    }
}

// just place the display's desired operations into a vector, don't update the display yet
// since the GameState will undergo lots of temporary changes
void GraphicsDisplay::notify(Subject<Info, Message> &whoFrom, const Message &m) {
    if (m.type == MessageType::PieceChanged) {
        Info info = whoFrom.getInfo();
        pieceChanges.emplace_back(PieceChange{info.row, info.col, m.piece->clone()});
    }
}

// actually update the graphics display by reading in from the vector
void GraphicsDisplay::commit() {
    for (auto &change : pieceChanges) {
        drawPiece(change.row, change.col, change.piece);
    }
    pieceChanges.empty();
}
