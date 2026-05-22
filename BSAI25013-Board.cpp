#include "BSAI25013-Board.h"
#include "BSAI25013-Pawn.h"
#include "BSAI25013-Rook.h"
#include "BSAI25013-Knight.h"
#include "BSAI25013-Bishop.h"
#include "BSAI25013-Queen.h"
#include "BSAI25013-King.h"
#include <fstream>
#include <cmath>
#include <iostream>

using namespace std;

Board::Board(int SH, int SW)
    : turn(CLRWHITE), epValid(false), epTarget({ -1,-1 }),wKM(false), bKM(false), wRLM(false), wRRM(false), bRLM(false), bRRM(false),
    appState(MENU), sel(nullptr), status("NORMAL"),
    p1("Player1"), p2("Player2"), needPromo(false), promoPos({ 0,0 }), replayIdx(0),
    currentIdx(0)
{
    BoardSize = { SH, SW };
    int cH = SH / 8, cW = SW / 8;
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++) {
            B[r][c].SetPosition(r, c, c * cW, r * cH, cH, cW);
            B[r][c].Changepiece(nullptr);
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) { legal[i][j] = false; capMap[i][j] = false; }
    }
    pieceTextures[0] = LoadTexture("BSAI25013-Assets/w_pawn.png");
    pieceTextures[1] = LoadTexture("BSAI25013-Assets/w_rook.png");
    pieceTextures[2] = LoadTexture("BSAI25013-Assets/w_knight.png");
    pieceTextures[3] = LoadTexture("BSAI25013-Assets/w_bishop.png");
    pieceTextures[4] = LoadTexture("BSAI25013-Assets/w_king.png");
    pieceTextures[5] = LoadTexture("BSAI25013-Assets/w_queen.png");
    pieceTextures[6] = LoadTexture("BSAI25013-Assets/b_pawn.png");
    pieceTextures[7] = LoadTexture("BSAI25013-Assets/b_rook.png");
    pieceTextures[8] = LoadTexture("BSAI25013-Assets/b_knight.png");
    pieceTextures[9] = LoadTexture("BSAI25013-Assets/b_bishop.png");
    pieceTextures[10] = LoadTexture("BSAI25013-Assets/b_king.png");
    pieceTextures[11] = LoadTexture("BSAI25013-Assets/b_queen.png");
    moveSound = LoadSound("BSAI25013-Assets/move.wav");
    captureSound = LoadSound("BSAI25013-Assets/capture.wav");
    resetBoardToInitial();
}



Board::~Board()
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (!B[r][c].isFree()) 
            {
                delete B[r][c].GetPiece();

                B[r][c].Changepiece(nullptr);
            }
        }
    }
    for (int i = 0; i < 12; i++)
    {
        UnloadTexture(pieceTextures[i]);
    }


    UnloadSound(moveSound);
    UnloadSound(captureSound);
}

void Board::resetBoardToInitial()
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (!B[r][c].isFree()) {
                delete B[r][c].GetPiece();
                B[r][c].Changepiece(nullptr);
            }
        }
    }
    for (int i = 0; i < 8; i++)
    {
        B[1][i].Changepiece(new Pawn({ 1,i }, CLRBLACK));
        B[6][i].Changepiece(new Pawn({ 6,i }, CLRWHITE));
    }
    B[0][0].Changepiece(new Rook({ 0,0 }, CLRBLACK));   B[0][7].Changepiece(new Rook({ 0,7 }, CLRBLACK));
    B[7][0].Changepiece(new Rook({ 7,0 }, CLRWHITE));   B[7][7].Changepiece(new Rook({ 7,7 }, CLRWHITE));
    B[0][1].Changepiece(new Knight({ 0,1 }, CLRBLACK)); B[0][6].Changepiece(new Knight({ 0,6 }, CLRBLACK));
    B[7][1].Changepiece(new Knight({ 7,1 }, CLRWHITE)); B[7][6].Changepiece(new Knight({ 7,6 }, CLRWHITE));
    B[0][2].Changepiece(new Bishop({ 0,2 }, CLRBLACK)); B[0][5].Changepiece(new Bishop({ 0,5 }, CLRBLACK));
    B[7][2].Changepiece(new Bishop({ 7,2 }, CLRWHITE)); B[7][5].Changepiece(new Bishop({ 7,5 }, CLRWHITE));
    B[0][3].Changepiece(new Queen({ 0,3 }, CLRBLACK));  B[7][3].Changepiece(new Queen({ 7,3 }, CLRWHITE));
    B[0][4].Changepiece(new King({ 0,4 }, CLRBLACK));   B[7][4].Changepiece(new King({ 7,4 }, CLRWHITE));
    turn = CLRWHITE;
    epValid = false;
    epTarget = { -1, -1 };
    wKM = bKM = wRLM = wRRM = bRLM = bRRM = false;
}

