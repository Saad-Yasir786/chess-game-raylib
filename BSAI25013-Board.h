#pragma once
#include "BSAI25013-Cell.h"
#include "BSAI25013-Helper.h"
#include "raylib.h"
#include <vector>
#include <string>

using namespace std;

struct Move { pos src; pos dst; };

enum AppState { MENU, PLAYING, GAME_OVER, REPLAYING };

class Board {
    Cell B[8][8];
    pos BoardSize;
    COLOR turn;
    Texture2D pieceTextures[12];
    bool epValid;
    pos epTarget;
    bool wKM, bKM, wRLM, wRRM, bRLM, bRRM;

    vector<Move> moveHistory;
    size_t currentIdx;

    Texture2D pieceTex;
    Sound moveSound, captureSound;
    AppState appState;
    Cell* sel;
    bool legal[8][8];
    bool capMap[8][8];
    string status;
    string p1, p2;
    bool needPromo;
    pos promoPos;
    int replayIdx;
    vector<Move> replayHistory;

    void clearSel();
    void buildCapMap(pos s);
    void checkEndState();
    Rectangle getPieceRect(char sym) const;
    void drawBoard() const;
    void drawMenu() const;
    void drawGameOver() const;
    void handlePlayingInput();
    void handleMenuInput();
    void handleReplayInput();

    void resetBoardToInitial();
    void setBoardTo(size_t idx);

public:
    Board(int SH, int SW);
    ~Board();

    void RunGame();
    void InitBoard();

    Cell* ClickonWhichcell(int x, int y);
    const Cell* getCell(int r, int c) const;
    Cell* getCell(int r, int c);
    void Printboard() const;
    void changeTurn();
    COLOR getTurn() const;
    bool isValidSource(pos p) const;
    bool isValidDestination(pos src, pos dst) const;
    Piece* pieceAt(pos p) const;
    void update(pos src, pos dst);

    bool canEnPassant() const;
    pos getEnPassantTarget() const;

    bool isSquareAttacked(pos p, COLOR byCOLOR) const;
    bool isInCheck(COLOR c) const;
    bool wouldLeaveInCheck(pos s, pos d);
    bool canCastle(pos s, pos d) const;
    bool isCheckmate(COLOR c);
    bool isStalemate(COLOR c);
    bool hasAnyLegalMove(COLOR c);

    void getLegalMoves(pos s, bool out[8][8]);
    bool executeMove(pos s, pos d, bool recordMove = true);
    void promotePawn(pos p, char choice);

    bool hasUndo() const;
    bool hasRedo() const;
    void undo();
    void redo();

    void saveToFile(const string& fn, const string& pp1, const string& pp2) const;
    bool loadFromFile(const string& fn, string& pp1, string& pp2);

    const vector<Move>& getMoveHistory() const;
    void replayInit();
    void replayStep(int idx);

    Piece* makePieceFromChar(char ch, pos p) const;
};