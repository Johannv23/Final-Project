#include "LoseScene.h"

LoseScene::LoseScene() : Scene{{0.0f, 0.0f}, "#000000"} {}
LoseScene::LoseScene(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}
LoseScene::~LoseScene() { shutdown(); }

void LoseScene::initialise()
{
    mGameState.nextSceneID = 0;
    mGameState.player      = nullptr;
    mGameState.enemies     = nullptr;
    mGameState.map         = nullptr;
}

void LoseScene::update(float deltaTime) {}

void LoseScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    const char *title = "YOU LOSE";
    int titleSize     = 64;
    int titleW        = MeasureText(title, titleSize);
    DrawText(title,
             (int)(mOrigin.x - titleW / 2),
             (int)(mOrigin.y - 60),
             titleSize, RED);

    const char *sub  = "Press ENTER to return to menu";
    int         subW = MeasureText(sub, 24);
    DrawText(sub,
             (int)(mOrigin.x - subW / 2),
             (int)(mOrigin.y + 40),
             24, WHITE);
}

void LoseScene::shutdown() {}
