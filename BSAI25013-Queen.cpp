#include "BSAI25013-Queen.h"
#include "BSAI25013-Board.h"
#include <cmath>

Queen::Queen(pos Currpos, COLOR c) : Piece(Currpos, c) {}

char Queen::getSymbol() const { return PieceCLR == CLRWHITE ? 'Q' : 'q'; }

Piece* Queen::clone() const
{
    Queen* n = new Queen(Currentposition, PieceCLR);
    n->SetHasMoved(hasMoved);
    return n;
}

bool Queen::isValidMove(pos end, const Board& board) const
{
    pos s = Currentposition;
    bool horiz = s.r == end.r && s.c != end.c;
    bool vert = s.c == end.c && s.r != end.r;
    bool diag = abs(s.r - end.r) == abs(s.c - end.c) && s.r != end.r;
    if (!horiz && !vert && !diag) return false;
    if (!Piece::isValidMove(end, board)) return false;
    if (horiz) return isHorizontalPathClear(s, end, board);
    if (vert) return isVerticalPathClear(s, end, board);
    return isDiagonalPathClear(s, end, board);
}

void Queen::Draw_piece(Texture2D tex) const
{
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { (float)(Currentposition.c * 100 + 4), (float)(Currentposition.r * 100 + 4), 92, 92 };
    DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
}