void Board::RunGame()
{
    while (!WindowShouldClose())
    {
        switch (appState) 
        {
        case MENU:
        {
            handleMenuInput();
            break;
        }
        case PLAYING:
            handlePlayingInput();
            break;
        case GAME_OVER:
            if (IsKeyPressed(KEY_U)) 
            {
                undo();
                clearSel();
                status = "NORMAL";
                appState = PLAYING;
            }
            if (IsKeyPressed(KEY_M)) {
                appState = MENU;
                clearSel();
            }
            break;
        case REPLAYING:

            handleReplayInput();
            break;
        }

        BeginDrawing();
        Color MBlue = { 26, 26, 46, 255 };
        ClearBackground(MBlue);

        if (appState == MENU) drawMenu();
        else if (appState == REPLAYING) 
        {
            drawBoard();
        }
        else { drawBoard();
        if (appState == GAME_OVER) 
            drawGameOver(); }

        EndDrawing();
    }
}

void Board::handleMenuInput()
{
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
    {
        return;
    }
    Vector2 mp = GetMousePosition();
    if (mp.x > 275 && mp.x < 525 and mp.y>305 and mp.y < 360) {
        replayInit();
        clearSel(); status = "NORMAL"; appState = PLAYING;
    }
    if (mp.x > 275 and mp.x < 525 and mp.y>375 and mp.y < 430) {
        string lp1, lp2;
        if (loadFromFile("savegame.txt", lp1, lp2))
        {
            p1 = lp1; p2 = lp2; appState = PLAYING;
            status = "NORMAL";
        }
    }
    if (mp.x > 275 && mp.x < 525 and mp.y>445 && mp.y < 500)
    {
        string lp1, lp2;
        if (loadFromFile("savegame.txt", lp1, lp2)) {
            replayHistory = moveHistory;
            replayInit();
            replayIdx = 0;
            appState = REPLAYING;
        }



    }
}

void Board::handlePlayingInput()
{
    if (needPromo) {
        promotePawn(promoPos, 'Q');
        needPromo = false;
        checkEndState();
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mp = GetMousePosition();
        Cell* clicked = ClickonWhichcell(mp.x, mp.y);
        if (clicked) 
        {
            pos cp = clicked->GetBoardPos();
            if (sel and legal[cp.r][cp.c]) {
                if (currentIdx < moveHistory.size()) {
                    moveHistory.resize(currentIdx);
                }
                Piece* moving = pieceAt(sel->GetBoardPos());
                bool cap = pieceAt(cp) != nullptr;
                if (!cap && moving) 
                {
                    char sym = moving->getSymbol(), up = (sym >= 'a') ? (char)(sym - 32) : sym;
                    if (up == 'P' && epValid && cp.r == epTarget.r and cp.c == epTarget.c) {
                        cap = true;
                    }
                }
                executeMove(sel->GetBoardPos(), cp);
                if (cap) PlaySound(captureSound); 
                else PlaySound(moveSound);
                Piece* pp = pieceAt(cp);
                if (pp && (pp->getSymbol() == 'P' or pp->getSymbol() == 'p'))
                    if (cp.r == 0 or cp.r == 7)
                    {
                        needPromo = true;
                        promoPos = cp;
                    }
                clearSel();
                if (!needPromo) checkEndState();
            }
            else if (isValidSource(cp)) { clearSel(); 
            sel = clicked; 
            buildCapMap(cp); 
            }
            else clearSel();
        }
    }

    if (IsKeyPressed(KEY_U))
    {
        undo(); clearSel(); status = "NORMAL";
    }
    if (IsKeyPressed(KEY_R)) 
    { redo(); clearSel(); }
    if (IsKeyPressed(KEY_S)) saveToFile("savegame.txt", p1, p2);
    if (IsKeyPressed(KEY_M)) { appState = MENU; clearSel(); }
}

