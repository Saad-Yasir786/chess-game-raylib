#include "BSAI25013-King.h"
#include "BSAI25013-Board.h"
#include <cmath>

King::King(pos Currpos, COLOR c) : Piece(Currpos, c) {}

char King::getSymbol() const { return PieceCLR == CLRWHITE ? 'K' : 'k'; }

Piece* King::clone() const
{
    King* n = new King(Currentposition, PieceCLR);
    n->SetHasMoved(hasMoved);
    return n;
}

bool King::isValidMove(pos end, const Board& board) const
{
    pos s = Currentposition;
    int dr = abs(s.r - end.r), dc = abs(s.c - end.c);
    if (dr == 0 && dc == 2) return true;
    if (dr > 1 || dc > 1 || (dr == 0 && dc == 0)) return false;
    return Piece::isValidMove(end, board);
}

void King::Draw_piece(Texture2D tex) const
{
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { (float)(Currentposition.c * 100 + 4), (float)(Currentposition.r * 100 + 4), 92, 92 };
    DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
}
