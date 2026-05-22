#include "raylib.h"
#include "BSAI25013-Board.h"

int main()
{
    InitWindow(800, 800, "BSAI25013");
    SetTargetFPS(60);
    InitAudioDevice();
    Board game(800, 800);
    game.RunGame();

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

//Arrow keys to navigate in replay Mode
//S to save the game to file
//U for undo and R for Redo while the game is running