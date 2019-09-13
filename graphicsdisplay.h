#ifndef GRAPHICSDISPLAY_H
#define GRAPHICSDISPLAY_H
#include <memory>
#include <vector>
#include "window.h"
#include "observer.h"
#include "info.h"
#include "enums.h"
#include "message.h"

class GraphicsDisplay: public Observer<Info, Message> {
  const int gridSize;
  std::unique_ptr<Xwindow> window;
  struct PieceChange {
    const int row, col;
    std::unique_ptr<Piece> piece;
  };
  std::vector<PieceChange> pieceChanges;
  void drawRectangle(int row, int col);
  void drawPiece(int row, int col, const std::unique_ptr<Piece> &piece);
  int getColour(Colour colour);
  public:
    GraphicsDisplay(int n);
    void notify(Subject<Info, Message> &whoFrom, const Message &m) override;
    void commit();
};
#endif
