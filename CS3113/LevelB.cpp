#include "LevelB.h"

struct DialogueLine { const char *speaker; const char *text; };

static const DialogueLine kIntro[] = {
    { "YOU", "I beat Offset..." },
    { "YOU", "Where am I? Did I just get summoned somewhere?" },
    { "YOU", "Whatever... looks like there's a way through this place." },
    { "TIP", "HUG THE WALLS! map was hard to make" },
    { "YOU", "Guess I gotta finish this maze." },
};
static const int kIntroCount = 5;

static const DialogueLine kCarti[] = {
    { "YOU",   "Hello..." },
    { "CARTI", "..." },
    { "CARTI", "..." },
    { "CARTI", "..." },
};
static const int kCartiCount = 4;

LevelB::LevelB() : Scene{{0.0f, 0.0f}, "#000000"} {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}
LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{
    mGameState.nextSceneID = 0;

    mBgTexture    = LoadTexture("assets/levels/maze_bg.png");
    mSpeechBubble = LoadTexture("assets/characters/speechbubble.png");
    mShader       = LoadShader("assets/Shaders/darkness.vert",
                               "assets/Shaders/darkness.frag");
    mLightLoc     = GetShaderLocation(mShader, "lightPosition");
    mBgMusic      = LoadMusicStream("assets/audio/cave_bg_music.wav");
    PlayMusicStream(mBgMusic);

    mGameState.map = new Map(
        LEVEL_B_WIDTH, LEVEL_B_HEIGHT,
        (unsigned int *) mLevelData,
        "assets/tiles/club_tiles.png",
        TILE_DIMENSION, 16, 16, mOrigin
    );

    float spawnX = mOrigin.x - (LEVEL_B_WIDTH  * TILE_DIMENSION) / 2.0f
                   + 12 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float spawnY = mOrigin.y - (LEVEL_B_HEIGHT * TILE_DIMENSION) / 2.0f
                   + 18 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;

    std::map<Direction, std::vector<int>> playerAtlas = {
        { DOWN,  {0,  1,  2,  3}  },
        { LEFT,  {4,  5,  6,  7}  },
        { RIGHT, {8,  9, 10, 11}  },
        { UP,    {12, 13, 14, 15} },
    };
    mGameState.player = new Entity(
        {spawnX, spawnY}, {96.0f, 96.0f},
        "assets/characters/player.png",
        ATLAS, {4, 4}, playerAtlas, PLAYER
    );
    mGameState.player->setColliderDimensions({48.0f, 48.0f});
    mGameState.player->setSpeed(180);
    mGameState.player->setAcceleration({0.0f, 0.0f});

    float cartiX = mOrigin.x - (LEVEL_B_WIDTH  * TILE_DIMENSION) / 2.0f
                   + 20 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float cartiY = mOrigin.y - (LEVEL_B_HEIGHT * TILE_DIMENSION) / 2.0f
                   + 3 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;

    std::map<Direction, std::vector<int>> cartiAtlas = {
        { DOWN,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { LEFT,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { RIGHT, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { UP,    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
    };
    mCarti = new Entity(
        {cartiX, cartiY}, {256.0f, 256.0f},
        "assets/characters/carti-idle.png",
        ATLAS, {7, 7}, cartiAtlas, NPC
    );
    mCarti->setDirection(DOWN);
    mCarti->setFrameSpeed(8);
    mCartiPos = {cartiX, cartiY};

    mGameState.numEnemies = 0;
    mGameState.enemies    = nullptr;

    mDialogueActive       = true;
    mDialogueLine         = 0;
    mDialogueTimer        = 0.0f;
    mDialoguePhase        = 0;
    mCartiBattleTriggered = false;
}

void LevelB::update(float deltaTime)
{
    UpdateMusicStream(mBgMusic);

    if (mDialogueActive)
    {
        mDialogueTimer += deltaTime;
        int count = (mDialoguePhase == 0) ? kIntroCount : kCartiCount;
        if (IsKeyPressed(KEY_ENTER) || mDialogueTimer >= 2.0f)
        {
            mDialogueLine++;
            mDialogueTimer = 0.0f;
            if (mDialogueLine >= count)
            {
                mDialogueActive = false;
                mDialogueLine   = 0;
                if (mDialoguePhase == 1)
                    mGameState.nextSceneID = 4;
            }
        }
        if (mDialoguePhase == 0) return;
    }

    Vector2 pos  = mGameState.player->getPosition();
    int     spd  = mGameState.player->getSpeed();
    Vector2 move = {0.0f, 0.0f};

    if (IsKeyDown(KEY_A)) { move.x = -1; mGameState.player->setDirection(LEFT);  }
    if (IsKeyDown(KEY_D)) { move.x =  1; mGameState.player->setDirection(RIGHT); }
    if (IsKeyDown(KEY_W)) { move.y = -1; mGameState.player->setDirection(UP);    }
    if (IsKeyDown(KEY_S)) { move.y =  1; mGameState.player->setDirection(DOWN);  }

    float len = sqrtf(move.x * move.x + move.y * move.y);
    if (len > 0) { move.x /= len; move.y /= len; mGameState.player->animate(deltaTime); }

    float mapLeft = mOrigin.x - (LEVEL_B_WIDTH  * TILE_DIMENSION) / 2.0f;
    float mapTop  = mOrigin.y - (LEVEL_B_HEIGHT * TILE_DIMENSION) / 2.0f;
    float half    = 24.0f;

    auto isSolid = [](int t) { return t == 1; };
    auto tileAt  = [&](float wx, float wy) -> int {
        int col = (int)((wx - mapLeft) / TILE_DIMENSION);
        int row = (int)((wy - mapTop)  / TILE_DIMENSION);
        if (col < 0 || col >= LEVEL_B_WIDTH || row < 0 || row >= LEVEL_B_HEIGHT) return 1;
        return (int)mLevelData[row * LEVEL_B_WIDTH + col];
    };

    float nx = pos.x + move.x * spd * deltaTime;
    if (isSolid(tileAt(nx-half,pos.y-half)) || isSolid(tileAt(nx-half,pos.y+half)) ||
        isSolid(tileAt(nx+half,pos.y-half)) || isSolid(tileAt(nx+half,pos.y+half)))
        nx = pos.x;

    float ny = pos.y + move.y * spd * deltaTime;
    if (isSolid(tileAt(nx-half,ny-half)) || isSolid(tileAt(nx-half,ny+half)) ||
        isSolid(tileAt(nx+half,ny-half)) || isSolid(tileAt(nx+half,ny+half)))
        ny = pos.y;

    mGameState.player->setPosition({nx, ny});

    if (!mDialogueActive && !mCartiBattleTriggered && IsKeyPressed(KEY_ENTER))
    {
        float dx = nx - mCartiPos.x, dy = ny - mCartiPos.y;
        if (sqrtf(dx*dx + dy*dy) < TALK_RADIUS)
        {
            mDialogueActive       = true;
            mDialogueLine         = 0;
            mDialogueTimer        = 0.0f;
            mDialoguePhase        = 1;
            mCartiBattleTriggered = true;
        }
    }

    mCarti->animate(deltaTime);

    Vector2 playerPos = mGameState.player->getPosition();
    float lightPos[2] = { playerPos.x, playerPos.y };
    SetShaderValue(mShader, mLightLoc, lightPos, SHADER_UNIFORM_VEC2);
}

void LevelB::render()
{
    ClearBackground({5, 5, 15, 255});

    float mapLeft = mOrigin.x - (LEVEL_B_WIDTH  * TILE_DIMENSION) / 2.0f;
    float mapTop  = mOrigin.y - (LEVEL_B_HEIGHT * TILE_DIMENSION) / 2.0f;
    float mapW    = LEVEL_B_WIDTH  * TILE_DIMENSION;
    float mapH    = LEVEL_B_HEIGHT * TILE_DIMENSION;

    BeginShaderMode(mShader);
    DrawTexturePro(
        mBgTexture,
        {0, 0, (float)mBgTexture.width, (float)mBgTexture.height},
        {mapLeft, mapTop, mapW, mapH},
        {0, 0}, 0.0f, WHITE
    );
    mCarti->render();
    mGameState.player->render();
    EndShaderMode();

    if (!mDialogueActive && !mCartiBattleTriggered)
    {
        Vector2 pp = mGameState.player->getPosition();
        float dx = pp.x - mCartiPos.x, dy = pp.y - mCartiPos.y;
        if (sqrtf(dx*dx + dy*dy) < TALK_RADIUS)
        {
            DrawTexturePro(mSpeechBubble,
                {0, 0, (float)mSpeechBubble.width, (float)mSpeechBubble.height},
                {mCartiPos.x - 30, mCartiPos.y - 120, 60, 60},
                {0, 0}, 0.0f, WHITE);
        }
    }

    if (mDialogueActive)
    {
        const DialogueLine *lines = (mDialoguePhase == 0) ? kIntro : kCarti;
        const char *speaker = lines[mDialogueLine].speaker;
        const char *text    = lines[mDialogueLine].text;
        DrawRectangle(20, 450, 760, 130, {0, 0, 0, 220});
        DrawRectangleLines(20, 450, 760, 130, {255, 80, 255, 255});
        Color nameColor = (speaker[0] == 'Y') ? YELLOW
                        : (speaker[0] == 'T') ? (Color){0,255,200,255}
                        :                       (Color){255,80,255,255};
        DrawText(speaker, 36, 458, 22, nameColor);
        DrawText(text,    36, 490, 16, WHITE);
        DrawText("[ENTER] skip", 580, 565, 13, {150, 150, 150, 255});
    }
}

void LevelB::shutdown()
{
    StopMusicStream(mBgMusic);
    UnloadMusicStream(mBgMusic);
    if (mBgTexture.id != 0)    UnloadTexture(mBgTexture);
    if (mSpeechBubble.id != 0) UnloadTexture(mSpeechBubble);
    UnloadShader(mShader);
    if (mCarti)             { delete mCarti;               mCarti             = nullptr; }
    if (mGameState.player)  { delete mGameState.player;    mGameState.player  = nullptr; }
    if (mGameState.enemies) { delete[] mGameState.enemies; mGameState.enemies = nullptr; }
    if (mGameState.map)     { delete mGameState.map;       mGameState.map     = nullptr; }
}