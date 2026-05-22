#include "BSAI25013-Rook.h"
#include "BSAI25013-Board.h"

Rook::Rook(pos p, COLOR c) : Piece(p, c) {}

char Rook::getSymbol() const { return PieceCLR == CLRWHITE ? 'R' : 'r'; }

Piece* Rook::clone() const
{
    Rook* n = new Rook(Currentposition, PieceCLR);
    n->SetHasMoved(hasMoved);
    return n;
}

bool Rook::isValidMove(pos end, const Board& board) const
{
    pos s = Currentposition;
    if (s.r != end.r && s.c != end.c) return false;
    if (!Piece::isValidMove(end, board)) return false;
    if (s.r == end.r) return isHorizontalPathClear(s, end, board);
    return isVerticalPathClear(s, end, board);
}

void Rook::Draw_piece(Texture2D tex) const
{
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { (float)(Currentposition.c * 100 + 4), (float)(Currentposition.r * 100 + 4), 92, 92 };
    DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
}