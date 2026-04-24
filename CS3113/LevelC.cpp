#include "LevelC.h"

extern int gLives;

LevelC::LevelC() : Scene{{0.0f, 0.0f}, "#000000"} {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}
LevelC::~LevelC() { shutdown(); }

void LevelC::drawSprite(Texture2D tex, int frame, int cols, int rows,
                        float x, float y, float w, float h,
                        bool flipX, Color tint)
{
    float fw = (float)tex.width  / cols;
    float fh = (float)tex.height / rows;
    int   col = frame % cols;
    int   row = frame / cols;
    Rectangle src = { col * fw, row * fh, fw, fh };
    if (flipX) src.width = -src.width;
    DrawTexturePro(tex, src, {x, y, w, h}, {0, 0}, 0.0f, tint);
}

bool LevelC::rectsOverlap(Vector2 aPos, float aW, float aH,
                           Vector2 bPos, float bW, float bH)
{
    return aPos.x < bPos.x + bW && aPos.x + aW > bPos.x &&
           aPos.y < bPos.y + bH && aPos.y + aH > bPos.y;
}

void LevelC::spawnOffsetProjectile()
{
    Projectile p;
    float dir  = mOffsetFacingRight ? 1.0f : -1.0f;
    p.pos      = { mOffsetX + (mOffsetFacingRight ? 160.0f : -PROJ_W),
                   mOffsetY + 60.0f };
    p.vel      = { dir * PROJECTILE_SPEED, 0.0f };
    p.active   = true;
    p.isPlayer = false;
    mProjectiles.push_back(p);
    PlaySound(mSndAttack);
}

void LevelC::spawnPlayerProjectile()
{
    Projectile p;
    p.pos      = { mPlayerPos.x + (mFacingRight ? mPlayerW : -PROJ_W),
                   mPlayerPos.y + mPlayerH * 0.35f };
    p.vel      = { mFacingRight ? PROJECTILE_SPEED : -PROJECTILE_SPEED, 0.0f };
    p.active   = true;
    p.isPlayer = true;
    mProjectiles.push_back(p);
}

void LevelC::initialise()
{
    mGameState.nextSceneID = 0;

    mBgTexture     = LoadTexture("assets/tiles/battle_bg.png");
    mPlayerTex     = LoadTexture("assets/characters/battle_player.png");
    mOffsetIdleTex = LoadTexture("assets/characters/offset_idle.png");
    mOffsetRunTex  = LoadTexture("assets/characters/offset_run.png");
    mBgMusic       = LoadMusicStream("assets/audio/offset_battle_music.wav");
    PlayMusicStream(mBgMusic);
    mSndAttack     = LoadSound("assets/audio/migoDAUM.wav");
    mSndRun        = LoadSound("assets/audio/migoOUUU.wav");

    mPlayerPos     = { 200.0f, GROUND_Y - mPlayerH };
    mPlayerVel     = { 0.0f, 0.0f };
    mFacingRight   = true;
    mOnGround      = true;
    mIsMoving      = false;
    mAnimFrame     = 0;
    mAnimTimer     = 0.0f;

    mOffsetX           = OFFSET_RIGHT_X;
    mOffsetFacingRight = false;
    mOffsetTargetX     = OFFSET_LEFT_X;
    mOffsetMoving      = false;
    mOffsetHP          = 100;
    mOffsetAnimFrame   = 0;
    mOffsetAnimTimer   = 0.0f;
    mAITimer           = 0.0f;
    mAIShootTimer      = 0.0f;

    mProjectiles.clear();
    mPlayerHitFlash = 0.0f;
    mOffsetHitFlash = 0.0f;
    mBattleOver     = false;
    mPlayerWon      = false;

    mGameState.player     = nullptr;
    mGameState.enemies    = nullptr;
    mGameState.numEnemies = 0;
    mGameState.map        = nullptr;
}

