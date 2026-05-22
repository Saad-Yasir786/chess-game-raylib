#pragma once
#include "BSAI25013-Piece.h"

class Cell {
    pos BoardPosition;
    pos ScreenPosition;
    Piece* pieceptr;
    int cellH, cellW;
public:
    Cell();
    void SetPosition(int r, int c, int x, int y, int Cellh, int Cellw);
    bool isFree() const;
    Piece* GetPiece() const;
    void Changepiece(Piece* p);
    bool isInside(int clickr, int clickc) const;
    pos GetScreenPos() const;
    pos GetBoardPos() const;
    int GetCellW() const;
    int GetCellH() const;
};