void Board::handleReplayInput()
{
    if (IsKeyPressed(KEY_RIGHT) or IsKeyPressed(KEY_SPACE)) {
        if (replayIdx < replayHistory.size()) {
            pos s = replayHistory[replayIdx].src, d = replayHistory[replayIdx].dst;
            executeMove(s, d, false);
            PlaySound(moveSound);
            replayIdx++;
        }
    }
    if (IsKeyPressed(KEY_LEFT)) {
        if (replayIdx > 0) {
            replayIdx--;
            resetBoardToInitial();
            for (int i = 0; i < replayIdx; i++) 
            {
                executeMove(replayHistory[i].src, replayHistory[i].dst, false);
            }
        }
    }
    if (IsKeyPressed(KEY_M)) { appState = MENU; clearSel(); }
}

Rectangle Board::getPieceRect(char sym) const
{
    int sW = pieceTex.width / 6, sH = pieceTex.height / 2, col = 5;
    char u = (sym >= 'a') ? (char)(sym - 32) : sym;
    if (u == 'K')
        col = 0;
    else if (u == 'Q') col = 1;
    else if (u == 'B') 
        col = 2;
    else if (u == 'N') col = 3;
    else if (u == 'R') col = 4;
    int row = (sym >= 'a') ? 0 : 1;
    return { (float)(col * sW),(float)(row * sH),(float)sW,(float)sH };
}

void Board::drawBoard() const
{
    int cH = BoardSize.r / 8, cW = BoardSize.c / 8;
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Color cellColor = ((r + c) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
            if (sel != nullptr and sel->GetBoardPos().r == r && sel->GetBoardPos().c == c)
                cellColor = YELLOW;
            else if (legal[r][c])
                cellColor = GREEN;
            else if (capMap[r][c])
                cellColor = RED;

            DrawRectangle(c * cW, r * cH, cW, cH, cellColor);
            Piece* p = B[r][c].GetPiece();
            if (p)
            {
                char sym = p->getSymbol();
                int idx = -1;
                if (sym == 'P') idx = 0; 
                else if (sym == 'R') idx = 1;
                else if (sym == 'N') idx = 2;
                else if (sym == 'B') idx = 3;
                else if (sym == 'Q') idx = 4; else if (sym == 'K') idx = 5;
                else if (sym == 'p') idx = 6; 
                else if (sym == 'r') idx = 7; else if (sym == 'n') idx = 8; 
                else if (sym == 'b') idx = 9;
                else if (sym == 'q') idx = 10; 
                else if (sym == 'k') idx = 11;
                if (idx != -1)
                {
                    p->Draw_piece(pieceTextures[idx]);
                }
            }
        }
    }
}
struct Btn { const char* lbl; int y; Color c; };

void Board::drawMenu() const
{
    int SW = BoardSize.c, SH = BoardSize.r;
    ClearBackground({ 255, 218, 221, 255 });
    DrawCircle(100, 100, 15, { 255, 182, 193, 200 });
    DrawCircle(SW - 120, SH - 80, 25, { 255, 192, 203, 180 });
    DrawCircle(SW / 2 + 150, 60, 10, { 255, 105, 180, 150 });
    DrawPoly({ 80, (float)SH - 100 }, 3, 30, 0, { 255, 182, 193, 200 });
    DrawText("CHESS", SW / 2 - 95, 100, 80, { 255, 105, 180, 255 });
    DrawText("BSAI25013", SW / 2 - 65, 190, 22, { 219, 112, 147, 255 });
    Btn btns[] = { {"NEW GAME",310,{ 255, 182, 193, 255 }},{"LOAD GAME",380,{ 255, 192, 203, 255 }},{"REPLAY",450,{ 255, 105, 180, 255 }} };
    Vector2 mp = GetMousePosition();
    for (auto& b : btns) {
        bool hov = (mp.x > 275 and mp.x<525 and mp.y>b.y && mp.y < b.y + 50);
        DrawRectangleRounded({ 275, (float)b.y, 250, 50 }, 0.3f, 8, { 219, 112, 147, 255 });
        DrawRectangleRounded({ 277, (float)b.y + 2, 246, 46 }, 0.3f, 8, RAYWHITE);
        int tw = MeasureText(b.lbl, 22);
        DrawText(b.lbl, 400 - tw / 2, b.y + 13, 22, hov ? Color{ 255, 105, 180, 255 } : WHITE);
    }
    DrawText("S=Save  U=Undo  R=Redo in game", SW / 2 - 150, SH - 35, 15, GRAY);
}

