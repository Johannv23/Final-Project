/**
* Author: [Your name here]
* Assignment: Rise of the AI
* Date due: 2026-04-25, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <vector>
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/LevelD.h"
#include "CS3113/MenuScene.h"

constexpr int   SCREEN_W        = 1280;
constexpr int   SCREEN_H        = 960;
constexpr int   FPS             = 60;
constexpr float FIXED_TIMESTEP  = 1.0f / 60.0f;
constexpr Vector2 ORIGIN        = { SCREEN_W / 2.0f, SCREEN_H / 2.0f };

MenuScene        *gMenuScene      = nullptr;
int               gLives          = 100;
int               gCash           = 0;
AppStatus         gAppStatus      = RUNNING;
Scene            *gCurrentScene   = nullptr;
std::vector<Scene*> gLevels;
Camera2D          gCamera         = {0};
float             gPreviousTicks  = 0.0f;
float             gTimeAccumulator= 0.0f;
LevelA           *gLevelA         = nullptr;
LevelB           *gLevelB         = nullptr;
LevelC           *gLevelC         = nullptr;
LevelD           *gLevelD         = nullptr;

void switchToScene(Scene *scene)
{
    if (gCurrentScene != nullptr) gCurrentScene->shutdown();
    gCurrentScene = scene;
    gCurrentScene->initialise();

    if (dynamic_cast<LevelC*>(scene) || dynamic_cast<LevelD*>(scene)) {
        gCamera.zoom   = 1.0f;
        gCamera.offset = { 640.0f, 480.0f };
        gCamera.target = { 800.0f, 480.0f };
    }
    else
    {
        gCamera.zoom = 0.7f;
        if (gCurrentScene->getState().player != nullptr)
            gCamera.target = gCurrentScene->getState().player->getPosition();
        else
            gCamera.target = ORIGIN;
    }
}

void initialise()
{
    InitWindow(SCREEN_W, SCREEN_H, "Rise of the AI");
    SetTargetFPS(FPS);
    InitAudioDevice();

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 0.7f;

    gMenuScene = new MenuScene(ORIGIN, "#000000");
    gLevelA    = new LevelA(ORIGIN, "#000000");
    gLevelB    = new LevelB(ORIGIN, "#000000");
    gLevelC    = new LevelC(ORIGIN, "#000000");
    gLevelD    = new LevelD(ORIGIN, "#000000");

    gLevels.push_back(gLevelA);    // index 0 → scene ID 1
    gLevels.push_back(gLevelB);    // index 1 → scene ID 2
    gLevels.push_back(gLevelC);    // index 2 → scene ID 3
    gLevels.push_back(gLevelD);    // index 3 → scene ID 4
    gLevels.push_back(gMenuScene);

    switchToScene(gMenuScene);
    gPreviousTicks = (float)GetTime();
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;

    if (IsKeyPressed(KEY_TWO))   switchToScene(gLevels[1]);
    if (IsKeyPressed(KEY_THREE)) switchToScene(gLevels[2]);
    if (IsKeyPressed(KEY_FOUR))  switchToScene(gLevels[3]);
}

void update()
{
    float ticks = (float)GetTime();
    float dt    = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    dt += gTimeAccumulator;
    if (dt < FIXED_TIMESTEP) { gTimeAccumulator = dt; return; }
    while (dt >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);

        int nextScene = gCurrentScene->getState().nextSceneID;
        if (nextScene != 0) switchToScene(gLevels[nextScene - 1]);

        Entity *player = gCurrentScene->getState().player;
        if (player && !dynamic_cast<LevelC*>(gCurrentScene)
                   && !dynamic_cast<LevelD*>(gCurrentScene))
        {
            Vector2 diff = Vector2Subtract(player->getPosition(), gCamera.target);
            gCamera.target = Vector2Add(gCamera.target, Vector2Scale(diff, 0.1f));
        }
        dt -= FIXED_TIMESTEP;
    }
    gTimeAccumulator = dt;
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);
        gCurrentScene->render();
    EndMode2D();

    float hpFraction = gLives / 100.0f;
    DrawRectangle(10, 10, 204, 22, DARKGRAY);
    DrawRectangle(12, 12, (int)(200 * hpFraction), 18, RED);
    DrawText(TextFormat("HP  %d / 100", gLives), 15, 13, 14, WHITE);

    if (gCash > 0)
        DrawText(TextFormat("CASH: $%d", gCash), 10, 38, 20, GREEN);

    EndDrawing();
}

void shutdown()
{
    gCurrentScene->shutdown();
    delete gMenuScene;
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;
    delete gLevelD;
    CloseAudioDevice();
    CloseWindow();
}

int main()
{
    initialise();
    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }
    shutdown();
    return 0;
}