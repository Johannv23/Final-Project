#include "MenuScene.h"

MenuScene::MenuScene() : Scene{{0.0f, 0.0f}, "#000000"} {}
MenuScene::MenuScene(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}
MenuScene::~MenuScene() { shutdown(); }

void MenuScene::initialise()
{
    mGameState.nextSceneID = 0;
    mGameState.player      = nullptr;
    mGameState.enemies     = nullptr;
    mGameState.numEnemies  = 0;
    mGameState.map         = nullptr;
    mTimer = 0.0f;
    mBlink = true;
}

void MenuScene::update(float deltaTime)
{
    mTimer += deltaTime;
    if (mTimer >= 0.5f) { mTimer = 0.0f; mBlink = !mBlink; }

    if (IsKeyPressed(KEY_ENTER))
        mGameState.nextSceneID = 1; // go to LevelA (index 1 in gLevels = scene ID 2)
}

void MenuScene::render()
{
    ClearBackground({5, 3, 15, 255});

    int sw = 1280, sh = 960;

    DrawText("Underground fight club",  sw/2 - MeasureText("Underground fight club", 90)/2 + 4, 154, 90, {80, 0, 120, 180});
    DrawText("Underground fight club",  sw/2 - MeasureText("Underground fight club", 90)/2 - 2, 148, 90, {200, 50, 255, 120});
    DrawText("Underground fight club",  sw/2 - MeasureText("Underground fight club", 90)/2,     150, 90, WHITE);

    DrawText("A Rap Battle Odyssey",
             sw/2 - MeasureText("A Rap Battle Odyssey", 28)/2,
             260, 28, {200, 150, 255, 220});

    DrawRectangle(sw/2 - 200, 305, 400, 2, {255, 80, 255, 180});

    int iy = 340;
    int lineH = 36;
    Color hdr  = {255, 80, 255, 255};
    Color body = {200, 200, 200, 255};

    DrawText("HOW TO PLAY", sw/2 - MeasureText("HOW TO PLAY", 24)/2, iy, 24, hdr);
    iy += lineH + 10;

    DrawText("CLUB  —  WASD to move   ENTER to talk to NPCs",
             sw/2 - MeasureText("CLUB  —  WASD to move   ENTER to talk to NPCs", 20)/2,
             iy, 20, body); iy += lineH;

    DrawText("MAZE  —  WASD to navigate   HUG THE WALLS",
             sw/2 - MeasureText("MAZE  —  WASD to navigate   HUG THE WALLS", 20)/2,
             iy, 20, body); iy += lineH;

    DrawText("BATTLE  —  A/D: Move   W: Jump   SPACE: Attack",
             sw/2 - MeasureText("BATTLE  —  A/D: Move   W: Jump   SPACE: Attack", 20)/2,
             iy, 20, body); iy += lineH;

    DrawText("DODGE  —  Normal shots: JUMP over them",
             sw/2 - MeasureText("DODGE  —  Normal shots: JUMP over them", 20)/2,
             iy, 20, body); iy += lineH;

    DrawText("           Yellow shots: STAY on the ground",
             sw/2 - MeasureText("           Yellow shots: STAY on the ground", 20)/2,
             iy, 20, {255, 255, 100, 220}); iy += lineH + 10;

    DrawRectangle(sw/2 - 200, iy, 400, 2, {255, 80, 255, 180});
    iy += 20;

    if (mBlink)
    {
        DrawText("PRESS ENTER TO START",
                 sw/2 - MeasureText("PRESS ENTER TO START", 30)/2,
                 iy, 30, {255, 80, 255, 255});
    }

    DrawText("NYU CS3113  |  Rise of the AI", 20, sh - 30, 16, {80, 80, 80, 200});
}

void MenuScene::shutdown()
{
    mGameState.player     = nullptr;
    mGameState.enemies    = nullptr;
    mGameState.map        = nullptr;
}
