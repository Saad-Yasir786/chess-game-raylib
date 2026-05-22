#pragma once
#include "BSAI25013-Helper.h"

class Board;
class Cell;

class Piece {
protected:
    pos Currentposition;
    COLOR PieceCLR;
    bool hasMoved;

public:
    Piece(pos Start_position, COLOR CLR);
    virtual ~Piece() {}
    virtual bool isValidMove(pos end, const Board& board) const;
    virtual void Draw_piece(Texture2D tex) const = 0;
    virtual char getSymbol() const = 0;
    virtual Piece* clone() const = 0;

    COLOR GetCOLOR() const;
    pos GetPosition() const;
    bool GetHasMoved() const;
    void SetHasMoved(bool v);

    virtual void move(pos dest);

    static bool isHorizontalPathClear(pos s, pos d, const Board& board);
    static bool isVerticalPathClear(pos s, pos d, const Board& board);
    static bool isDiagonalPathClear(pos s, pos d, const Board& board);
};