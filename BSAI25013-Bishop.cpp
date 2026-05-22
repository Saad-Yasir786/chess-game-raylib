#include "BSAI25013-Bishop.h"
#include "BSAI25013-Board.h"
#include <cmath>

Bishop::Bishop(pos Currpos, COLOR c) : Piece(Currpos, c) 
{
}
char Bishop::getSymbol() const { 
return PieceCLR == CLRWHITE ? 'B' : 'b'; }

Piece* Bishop::clone() const 
{ 
    Bishop* n = new Bishop(Currentposition, PieceCLR); 
n->SetHasMoved(hasMoved); 
    return n; 
}

bool Bishop::isValidMove(pos end, const Board& board) const
{
pos s = Currentposition;
    if (abs(s.r-end.r) != abs(s.c-end.c)) return false;
    if (!Piece::isValidMove(end, board)) 
    {
return false;
    }
    return isDiagonalPathClear(s, end, board);
}

void Bishop::Draw_piece(Texture2D tex) const
{
Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { (float)(Currentposition.c * 100 + 4), (float)(Currentposition.r * 100 + 4), 92, 92 };
DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
}