void LevelC::update(float deltaTime)
{
    UpdateMusicStream(mBgMusic);

    if (mBattleOver)
    {
        if (IsKeyPressed(KEY_ENTER))
            mGameState.nextSceneID = 2;
        return;
    }

    float moveX = 0.0f;
    if (IsKeyDown(KEY_A)) { moveX = -1.0f; mFacingRight = false; }
    if (IsKeyDown(KEY_D)) { moveX =  1.0f; mFacingRight = true;  }
    mIsMoving = (moveX != 0.0f);

    if (IsKeyPressed(KEY_W) && mOnGround)
    {
        mPlayerVel.y = JUMP_POWER;
        mOnGround    = false;
    }

    mPlayerShootCD -= deltaTime;
    if (IsKeyPressed(KEY_SPACE) && mPlayerShootCD <= 0.0f)
    {
        spawnPlayerProjectile();
        mPlayerShootCD = PLAYER_SHOOT_CD;
    }

    mPlayerPos.x += moveX * PLAYER_SPEED * deltaTime;
    mPlayerVel.y += GRAVITY * deltaTime;
    mPlayerPos.y += mPlayerVel.y * deltaTime;

    if (mPlayerPos.y >= GROUND_Y - mPlayerH)
    {
        mPlayerPos.y = GROUND_Y - mPlayerH;
        mPlayerVel.y = 0.0f;
        mOnGround    = true;
    }

    if (mPlayerPos.x < 0)                  mPlayerPos.x = 0;
    if (mPlayerPos.x > 1600.0f - mPlayerW) mPlayerPos.x = 1600.0f - mPlayerW;

    if (mIsMoving || !mOnGround)
    {
        mAnimTimer += deltaTime;
        if (mAnimTimer >= ANIM_SPEED)
        {
            mAnimTimer = 0.0f;
            mAnimFrame = (mAnimFrame + 1) % PLAYER_FRAMES;
        }
    }
    else mAnimFrame = 0;

    mOffsetAnimTimer += deltaTime;
    int frames = mOffsetMoving ? OFFSET_RUN_FRAMES : OFFSET_IDLE_FRAMES;
    if (mOffsetAnimTimer >= OFFSET_ANIM_SPEED)
    {
        mOffsetAnimTimer = 0.0f;
        mOffsetAnimFrame = (mOffsetAnimFrame + 1) % frames;
    }

    if (!mOffsetMoving)
    {
        mAIShootTimer += deltaTime;
        if (mAIShootTimer >= SHOOT_INTERVAL)
        {
            mAIShootTimer = 0.0f;
            spawnOffsetProjectile();
        }
    }

    mAITimer += deltaTime;
    if (mAITimer >= MOVE_INTERVAL)
    {
        mAITimer         = 0.0f;
        mOffsetMoving    = true;
        mOffsetAnimFrame = 0;
        mOffsetTargetX   = (mOffsetX > 800.0f) ? OFFSET_LEFT_X : OFFSET_RIGHT_X;
        PlaySound(mSndRun);
    }

    if (mOffsetMoving)
    {
        float dir = (mOffsetTargetX > mOffsetX) ? 1.0f : -1.0f;
        mOffsetFacingRight = (dir > 0);
        mOffsetX += dir * OFFSET_SPEED * deltaTime;
        if ((dir > 0 && mOffsetX >= mOffsetTargetX) ||
            (dir < 0 && mOffsetX <= mOffsetTargetX))
        {
            mOffsetX           = mOffsetTargetX;
            mOffsetMoving      = false;
            mOffsetAnimFrame   = 0;
            mAIShootTimer      = 0.0f;
            mOffsetFacingRight = (mPlayerPos.x > mOffsetX);
        }
    }

    for (auto &p : mProjectiles)
    {
        if (!p.active) continue;
        p.pos.x += p.vel.x * deltaTime;

        if (p.pos.x < -100 || p.pos.x > 1700) { p.active = false; continue; }

        if (p.isPlayer)
        {
            if (rectsOverlap(p.pos, PROJ_W, PROJ_H,
                             {mOffsetX, mOffsetY}, 160.0f, 160.0f))
            {
                p.active = false;
                mOffsetHP -= 5;
                mOffsetHitFlash = 0.2f;
                if (mOffsetHP <= 0) { mOffsetHP = 0; mBattleOver = true; mPlayerWon = true; }
            }
        }
        else
        {
            if (rectsOverlap(p.pos, PROJ_W, PROJ_H,
                             mPlayerPos, mPlayerW, mPlayerH))
            {
                p.active = false;
                gLives  -= 10;
                mPlayerHitFlash = 0.3f;
                if (gLives <= 0) { gLives = 0; mBattleOver = true; mPlayerWon = false; }
            }
        }
    }

    if (mPlayerHitFlash > 0) mPlayerHitFlash -= deltaTime;
    if (mOffsetHitFlash > 0) mOffsetHitFlash -= deltaTime;
    UpdateMusicStream(mBgMusic);
}

