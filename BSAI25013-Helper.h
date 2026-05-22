#pragma once
#include "raylib.h"
#include <string>
using namespace std;

enum COLOR { CLRBLACK, CLRWHITE };

struct pos {
    int r, c;
};

struct GameState {
    char layout[8][8];
    bool canEP, wKM, bKM, wRLM, wRRM, bRLM, bRRM;
    pos epTarget;
    COLOR turn;
    bool exists = false;
};