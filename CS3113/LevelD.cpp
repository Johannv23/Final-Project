#include "LevelD.h"

extern int gLives;

LevelD::LevelD() : Scene{{0.0f, 0.0f}, "#000000"} {}
LevelD::LevelD(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}
LevelD::~LevelD() { shutdown(); }

void LevelD::drawSprite(Texture2D tex, int frame, int cols, int rows,
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

bool LevelD::rectsOverlap(Vector2 aPos, float aW, float aH,
                           Vector2 bPos, float bW, float bH)
{
    return aPos.x < bPos.x + bW && aPos.x + aW > bPos.x &&
           aPos.y < bPos.y + bH && aPos.y + aH > bPos.y;
}

void LevelD::spawnCartiProjectile()
{
    ProjectileD p;
    float dir  = mCartiFacingRight ? 1.0f : -1.0f;
    p.pos      = { mCartiX + (mCartiFacingRight ? 260.0f : -PROJ_W),
                   mCartiY + 80.0f };
    p.vel      = { dir * PROJECTILE_SPEED, 0.0f };
    p.active   = true;
    p.isPlayer = false;
    p.isHigh   = false;
    mProjectiles.push_back(p);
    PlaySound(mSndAttack);
}

void LevelD::spawnCartiHighProjectile()
{
    ProjectileD p;
    float dir  = mCartiFacingRight ? 1.0f : -1.0f;
    p.pos      = { mCartiX + (mCartiFacingRight ? 260.0f : -PROJ_W),
                   HIGH_Y };
    p.vel      = { dir * PROJECTILE_SPEED, 0.0f };
    p.active   = true;
    p.isPlayer = false;
    p.isHigh   = true;
    mProjectiles.push_back(p);
    PlaySound(mSndHigh);
}

void LevelD::spawnPlayerProjectile()
{
    ProjectileD p;
    p.pos      = { mPlayerPos.x + (mFacingRight ? mPlayerW : -PROJ_W),
                   mPlayerPos.y + mPlayerH * 0.35f };
    p.vel      = { mFacingRight ? PROJECTILE_SPEED : -PROJECTILE_SPEED, 0.0f };
    p.active   = true;
    p.isPlayer = true;
    p.isHigh   = false;
    mProjectiles.push_back(p);
}

void LevelD::initialise()
{
    mGameState.nextSceneID = 0;

    mBgTexture      = LoadTexture("assets/levels/carti_bg.png");
    mPlayerTex      = LoadTexture("assets/characters/battle_player.png");
    mCartiIdleTex   = LoadTexture("assets/characters/carti-idle.png");
    mCartiRunTex    = LoadTexture("assets/characters/carti-run.png");
    mProjNormalTex  = LoadTexture("assets/audio/batsatt.png");
    mProjHighTex    = LoadTexture("assets/audio/whaehatt.png");
    mBgMusic        = LoadMusicStream("assets/audio/cart_bg_music.wav");
    PlayMusicStream(mBgMusic);

    mSndAttack = LoadSound("assets/audio/SCHYEAH.wav");
    mSndRun    = LoadSound("assets/audio/HOMIXIDE.wav");
    mSndHigh   = LoadSound("assets/audio/WHAT?.wav");

    mPlayerPos   = { 200.0f, GROUND_Y - mPlayerH };
    mPlayerVel   = { 0.0f, 0.0f };
    mFacingRight = true;
    mOnGround    = true;
    mIsMoving    = false;
    mAnimFrame   = 0;
    mAnimTimer   = 0.0f;

    mCartiX            = CARTI_RIGHT_X;
    mCartiFacingRight  = false;
    mCartiTargetX      = CARTI_LEFT_X;
    mCartiMoving       = false;
    mCartiHP           = 100;
    mCartiAnimFrame    = 0;
    mCartiAnimTimer    = 0.0f;
    mAITimer           = 0.0f;
    mAIShootTimer      = 0.0f;
    mAttackPhase       = 0;

    mProjectiles.clear();
    mPlayerHitFlash = 0.0f;
    mCartiHitFlash  = 0.0f;
    mBattleOver     = false;
    mPlayerWon      = false;

    mGameState.player     = nullptr;
    mGameState.enemies    = nullptr;
    mGameState.numEnemies = 0;
    mGameState.map        = nullptr;
}

void LevelD::update(float deltaTime)
{
    if (mBattleOver)
    {
        if (IsKeyPressed(KEY_ENTER))
            mGameState.nextSceneID = 1;
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

    mCartiAnimTimer += deltaTime;
    int frames = mCartiMoving ? CARTI_RUN_FRAMES : CARTI_IDLE_FRAMES;
    if (mCartiAnimTimer >= CARTI_ANIM_SPEED)
    {
        mCartiAnimTimer = 0.0f;
        mCartiAnimFrame = (mCartiAnimFrame + 1) % frames;
    }

    if (!mCartiMoving)
    {
        mAIShootTimer += deltaTime;
        if (mAIShootTimer >= SHOOT_INTERVAL)
        {
            mAIShootTimer = 0.0f;
            // cycle: 0=normal, 1=high, 2=high
            if (mAttackPhase == 0)
                spawnCartiProjectile();
            else
                spawnCartiHighProjectile();
            mAttackPhase = (mAttackPhase + 1) % 3;
        }
    }

    mAITimer += deltaTime;
    if (mAITimer >= MOVE_INTERVAL)
    {
        mAITimer        = 0.0f;
        mCartiMoving    = true;
        mCartiAnimFrame = 0;
        mCartiTargetX   = (mCartiX > 800.0f) ? CARTI_LEFT_X : CARTI_RIGHT_X;
        PlaySound(mSndRun);
    }

    if (mCartiMoving)
    {
        float dir = (mCartiTargetX > mCartiX) ? 1.0f : -1.0f;
        mCartiFacingRight = (dir > 0);
        mCartiX += dir * CARTI_SPEED * deltaTime;
        if ((dir > 0 && mCartiX >= mCartiTargetX) ||
            (dir < 0 && mCartiX <= mCartiTargetX))
        {
            mCartiX            = mCartiTargetX;
            mCartiMoving       = false;
            mCartiAnimFrame    = 0;
            mAIShootTimer      = 0.0f;
            mCartiFacingRight  = (mPlayerPos.x > mCartiX);
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
                             {mCartiX, mCartiY}, 160.0f, 160.0f))
            {
                p.active = false;
                mCartiHP -= 5;
                mCartiHitFlash = 0.2f;
                if (mCartiHP <= 0) { mCartiHP = 0; mBattleOver = true; mPlayerWon = true; }
            }
        }
        else
        {
            // High projectile only hits if player is jumping (not on ground)
            bool shouldHit = false;
            if (p.isHigh)
                shouldHit = !mOnGround &&
                            rectsOverlap(p.pos, PROJ_W, PROJ_H,
                                         mPlayerPos, mPlayerW, mPlayerH);
            else
                shouldHit = rectsOverlap(p.pos, PROJ_W, PROJ_H,
                                         mPlayerPos, mPlayerW, mPlayerH);

            if (shouldHit)
            {
                p.active = false;
                gLives  -= 10;
                mPlayerHitFlash = 0.3f;
                if (gLives <= 0) { gLives = 0; mBattleOver = true; mPlayerWon = false; }
            }
        }
    }

    if (mPlayerHitFlash > 0) mPlayerHitFlash -= deltaTime;
    if (mCartiHitFlash  > 0) mCartiHitFlash  -= deltaTime;
    UpdateMusicStream(mBgMusic);
}

