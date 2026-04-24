#include "WinScene.h"

WinScene::WinScene() : Scene{{0.0f, 0.0f}, "#000000"} {}
WinScene::WinScene(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}
WinScene::~WinScene() { shutdown(); }

void WinScene::initialise()
{
    mGameState.nextSceneID = 0;
    mGameState.player      = nullptr;
    mGameState.enemies     = nullptr;
    mGameState.map         = nullptr;
}

void WinScene::update(float deltaTime) {}

void WinScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    const char *title = "YOU WIN!";
    int titleSize     = 64;
    int titleW        = MeasureText(title, titleSize);
    DrawText(title,
             (int)(mOrigin.x - titleW / 2),
             (int)(mOrigin.y - 60),
             titleSize, YELLOW);

    const char *sub  = "Press ENTER to return to menu";
    int         subW = MeasureText(sub, 24);
    DrawText(sub,
             (int)(mOrigin.x - subW / 2),
             (int)(mOrigin.y + 40),
             24, WHITE);
}

void WinScene::shutdown() {}