void LevelC::render()
{
    ClearBackground(BLACK);

    DrawTexturePro(
        mBgTexture,
        {0, 0, (float)mBgTexture.width, (float)mBgTexture.height},
        {0, 0, 1600, 960}, {0, 0}, 0.0f, WHITE
    );

    for (auto &p : mProjectiles)
    {
        if (!p.active) continue;
        Color c = p.isPlayer ? (Color){0, 220, 255, 255} : (Color){255, 140, 0, 255};
        DrawRectangleV(p.pos, {PROJ_W, PROJ_H}, c);
        DrawRectangle((int)p.pos.x-4, (int)p.pos.y-4,
                      (int)PROJ_W+8, (int)PROJ_H+8, {c.r,c.g,c.b,50});
    }

    Color     offsetTint  = (mOffsetHitFlash > 0) ? RED : WHITE;
    Texture2D &offsetTex  = mOffsetMoving ? mOffsetRunTex : mOffsetIdleTex;
    int        offsetCols = mOffsetMoving ? 7 : 5;
    int        offsetRows = mOffsetMoving ? 7 : 5;

    drawSprite(offsetTex, mOffsetAnimFrame, offsetCols, offsetRows,
               mOffsetX, mOffsetY, 160.0f, 160.0f,
               !mOffsetFacingRight, offsetTint);

    Color playerTint = (mPlayerHitFlash > 0) ? RED : WHITE;
    drawSprite(mPlayerTex, mAnimFrame, 7, 7,
               mPlayerPos.x, mPlayerPos.y, mPlayerW, mPlayerH,
               !mFacingRight, playerTint);

    DrawRectangle(1150, 20, 430, 30, {0,0,0,180});
    DrawRectangle(1152, 22, (int)(426 * (mOffsetHP / 100.0f)), 26,
                  (Color){255, 60, 60, 255});
    DrawRectangleLines(1150, 20, 430, 30, {255,80,255,255});
    DrawText(TextFormat("OFFSET  %d / 100", mOffsetHP), 1158, 27, 14, WHITE);

    DrawText("A/D: Move   W: Jump   SPACE: Attack", 20, 920, 16,
             {180,180,180,200});

    if (mBattleOver)
    {
        DrawRectangle(0, 0, 1600, 960, {0,0,0,180});
        if (mPlayerWon)
        {
            DrawText("YOU WON!", 580, 380, 70, GOLD);
            DrawText("The crowd goes wild!", 530, 470, 32, WHITE);
        }
        else
        {
            DrawText("YOU LOST.", 560, 380, 70, RED);
            DrawText("Not this time...", 560, 470, 32, WHITE);
        }
        DrawText("[ENTER] Continue", 600, 560, 26, {150,150,150,255});
    }
}

void LevelC::shutdown()
{
    StopMusicStream(mBgMusic);
    UnloadMusicStream(mBgMusic);
    UnloadSound(mSndAttack);
    UnloadSound(mSndRun);
    if (mBgTexture.id != 0)     UnloadTexture(mBgTexture);
    if (mPlayerTex.id != 0)     UnloadTexture(mPlayerTex);
    if (mOffsetIdleTex.id != 0) UnloadTexture(mOffsetIdleTex);
    if (mOffsetRunTex.id != 0)  UnloadTexture(mOffsetRunTex);
    mProjectiles.clear();
    mGameState.player     = nullptr;
    mGameState.enemies    = nullptr;
    mGameState.map        = nullptr;
}