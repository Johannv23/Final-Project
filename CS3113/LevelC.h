#ifndef LEVELC_H
#define LEVELC_H

#include "Scene.h"
#include <vector>

struct Projectile
{
    Vector2 pos;
    Vector2 vel;
    bool    active   = false;
    bool    isPlayer = false;
};

class LevelC : public Scene
{
private:
    Texture2D mBgTexture;
    Texture2D mPlayerTex;
    Texture2D mOffsetIdleTex;
    Texture2D mOffsetRunTex;

    Music     mBgMusic;
    Sound     mSndAttack;
    Sound     mSndRun;

    Vector2 mPlayerPos;
    Vector2 mPlayerVel;
    float   mPlayerW      = 128.0f;
    float   mPlayerH      = 128.0f;
    bool    mFacingRight  = true;
    bool    mOnGround     = false;
    float   mAnimTimer    = 0.0f;
    int     mAnimFrame    = 0;
    bool    mIsMoving     = false;

    static constexpr int   PLAYER_FRAMES  = 49;
    static constexpr float ANIM_SPEED     = 0.08f;
    static constexpr float PLAYER_SPEED   = 300.0f;
    static constexpr float JUMP_POWER     = -700.0f;
    static constexpr float GRAVITY        = 1400.0f;
    static constexpr float GROUND_Y       = 750.0f;

    float mOffsetX;
    float mOffsetY           = GROUND_Y - 160.0f;
    bool  mOffsetFacingRight = false;
    int   mOffsetHP          = 100;
    bool  mOffsetMoving      = false;
    float mOffsetTargetX;
    float mOffsetAnimTimer   = 0.0f;
    int   mOffsetAnimFrame   = 0;

    static constexpr int   OFFSET_IDLE_FRAMES = 25;
    static constexpr int   OFFSET_RUN_FRAMES  = 49;
    static constexpr float OFFSET_ANIM_SPEED  = 0.1f;
    static constexpr float OFFSET_LEFT_X      = 250.0f;
    static constexpr float OFFSET_RIGHT_X     = 1200.0f;
    static constexpr float OFFSET_SPEED       = 350.0f;
    static constexpr float SHOOT_INTERVAL     = 2.0f;
    static constexpr float MOVE_INTERVAL      = 6.0f;

    float mAITimer      = 0.0f;
    float mAIShootTimer = 0.0f;

    std::vector<Projectile> mProjectiles;
    float mPlayerShootCD = 0.0f;
    static constexpr float PLAYER_SHOOT_CD  = 0.5f;
    static constexpr float PROJECTILE_SPEED = 600.0f;
    static constexpr float PROJ_W = 50.0f;
    static constexpr float PROJ_H = 14.0f;

    float mPlayerHitFlash = 0.0f;
    float mOffsetHitFlash = 0.0f;

    bool  mBattleOver = false;
    bool  mPlayerWon  = false;

    void spawnOffsetProjectile();
    void spawnPlayerProjectile();
    bool rectsOverlap(Vector2 aPos, float aW, float aH,
                      Vector2 bPos, float bW, float bH);
    void drawSprite(Texture2D tex, int frame, int cols, int rows,
                    float x, float y, float w, float h,
                    bool flipX, Color tint);

public:
    LevelC();
    LevelC(Vector2 origin, const char *bgHexCode);
    ~LevelC();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELC_H