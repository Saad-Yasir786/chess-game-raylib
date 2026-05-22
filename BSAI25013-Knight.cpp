#include "BSAI25013-Knight.h"
#include "BSAI25013-Board.h"
#include <cmath>

Knight::Knight(pos Currpos, COLOR c) : Piece(Currpos, c) {}

char Knight::getSymbol() const { return PieceCLR == CLRWHITE ? 'N' : 'n'; }

Piece* Knight::clone() const
{
    Knight* n = new Knight(Currentposition, PieceCLR);
    n->SetHasMoved(hasMoved);
    return n;
}

bool Knight::isValidMove(pos end, const Board& board) const
{
    pos s = Currentposition;
    int dr = abs(s.r - end.r), dc = abs(s.c - end.c);
    if (!((dr == 2 && dc == 1) || (dr == 1 && dc == 2))) return false;
    return Piece::isValidMove(end, board);
}

void Knight::Draw_piece(Texture2D tex) const
{
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { (float)(Currentposition.c * 100 + 4), (float)(Currentposition.r * 100 + 4), 92, 92 };
    DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
}