void Board::drawGameOver() const
{
    int SW = BoardSize.c, SH = BoardSize.r;
    DrawRectangle(0, 0, SW, SH, Fade(BLACK, 0.65f));
    DrawText("GAME OVER", SW / 2 - 140, SH / 2 - 55, 55, GOLD);
    DrawText(status.c_str(), SW / 2 - MeasureText(status.c_str(), 34) / 2, SH / 2 + 10, 34, RED);
    DrawText("U = Undo  |  M = Menu", SW / 2 - 105, SH / 2 + 65, 20, LIGHTGRAY);
}


void Board::clearSel()
{
    sel = nullptr;
    for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) { legal[i][j] = false; capMap[i][j] = false; }
}

void Board::buildCapMap(pos s)
{
    getLegalMoves(s, legal);
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++)
            capMap[r][c] = legal[r][c] && pieceAt({ r,c }) != nullptr;
    }
}

void Board::checkEndState()
{
    COLOR t = getTurn();
    if (isCheckmate(t)) { status = "CHECKMATE"; appState = GAME_OVER; }
    else if (isStalemate(t)) { status = "STALEMATE"; appState = GAME_OVER; }
    else if (isInCheck(t))  status = "CHECK";
    else status = "NORMAL";
}


Cell* Board::ClickonWhichcell(int x, int y)
{
    int cH = BoardSize.r / 8;
    int cW = BoardSize.c / 8, col = x / cW, row = y / cH;
    if (row >= 0 and row < 8 && col >= 0 and col < 8)
    {
        return &B[row][col];
    }
    return nullptr;
}

Cell* Board::getCell(int r, int c) { if (r >= 0 && r < 8 && c >= 0 and c < 8) 
    return &B[r][c]; 
return nullptr; }
const Cell* Board::getCell(int r, int c) const { if (r >= 0 && r < 8 and c >= 0 and c < 8)
    return &B[r][c];
return nullptr; }

Piece* Board::pieceAt(pos p) const { const Cell* c = getCell(p.r, p.c); 
return c ? c->GetPiece() : nullptr; }

void Board::update(pos src, pos dst)
{
    Cell* sc = getCell(src.r, src.c); Cell* dc = getCell(dst.r, dst.c);
    if (!dc->isFree()) { delete dc->GetPiece(); dc->Changepiece(nullptr); }
    Piece* p = sc->GetPiece(); sc->Changepiece(nullptr); p->move(dst); dc->Changepiece(p);
}

void Board::Printboard() const
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) cout << (B[i][j].isFree() ? '.' : B[i][j].GetPiece()->getSymbol());
        cout << endl;
    }
}

void Board::changeTurn() { turn = (turn == CLRWHITE) ? CLRBLACK : CLRWHITE; }
COLOR Board::getTurn() const { return turn; }
bool Board::canEnPassant() const { return epValid; }
pos Board::getEnPassantTarget() const { return epTarget; }

bool Board::isValidSource(pos p) const
{
    const Cell* c = getCell(p.r, p.c);
    if (!c or c->isFree()) return false;
    return c->GetPiece()->GetCOLOR() == turn;
}

bool Board::isValidDestination(pos src, pos dst) const
{
    const Cell* sc = getCell(src.r, src.c);
    const Cell* dc = getCell(dst.r, dst.c);
    if (!sc or !dc) return false;
    return sc->GetPiece()->isValidMove(dst, *this);
}

bool Board::isSquareAttacked(pos p, COLOR byCOLOR) const
{
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece* pc = B[r][c].GetPiece();
            if (!pc or pc->GetCOLOR() != byCOLOR) continue;
            char sym = pc->getSymbol();
            char up = (sym >= 'a' && sym <= 'z') ? (char)(sym - 32) : sym;
            if (up == 'K') { int dr = abs(r - p.r), dc2 = abs(c - p.c); if (dr <= 1 && dc2 <= 1 && !(dr == 0 && dc2 == 0)) return true; }
            else if (up == 'P') { int dir = (pc->GetCOLOR() == CLRWHITE) ? -1 : 1; if (p.r == r + dir && abs(p.c - c) == 1) return true; }
            else if (pc->isValidMove(p, *this)) return true;
        }
    }
    return false;
}

