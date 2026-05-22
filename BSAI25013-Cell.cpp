#include "BSAI25013-Cell.h"

Cell::Cell() : pieceptr(nullptr) {}

void Cell::SetPosition(int r, int c, int x, int y, int Cellh, int Cellw)
{
    BoardPosition = { r, c };
    ScreenPosition = { x, y };
    this->cellH = Cellh;
    this->cellW = Cellw;
}

bool Cell::isFree() const { return pieceptr == nullptr; }
Piece* Cell::GetPiece() const { return pieceptr; }
void Cell::Changepiece(Piece* p) { this->pieceptr = p; }

bool Cell::isInside(int x, int y) const
{
    return (x >= ScreenPosition.c and x <= ScreenPosition.c + cellW and y >= ScreenPosition.r and y <= ScreenPosition.r + cellH);
}

pos Cell::GetScreenPos() const { return ScreenPosition; }
pos Cell::GetBoardPos() const { return BoardPosition; }
int Cell::GetCellW() const { return cellW; }
int Cell::GetCellH() const { return cellH; }