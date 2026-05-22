#include "BSAI25013-Piece.h"
#include "BSAI25013-Board.h"
#include "BSAI25013-Cell.h"

using namespace std;

Piece::Piece(pos Start_position, COLOR CLR) : Currentposition(Start_position), PieceCLR(CLR), hasMoved(false) {}

bool Piece::isValidMove(pos end, const Board& board) const
{
	const Cell* dst = board.getCell(end.r, end.c);
	if (!dst)
		return false;
	if (dst->isFree())
		return true;
	if (dst->GetPiece()->GetCOLOR() != PieceCLR)
		return true;
	return false;
}

void Piece::move(pos dest)
{
	Currentposition = dest;
	hasMoved = true;
}

COLOR Piece::GetCOLOR() const { return PieceCLR; }
pos Piece::GetPosition() const { return Currentposition; }
bool Piece::GetHasMoved() const { return hasMoved; }
void Piece::SetHasMoved(bool v) { hasMoved = v; }

bool Piece::isHorizontalPathClear(pos s, pos d, const Board& board)
{
	int start = (s.c < d.c) ? s.c : d.c, end = (s.c > d.c) ? s.c : d.c;
	for (int c = start + 1; c < end; c++)
	{
		if (!board.getCell(s.r, c)->isFree())
		{
			return false;
		}
	}
	return true;
}

bool Piece::isVerticalPathClear(pos s, pos d, const Board& board)
{
	int start = (s.r < d.r) ? s.r : d.r;
	int end = (s.r > d.r) ? s.r : d.r;
	for (int r = start + 1; r < end; r++)
	{
		if (!board.getCell(r, s.c)->isFree()) 
		{
			return false;
		}
	}
	return true;
}

bool Piece::isDiagonalPathClear(pos s, pos d, const Board& board)
{
	int dr = (d.r > s.r) ? 1 : -1, dc = (d.c > s.c) ? 1 : -1;
	int r = s.r + dr, c = s.c + dc;
	while (r != d.r and c != d.c)
	{
		if (!board.getCell(r, c)->isFree())
		{
			return false;
		}
		r += dr;
		c += dc;
	}
	return true;
}