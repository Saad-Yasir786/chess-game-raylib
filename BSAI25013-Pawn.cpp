#include "BSAI25013-Pawn.h"
#include "BSAI25013-Board.h"
#include <cmath>

Pawn::Pawn(pos Currpos, COLOR c) : Piece(Currpos, c), isFirst(true) {}

void Pawn::move(pos dest)
{
    isFirst = false;
    Currentposition = dest;
    hasMoved = true;
}

char Pawn::getSymbol() const { return PieceCLR == CLRWHITE ? 'P' : 'p'; }

Piece* Pawn::clone() const
{
    Pawn* n = new Pawn(Currentposition, PieceCLR);
    n->isFirst = isFirst;
    n->hasMoved = hasMoved;
    return n;
}

bool Pawn::isValidMove(pos end, const Board& board) const
{
    pos s = Currentposition;
    int dir = (PieceCLR == CLRWHITE) ? -1 : 1;
    const Cell* dst = board.getCell(end.r, end.c);
    if (!dst) return false;

    if (end.c == s.c && dst->isFree())
    {
        if (end.r == s.r + dir) return true;
        if (isFirst && end.r == s.r + 2 * dir)
        {
            if (board.getCell(s.r + dir, s.c)->isFree()) return true;
        }
    }
    if (abs(end.c - s.c) == 1 && end.r == s.r + dir)
    {
        if (!dst->isFree() && dst->GetPiece()->GetCOLOR() != PieceCLR) return true;
        if (dst->isFree() && board.canEnPassant() && end.r == board.getEnPassantTarget().r && end.c == board.getEnPassantTarget().c)
        {
            pos capPos = { s.r, end.c };
            Piece* capPiece = board.pieceAt(capPos);
            if (capPiece && capPiece->GetCOLOR() != PieceCLR)
            {
                char capSym = capPiece->getSymbol();
                if (capSym == 'P' || capSym == 'p') return true;
            }
        }
    }
    return false;
}

void Pawn::Draw_piece(Texture2D tex) const
{
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { (float)(Currentposition.c * 100 + 4), (float)(Currentposition.r * 100 + 4), 92, 92 };
    DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
}
