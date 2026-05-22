#pragma once
#include "BSAI25013-Piece.h"

class Pawn : public Piece {
    bool isFirst;
public:
    Pawn(pos Currpos, COLOR c);
    bool isValidMove(pos end, const Board& board) const override;
    void Draw_piece(Texture2D tex) const override;
    void move(pos dest) override;
    char getSymbol() const override;
    Piece* clone() const override;
};