bool Board::isInCheck(COLOR c) const
{
    char kSym = (c == CLRWHITE) ? 'K' : 'k'; pos kp = { -1,-1 };
    for (int r = 0; r < 8; r++)
    {
        for (int col = 0; col < 8; col++)
            if (!B[r][col].isFree() && B[r][col].GetPiece()->getSymbol() == kSym) kp = { r,col };
    }
    if (kp.r == -1) return false;
    return isSquareAttacked(kp, (c == CLRWHITE) ? CLRBLACK : CLRWHITE);
}

bool Board::wouldLeaveInCheck(pos s, pos d)
{
    Piece* mv = B[s.r][s.c].GetPiece();
    if (!mv) return true;
    Piece* cap = B[d.r][d.c].GetPiece();
    bool oldMoved = mv->GetHasMoved();

    bool isEP = false;
    pos epCapPos = { -1, -1 };
    char sym = mv->getSymbol();
    if ((sym == 'P' or sym == 'p') and B[d.r][d.c].isFree() && canEnPassant() and d.r == epTarget.r and d.c == epTarget.c) {
        epCapPos.r = (mv->GetCOLOR() == CLRWHITE) ? d.r + 1 : d.r - 1;
        epCapPos.c = d.c;
        Piece* epVictim = B[epCapPos.r][epCapPos.c].GetPiece();
        if (epVictim && epVictim->GetCOLOR() != mv->GetCOLOR()) 
        {
            char victimSym = epVictim->getSymbol();
            if (victimSym == 'P' or victimSym == 'p') isEP = true;
        }
    }

    Piece* epCapPiece = isEP ? B[epCapPos.r][epCapPos.c].GetPiece() : nullptr;
    B[d.r][d.c].Changepiece(mv);
    B[s.r][s.c].Changepiece(nullptr);
    if (isEP) {
        B[epCapPos.r][epCapPos.c].Changepiece(nullptr);
    }
    pos op = mv->GetPosition();
    mv->Piece::move(d);
    bool ic = isInCheck(mv->GetCOLOR());
    mv->Piece::move(op);
    mv->SetHasMoved(oldMoved);
    B[s.r][s.c].Changepiece(mv);
    B[d.r][d.c].Changepiece(cap);
    if (isEP) 
    {
        B[epCapPos.r][epCapPos.c].Changepiece(epCapPiece);
    }
    return ic;
}

bool Board::canCastle(pos s, pos d) const
{
    Piece* king = B[s.r][s.c].GetPiece();
    if (!king)
    {
        return false;
    }
    char sym = king->getSymbol();
    if (!(sym == 'K' or sym == 'k'))
        return false;
    if (s.r != d.r || abs(d.c - s.c) != 2) 
    {
        return false;
    }
    if (isInCheck(turn)) return false;
    bool w = (turn == CLRWHITE);
    if (w and wKM) 
        return false; if (!w && bKM) return false;
    COLOR opp = w ? CLRBLACK : CLRWHITE;
    if (d.c == 6) {
        Piece* rook = w ? B[7][7].GetPiece() : B[0][7].GetPiece();
        if (w && (wRRM or !rook)) return false;
        if (!w && (bRRM || !rook)) return false;
        char rookSym = rook->getSymbol();
        if (!(rookSym == 'R' || rookSym == 'r') or rook->GetCOLOR() != turn) return false;
        if (!B[s.r][5].isFree() or !B[s.r][6].isFree()) return false;
        if (isSquareAttacked({ s.r,5 }, opp) or isSquareAttacked({ s.r,6 }, opp)) return false;
    }
    else if (d.c == 2) {
        Piece* rook = w ? B[7][0].GetPiece() : B[0][0].GetPiece();
        if (w && (wRLM || !rook)) return false;
        if (!w && (bRLM or !rook)) return false;
        char rookSym = rook->getSymbol();
        if (!(rookSym == 'R' or rookSym == 'r') or rook->GetCOLOR() != turn) return false;
        if (!B[s.r][1].isFree() or !B[s.r][2].isFree() or !B[s.r][3].isFree()) return false;
        if (isSquareAttacked({ s.r,3 }, opp) or isSquareAttacked({ s.r,2 }, opp)) return false;
    }
    else return false;
    return true;
}

