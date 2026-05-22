#pragma once
#include "BSAI25013-Piece.h"

class Queen : public Piece {
public:
    Queen(pos Currpos, COLOR c);
    bool isValidMove(pos end, const Board& board) const override;
    void Draw_piece(Texture2D tex) const override;
    char getSymbol() const override;
    Piece* clone() const override;
};