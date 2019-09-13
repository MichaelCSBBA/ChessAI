#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H
#include <vector>
#include <memory>
#include <iostream>
#include "observer.h"
#include "info.h"
#include "enums.h"
#include "message.h"

class TextDisplay : public Observer<Info, Message> {
    const int n;
    std::vector<std::vector<char>> board;
    char getEmptySquareChar(int row, int col);
    public:
        TextDisplay(int n);
        void notify(Subject<Info, Message> &whoFrom, const Message &m) override;
        friend std::ostream &operator<<(std::ostream &out, const TextDisplay &td);
};
#endif