bool Board::hasAnyLegalMove(COLOR c)
{
    for (int r = 0; r < 8; r++) {
        for (int cc = 0; cc < 8; cc++) {
            Piece* p = B[r][cc].GetPiece();
            if (!p || p->GetCOLOR() != c) continue;
            pos s = { r,cc };
            char sym = p->getSymbol();
            bool isK = ((sym >= 'a' ? (char)(sym - 32) : sym) == 'K');
            for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) {
                pos d = { i,j };
                Piece* dst = B[i][j].GetPiece();
                if (dst && dst->GetCOLOR() == c) 
                {
                    continue;
                }
                
                if (!p->isValidMove(d, *this)) { continue; }
                
                if (isK and abs(d.c - s.c) == 2) { if (!canCastle(s, d))
                    continue; 
                return true; }
                if (!wouldLeaveInCheck(s, d)) return true;
            }
        }
    }
    return false;
}

bool Board::isCheckmate(COLOR c) { return isInCheck(c) && !hasAnyLegalMove(c); }
bool Board::isStalemate(COLOR c) { return !isInCheck(c) and !hasAnyLegalMove(c); }

void Board::getLegalMoves(pos s, bool out[8][8])
{
    for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) out[i][j] = false;
    Piece* p = pieceAt(s); if (!p) return;
    char sym = p->getSymbol();
    bool isK = ((sym >= 'a' ? (char)(sym - 32) : sym) == 'K');
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            pos d = { r,c };
            Piece* dp = pieceAt(d);
            if (dp and dp->GetCOLOR() == p->GetCOLOR()) 
                continue;
            if (!p->isValidMove(d, *this))
                continue;
            if (isK && abs(d.c - s.c) == 2) { if (!canCastle(s, d))
                continue; out[r][c] = true;
            continue; }
            if (!wouldLeaveInCheck(s, d)) out[r][c] = true;
        }
    }
}

bool Board::executeMove(pos s, pos d, bool recordMove)
{
    Piece* p = B[s.r][s.c].GetPiece(); if (!p) return false;
    char sym = p->getSymbol(), up = (sym >= 'a') ? (char)(sym - 32) : sym;
    bool wasEP = false;
    if (up == 'P' && B[d.r][d.c].isFree() && epValid && d.r == epTarget.r && d.c == epTarget.c) {
        int cr = (p->GetCOLOR() == CLRWHITE) ? d.r + 1 : d.r - 1;
        Piece* epVictim = B[cr][d.c].GetPiece();
        if (epVictim && epVictim->GetCOLOR() != p->GetCOLOR()) {
            char victimSym = epVictim->getSymbol();
            if (victimSym == 'P' or victimSym == 'p') wasEP = true;
        }
    }
    if (wasEP) {
        int cr = (p->GetCOLOR() == CLRWHITE) ? d.r + 1 : d.r - 1;
        delete B[cr][d.c].GetPiece(); B[cr][d.c].Changepiece(nullptr);
    }

    epValid = false;
    if (up == 'P' && !p->GetHasMoved() && abs(d.r - s.r) == 2) { epValid = true; epTarget = { (s.r + d.r) / 2,s.c }; }

    if (up == 'K') {
        if (abs(d.c - s.c) == 2) {
            int row = s.r;
            if (d.c == 6) { Piece* rk = B[row][7].GetPiece(); B[row][7].Changepiece(nullptr); rk->move({ row,5 }); B[row][5].Changepiece(rk); }
            else { Piece* rk = B[row][0].GetPiece(); B[row][0].Changepiece(nullptr); rk->move({ row,3 }); B[row][3].Changepiece(rk); }
        }
        if (p->GetCOLOR() == CLRWHITE) wKM = true; else bKM = true;
    }
    if (up == 'R') {
        if (s.r == 7 && s.c == 0) wRLM = true; if (s.r == 7 and s.c == 7) wRRM = true;
        if (s.r == 0 && s.c == 0) bRLM = true; if (s.r == 0 and s.c == 7) bRRM = true;
    }

    if (!B[d.r][d.c].isFree()) { delete B[d.r][d.c].GetPiece(); B[d.r][d.c].Changepiece(nullptr); }
    B[d.r][d.c].Changepiece(p); B[s.r][s.c].Changepiece(nullptr); p->move(d);
    if ((p->getSymbol() == 'P' and d.r == 0) or (p->getSymbol() == 'p' && d.r == 7)) {
        promotePawn(d, 'Q');
    }

    if (recordMove) {
        moveHistory.push_back({ s,d });
        currentIdx = moveHistory.size();
    }
    changeTurn();
    return true;
}