void LevelD::render()
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
        if (p.isPlayer)
        {
            Color c = {0, 220, 255, 255};
            DrawRectangleV(p.pos, {PROJ_W, PROJ_H}, c);
            DrawRectangle((int)p.pos.x-4, (int)p.pos.y-4,
                          (int)PROJ_W+8, (int)PROJ_H+8, {c.r,c.g,c.b,50});
        }
        else
        {
            Texture2D &projTex = p.isHigh ? mProjHighTex : mProjNormalTex;
            Color tint = p.isHigh ? (Color){255, 50, 50, 255} : (Color){255, 150, 0, 255};
            float pw = 80.0f, ph = 50.0f;
            DrawTexturePro(
                projTex,
                {0, 0, (float)projTex.width, (float)projTex.height},
                {p.pos.x, p.pos.y, pw, ph},
                {0, 0}, 0.0f, tint
            );
        }
    }

    Color     cartiTint = (mCartiHitFlash > 0) ? RED : WHITE;
    Texture2D &cartiTex = mCartiMoving ? mCartiRunTex : mCartiIdleTex;

    drawSprite(cartiTex, mCartiAnimFrame, 7, 7,
               mCartiX, mCartiY, 160.0f, 160.0f,
               !mCartiFacingRight, cartiTint);

    Color playerTint = (mPlayerHitFlash > 0) ? RED : WHITE;

    drawSprite(mPlayerTex, mAnimFrame, 7, 7,
               mPlayerPos.x, mPlayerPos.y, mPlayerW, mPlayerH,
               !mFacingRight, playerTint);

    DrawRectangle(1150, 20, 430, 30, {0,0,0,180});
    DrawRectangle(1152, 22, (int)(426 * (mCartiHP / 100.0f)), 26,
                  (Color){255, 30, 80, 255});
    DrawRectangleLines(1150, 20, 430, 30, {255,80,255,255});
    DrawText(TextFormat("CARTI  %d / 100", mCartiHP), 1158, 27, 14, WHITE);

    DrawText("A/D: Move   W: Jump   SPACE: Attack", 20, 920, 16,
             {180,180,180,200});
    DrawText("Yellow = fly over it! Stay on ground.", 20, 900, 14,
             {255, 255, 0, 180});

    if (mBattleOver)
    {
        DrawRectangle(0, 0, 1600, 960, {0,0,0,180});
        if (mPlayerWon)
        {
            DrawText("YOU WON!", 580, 380, 70, GOLD);
        }
        else
        {
            DrawText("YOU LOST.", 560, 380, 70, RED);
            DrawText("Carti is on another level...", 500, 470, 32, WHITE);
        }
        DrawText("[ENTER] Continue", 600, 560, 26, {150,150,150,255});
    }
}

void LevelD::shutdown()
{
    StopMusicStream(mBgMusic);
    UnloadMusicStream(mBgMusic);
    if (mBgTexture.id != 0)       UnloadTexture(mBgTexture);
    if (mPlayerTex.id != 0)       UnloadTexture(mPlayerTex);
    if (mCartiIdleTex.id != 0)    UnloadTexture(mCartiIdleTex);
    if (mCartiRunTex.id != 0)     UnloadTexture(mCartiRunTex);
    if (mProjNormalTex.id != 0)   UnloadTexture(mProjNormalTex);
    if (mProjHighTex.id != 0)     UnloadTexture(mProjHighTex);
    UnloadSound(mSndAttack);
    UnloadSound(mSndRun);
    UnloadSound(mSndHigh);
    mProjectiles.clear();
    mGameState.player     = nullptr;
    mGameState.enemies    = nullptr;
    mGameState.map        = nullptr;
}