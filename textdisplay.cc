#include <vector>
#include <memory>
#include <iostream>
#include "piece.h"
#include "textdisplay.h"
#include "observer.h"
#include "subject.h"
#include "info.h"
#include "message.h"
#include "window.h"

using namespace std;

TextDisplay::TextDisplay(int n) : n{n}, board{vector<vector<char>>(n, vector<char>(n, ' '))} {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            board[i][j] = getEmptySquareChar(i, j);
        }
    }
}

char TextDisplay::getEmptySquareChar(int row, int col) {
    return (row + col) % 2 ? ' ' : '_';
}

void TextDisplay::notify(Subject<Info, Message> &whoFrom, const Message &m) {
    if (m.type == MessageType::PieceChanged) {
        Info info = whoFrom.getInfo();
        if (m.piece->type == PieceType::NoneType) {
            board[info.row][info.col] = getEmptySquareChar(info.row, info.col);
        } else {
            board[info.row][info.col] = m.piece->getChar();
        }
    }
}

ostream &operator<<(ostream &out, const TextDisplay &td) {
    int rowNumber = td.n;
    for (auto row = td.board.rbegin(); row != td.board.rend(); ++row) {
        out << rowNumber << " ";
        for (auto c : *row) {
            out << c;
        }
        out << endl;
        rowNumber--;
    }
    out << endl;
    out << "  abcdefgh" << endl;
    return out;
}