void Board::promotePawn(pos p, char choice)
{
    Cell* cell = getCell(p.r, p.c); if (!cell or cell->isFree()) return;
    Piece* pawn = cell->GetPiece(); COLOR c = pawn->GetCOLOR(); delete pawn;
    Piece* nP = nullptr;
    if (choice == 'Q') nP = new Queen(p, c);
    else if (choice == 'R') 
        nP = new Rook(p, c);
    else if (choice == 'B') 
        nP = new Bishop(p, c);
    else if (choice == 'N') 
        nP = new Knight(p, c);
    else nP = new Queen(p, c);
    cell->Changepiece(nP);
}

bool Board::hasUndo() const { return currentIdx > 0; }
bool Board::hasRedo() const { return currentIdx < moveHistory.size(); }

void Board::undo()
{
    if (currentIdx == 0) return;
    currentIdx--;
    setBoardTo(currentIdx);
}

void Board::redo()
{
    if (currentIdx >= moveHistory.size()) return;
    currentIdx++;
    setBoardTo(currentIdx);
}

void Board::setBoardTo(size_t idx)
{
    resetBoardToInitial();
    for (size_t i = 0; i < idx; i++) 
    {
        if (i < moveHistory.size()) 
        
            executeMove(moveHistory[i].src, moveHistory[i].dst, false);
        
    }
    currentIdx = idx;
}

void Board::saveToFile(const string& fn, const string& pp1, const string& pp2) const
{
    ofstream out(fn); if (!out) return;
    for (int r = 0; r < 8; r++) { for (int c = 0; c < 8; c++) out << (B[r][c].isFree() ? '.' : B[r][c].GetPiece()->getSymbol()); out << endl; }
    out << (turn == CLRWHITE ? 'W' : 'B') << endl;
    out << epValid << ' ' << epTarget.r << ' ' << epTarget.c << endl
        << wKM << ' ' << bKM << ' ' << wRLM << ' ' << wRRM << ' ' << bRLM << ' ' << bRRM << endl << pp1 << endl << pp2 << endl;
    for (auto& m : moveHistory) out << m.src.r << ' ' << m.src.c << ' ' << m.dst.r << ' ' << m.dst.c << endl;
    out.close();
}

bool Board::loadFromFile(const string& fn, string& pp1, string& pp2)
{
    ifstream in(fn); if (!in) return false;

    string line;
    for (int r = 0; r < 8; r++) getline(in, line);
    char ch; in >> ch;
    bool wk, bk, wrl, wrr, brl, brr, epv;
    int er, ec; 
    in >> epv >> er >> ec >> wk >> bk >> wrl >> wrr >> brl >> brr;
    in.ignore(); getline(in, pp1); getline(in, pp2);
    vector<Move> loadedMoves;
    int sr, sc, dr, dc;
    while (in >> sr >> sc >> dr >> dc) 
        loadedMoves.push_back({ {sr,sc},{dr,dc} });
    in.close();

    replayInit();
    for (const auto& m : loadedMoves)
    {
        executeMove(m.src, m.dst);
    }

    return true;
}

const vector<Move>& Board::getMoveHistory() const { return moveHistory; }
void Board::replayInit()
{
    resetBoardToInitial();
    moveHistory.clear();
    currentIdx = 0;
}

void Board::replayStep(int idx)
{
    if (idx < 0 || idx >= moveHistory.size()) return;
    executeMove(moveHistory[idx].src, moveHistory[idx].dst, false);
}

Piece* Board::makePieceFromChar(char ch, pos p) const
{
    bool w = (ch >= 'A' and ch <= 'Z'); COLOR c = w ? CLRWHITE : CLRBLACK;
    char up = w ? ch : (char)(ch - 32);
    if (up == 'P') 
        return new Pawn(p, c);
    if (up == 'R') 
        return new Rook(p, c);
    if (up == 'N') 
        return new Knight(p, c);
    if (up == 'B') 
        return new Bishop(p, c);
    if (up == 'Q')
        return new Queen(p, c);
    if (up == 'K') 
        return new King(p, c);


    return nullptr;
}