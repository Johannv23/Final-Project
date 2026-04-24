#ifndef LEVELD_H
#define LEVELD_H

#include "Scene.h"
#include <vector>

struct ProjectileD
{
    Vector2 pos;
    Vector2 vel;
    bool    active   = false;
    bool    isPlayer = false;
    bool    isHigh   = false;  // flies high — stand still to dodge
};

class LevelD : public Scene
{
private:
    Texture2D mBgTexture;
    Texture2D mPlayerTex;
    Texture2D mCartiIdleTex;
    Texture2D mCartiRunTex;
    Texture2D mProjNormalTex;
    Texture2D mProjHighTex;
    Music     mBgMusic;

    Sound mSndRun;
    Sound mSndAttack;
    Sound mSndHigh;

    Vector2 mPlayerPos;
    Vector2 mPlayerVel;
    float   mPlayerW     = 128.0f;
    float   mPlayerH     = 128.0f;
    bool    mFacingRight = true;
    bool    mOnGround    = false;
    float   mAnimTimer   = 0.0f;
    int     mAnimFrame   = 0;
    bool    mIsMoving    = false;

    static constexpr int   PLAYER_FRAMES = 49;
    static constexpr float ANIM_SPEED    = 0.08f;
    static constexpr float PLAYER_SPEED  = 300.0f;
    static constexpr float JUMP_POWER    = -700.0f;
    static constexpr float GRAVITY       = 1400.0f;
    static constexpr float GROUND_Y      = 750.0f;

    float mCartiX;
    float mCartiY            = GROUND_Y - 160.0f;
    bool  mCartiFacingRight  = false;
    int   mCartiHP           = 100;
    bool  mCartiMoving       = false;
    float mCartiTargetX;
    float mCartiAnimTimer    = 0.0f;
    int   mCartiAnimFrame    = 0;

    // attack phase cycles: 0=normal, 1=normal, 2=high
    int   mAttackPhase       = 0;

    static constexpr int   CARTI_IDLE_FRAMES = 49;
    static constexpr int   CARTI_RUN_FRAMES  = 49;
    static constexpr float CARTI_ANIM_SPEED  = 0.1f;
    static constexpr float CARTI_LEFT_X      = 250.0f;
    static constexpr float CARTI_RIGHT_X     = 1200.0f;
    static constexpr float CARTI_SPEED       = 400.0f;
    static constexpr float SHOOT_INTERVAL    = 1.0f;
    static constexpr float MOVE_INTERVAL     = 10.0f;

    float mAITimer       = 0.0f;
    float mAIShootTimer  = 0.0f;

    std::vector<ProjectileD> mProjectiles;
    float mPlayerShootCD = 0.0f;
    static constexpr float PLAYER_SHOOT_CD  = 0.5f;
    static constexpr float PROJECTILE_SPEED = 650.0f;
    static constexpr float PROJ_W  = 50.0f;
    static constexpr float PROJ_H  = 14.0f;
    static constexpr float HIGH_Y  = GROUND_Y - 200.0f;

    float mPlayerHitFlash = 0.0f;
    float mCartiHitFlash  = 0.0f;

    bool  mBattleOver = false;
    bool  mPlayerWon  = false;

    void spawnCartiProjectile();
    void spawnCartiHighProjectile();
    void spawnPlayerProjectile();
    bool rectsOverlap(Vector2 aPos, float aW, float aH,
                      Vector2 bPos, float bW, float bH);
    void drawSprite(Texture2D tex, int frame, int cols, int rows,
                    float x, float y, float w, float h,
                    bool flipX, Color tint);

public:
    LevelD();
    LevelD(Vector2 origin, const char *bgHexCode);
    ~LevelD();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELD_H