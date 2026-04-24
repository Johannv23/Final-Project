#include "LevelA.h"

struct DialogueLine { const char *speaker; const char *text; };

static const DialogueLine kBartender0[] = {
    { "YOU",        "How do I make my way up in this world?"                           },
    { "BARTENDER",  "Ha. You really just walked in here asking that?"                  },
    { "BARTENDER",  "Look around you. You see that stage up there?"                    },
    { "BARTENDER",  "That's Offset's house. He runs this whole scene."                 },
    { "BARTENDER",  "Nobody gets on that stage without going through him first."       },
    { "YOU",        "So how do I get to him?"                                          },
    { "BARTENDER",  "You don't. Not looking like that."                                },
    { "BARTENDER",  "The Champ only takes you seriously if you got the drip to match." },
    { "BARTENDER",  "Go introduce yourself to him first. See what he says."            },
    { "BARTENDER",  "Then come back and talk to me."                                   },
};
static const int kBartender0Count = 10;

static const DialogueLine kOffset1[] = {
    { "YOU",    "Aye, what's good. I'm tryna get on this stage."               },
    { "OFFSET", "*laughs* Nah. You ain't ready."                               },
    { "OFFSET", "You walkin in here with no drip, no rep, nothing."            },
    { "OFFSET", "Come back when you look like you belong here."                },
    { "YOU",    "Where do I even start?"                                       },
    { "OFFSET", "Talk to the bartender. He'll point you in the right direction."},
};
static const int kOffset1Count = 6;

static const DialogueLine kBartender2[] = {
    { "YOU",        "Offset said talk to you about my drip."                     },
    { "BARTENDER",  "Yeah I figured he'd send you over."                         },
    { "BARTENDER",  "I got a fit that'll get you in the room. $200."             },
    { "BARTENDER",  "You ain't got that kind of money on you though, do you?"    },
    { "YOU",        "Nah, not yet."                                               },
    { "BARTENDER",  "There's a guy in the VIP. Big coat, orange hat. He gambles."},
    { "BARTENDER",  "Beat him and you'll have more than enough. Go handle that." },
};
static const int kBartender2Count = 7;

static const DialogueLine kGambler3[] = {
    { "YOU",      "Heard you run games up here."                               },
    { "GAMBLER",  "*rolls dice* Who told you that?"                            },
    { "GAMBLER",  "Aight fine. Tic Tac Toe. $200 on the line."                },
    { "GAMBLER",  "You win, the money's yours. You lose, you leave me alone."  },
    { "YOU",      "Let's run it."                                              },
};
static const int kGambler3Count = 5;

static const DialogueLine kGamblerWin[] = {
    { "GAMBLER",  "..."                                          },
    { "GAMBLER",  "Aight. You got lucky. Here's your $200."      },
    { "YOU",      "Appreciate it. Now back to the bartender."    },
};

static const DialogueLine kBartender5[] = {
    { "YOU",        "Yo I beat him. Here's the $200."                              },
    { "BARTENDER",  "Aight no cap, I didn't think you'd actually do it."           },
    { "BARTENDER",  "Here's the fit. Go get changed."                              },
    { "YOU",        "*puts on the new fit*"                                        },
    { "BARTENDER",  "Yeah. Now you look like you belong here."                     },
    { "BARTENDER",  "Go back up to Offset. He'll see you now."                     },
};

static const DialogueLine kOffset6[] = {
    { "YOU",    "Aye. I got the drip. I'm ready."                             },
    { "OFFSET", "..."                                                          },
    { "OFFSET", "Alright. I'll give you that."                                },
    { "OFFSET", "But looking good and rapping good are two different things."  },
    { "OFFSET", "Step on the stage. Let's see what you actually got."         },
    { "YOU",    "Say less."                                                    },
};

static const int kOffset6Count = 6;
static const int kBartender5Count = 6;
static const int kGamblerWinCount = 3;

static int dialogueCount(int questStep)
{
    switch (questStep)
    {
        case 0: return kBartender0Count;
        case 1: return kOffset1Count;
        case 2: return kBartender2Count;
        case 3: return kGambler3Count;
        case 4: return kGamblerWinCount;
        case 5: return kBartender5Count;
        case 6: return kOffset6Count;
        default: return 0;
    }
}

static const DialogueLine *dialogueLines(int questStep)
{
    switch (questStep)
    {
        case 0: return kBartender0;
        case 1: return kOffset1;
        case 2: return kBartender2;
        case 3: return kGambler3;
        case 4: return kGamblerWin;
        case 5: return kBartender5;
        case 6: return kOffset6;
        default: return nullptr;
    }
}

int LevelA::tttCheckWinner()
{
    const int lines[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},
        {0,3,6},{1,4,7},{2,5,8},
        {0,4,8},{2,4,6}
    };
    for (auto &l : lines)
        if (mTTTBoard[l[0]] && mTTTBoard[l[0]]==mTTTBoard[l[1]] && mTTTBoard[l[1]]==mTTTBoard[l[2]])
            return mTTTBoard[l[0]];
    for (int i = 0; i < 9; i++) if (mTTTBoard[i] == 0) return 0;
    return 3;
}

void LevelA::tttAIMove()
{
    int best = -1;
    for (int i = 0; i < 9; i++)
    {
        if (mTTTBoard[i] != 0) continue;
        mTTTBoard[i] = 2;
        int w = tttCheckWinner();
        mTTTBoard[i] = 0;
        if (w != 2) { best = i; break; }
    }
    if (best == -1) return;
    mTTTBoard[best] = 2;
}

void LevelA::tttReset()
{
    for (int i = 0; i < 9; i++) mTTTBoard[i] = 0;
    mTTTCursor     = 0;
    mTTTPlayerTurn = true;
    mTTTOver       = false;
    mTTTWinner     = 0;
    mTTTAIThinking = false;
    mTTTAITimer    = 0.0f;
}

LevelA::LevelA() : Scene{{0.0f, 0.0f}, "#000000"} {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}
LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
    mBgMusic = LoadMusicStream("assets/audio/club_bg_music.wav");
    PlayMusicStream(mBgMusic);
    mGameState.nextSceneID = 0;

    mBgTexture    = LoadTexture("assets/tiles/club_bg.png");
    mSpeechBubble = LoadTexture("assets/characters/speechbubble.png");

    mGameState.map = new Map(
        LEVEL_A_WIDTH, LEVEL_A_HEIGHT,
        (unsigned int *) mLevelData,
        "assets/tiles/club_tiles.png",
        TILE_DIMENSION, 16, 16, mOrigin
    );

    float spawnX = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f
                   + 12 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float spawnY = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f
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

    float stageX = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f
                   + 18 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float stageY = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f
                   + 4 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    std::map<Direction, std::vector<int>> offsetAtlas = {
        { DOWN,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24} },
        { LEFT,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24} },
        { RIGHT, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24} },
        { UP,    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24} },
    };
    mOffsetEnemy = new Entity(
        {stageX, stageY}, {160.0f, 160.0f},
        "assets/characters/Offset.png",
        ATLAS, {5, 5}, offsetAtlas, NPC
    );
    mOffsetEnemy->setDirection(DOWN);
    mOffsetEnemy->setFrameSpeed(10);
    mOffsetPos = {stageX, stageY};

    mGameState.numEnemies = 0;
    mGameState.enemies    = nullptr;

    float bartenderX = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f
                       + 11 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float bartenderY = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f
                       + 7.5 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    std::map<Direction, std::vector<int>> bartenderAtlas = {
        { DOWN,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { LEFT,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { RIGHT, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { UP,    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
    };
    mBartender = new Entity(
        {bartenderX, bartenderY}, {160.0f, 160.0f},
        "assets/characters/Bartender-idle.png",
        ATLAS, {7, 7}, bartenderAtlas, NPC
    );
    mBartender->setDirection(DOWN);
    mBartender->setFrameSpeed(10);
    mBartenderPos = {bartenderX, bartenderY};

    float gamblerX = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f
                     + 2 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float gamblerY = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f
                     + 8 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    std::map<Direction, std::vector<int>> gamblerAtlas = {
        { DOWN,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { LEFT,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { RIGHT, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { UP,    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
    };
    mVIPGambler = new Entity(
        {gamblerX, gamblerY}, {160.0f, 160.0f},
        "assets/characters/gambler-sitting_idle_animation.png",
        ATLAS, {7, 7}, gamblerAtlas, NPC
    );
    mVIPGambler->setDirection(DOWN);
    mVIPGambler->setFrameSpeed(10);
    mGamblerPos = {gamblerX, gamblerY};

    float smokingX = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f
                     + 6 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float smokingY = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f
                     + 4 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    std::map<Direction, std::vector<int>> smokingAtlas = {
        { DOWN,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { LEFT,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { RIGHT, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { UP,    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
    };
    mSmokingGroup = new Entity(
        {smokingX, smokingY}, {560.0f, 560.0f},
        "assets/characters/group_smoking.png",
        ATLAS, {7, 7}, smokingAtlas, NPC
    );
    mSmokingGroup->setDirection(DOWN);
    mSmokingGroup->setFrameSpeed(10);

    float moshX = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f
                  + 18 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float moshY = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f
                  + 11 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    std::map<Direction, std::vector<int>> moshAtlas = {
        { DOWN,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { LEFT,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { RIGHT, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { UP,    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
    };
    mMoshPit = new Entity(
        {moshX, moshY}, {560.0f, 560.0f},
        "assets/characters/moshpit-moshinggggpit.png",
        ATLAS, {7, 7}, moshAtlas, NPC
    );
    mMoshPit->setDirection(DOWN);
    mMoshPit->setFrameSpeed(10);

    float drinkersX = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f
                      + 4 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float drinkersY = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f
                      + 15 * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    std::map<Direction, std::vector<int>> drinkersAtlas = {
        { DOWN,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { LEFT,  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { RIGHT, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
        { UP,    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48} },
    };
    mDrinkers = new Entity(
        {drinkersX, drinkersY}, {260.0f, 260.0f},
        "assets/characters/drinkers-drinking_idle.png",
        ATLAS, {7, 7}, drinkersAtlas, NPC
    );
    mDrinkers->setDirection(DOWN);
    mDrinkers->setFrameSpeed(10);

    tttReset();
}

void LevelA::update(float deltaTime)
{
    UpdateMusicStream(mBgMusic);

    if (mTTTActive)
    {
        if (!mTTTOver)
        {
            if (mTTTPlayerTurn)
            {
                if (IsKeyPressed(KEY_RIGHT) && (mTTTCursor % 3) < 2) mTTTCursor++;
                if (IsKeyPressed(KEY_LEFT)  && (mTTTCursor % 3) > 0) mTTTCursor--;
                if (IsKeyPressed(KEY_DOWN)  && mTTTCursor < 6)       mTTTCursor += 3;
                if (IsKeyPressed(KEY_UP)    && mTTTCursor > 2)       mTTTCursor -= 3;

                if (IsKeyPressed(KEY_ENTER) && mTTTBoard[mTTTCursor] == 0)
                {
                    mTTTBoard[mTTTCursor] = 1;
                    mTTTWinner = tttCheckWinner();
                    if (mTTTWinner == 0)
                    {
                        mTTTPlayerTurn = false;
                        mTTTAIThinking = true;
                        mTTTAITimer    = 0.0f;
                    }
                    else mTTTOver = true;
                }
            }
            else if (mTTTAIThinking)
            {
                mTTTAITimer += deltaTime;
                if (mTTTAITimer >= 0.8f)
                {
                    tttAIMove();
                    mTTTAIThinking = false;
                    mTTTWinner     = tttCheckWinner();
                    if (mTTTWinner == 0) mTTTPlayerTurn = true;
                    else                 mTTTOver = true;
                }
            }
        }
        else
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                mTTTActive = false;
                if (mTTTWinner == 1)
                {
                    mCash = 200;
                    mQuestStep      = 4;
                    mDialogueActive = true;
                    mDialogueLine   = 0;
                    mDialogueTimer  = 0.0f;
                }
                else
                {
                    tttReset();
                    mTTTActive = true;
                }
            }
        }
        return;
    }

    if (mDialogueActive)
    {
        mDialogueTimer += deltaTime;
        if (IsKeyPressed(KEY_ENTER) || mDialogueTimer >= 2.0f)
        {
            mDialogueLine++;
            mDialogueTimer = 0.0f;
            if (mDialogueLine >= dialogueCount(mQuestStep))
            {
                mDialogueActive = false;
                mDialogueLine   = 0;
                if (mQuestStep == 3)
                {
                    mTTTActive = true;
                    tttReset();
                }
                else if (mQuestStep == 4)
                {
                    mQuestStep = 5;
                }
                else if (mQuestStep == 5)
                {
                    mGameState.player->setTexture("assets/characters/mcdrippedout.png");
                    mGameState.player->setSpriteSheetDimensions({4, 4});
                    mCash = 0;
                    mGameState.player->setAnimationAtlas({
                        { DOWN,  {0,  1,  2,  3}  },
                        { LEFT,  {4,  5,  6,  7}  },
                        { RIGHT, {8,  9, 10, 11}  },
                        { UP,    {12, 13, 14, 15} },
                    });
                    mQuestStep = 6;
                }
                else if (mQuestStep == 6)
                {
                    mQuestStep = 7;
                    mGameState.nextSceneID = 3;
                }
                else mQuestStep++;
            }
        }
        return;
    }

    Vector2 pos  = mGameState.player->getPosition();
    int     spd  = mGameState.player->getSpeed();
    Vector2 move = {0.0f, 0.0f};

    if (IsKeyDown(KEY_A)) { move.x = -1; mGameState.player->setDirection(LEFT);  }
    if (IsKeyDown(KEY_D)) { move.x =  1; mGameState.player->setDirection(RIGHT); }
    if (IsKeyDown(KEY_W)) { move.y = -1; mGameState.player->setDirection(UP);    }
    if (IsKeyDown(KEY_S)) { move.y =  1; mGameState.player->setDirection(DOWN);  }

    float len = sqrtf(move.x*move.x + move.y*move.y);
    if (len > 0) { move.x /= len; move.y /= len; mGameState.player->animate(deltaTime); }

    float mapLeft = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f;
    float mapTop  = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f;
    float half    = 24.0f;

    auto isSolid = [](int t) { return t == 1; };
    auto tileAt  = [&](float wx, float wy) -> int {
        int col = (int)((wx - mapLeft) / TILE_DIMENSION);
        int row = (int)((wy - mapTop)  / TILE_DIMENSION);
        if (col < 0 || col >= LEVEL_A_WIDTH || row < 0 || row >= LEVEL_A_HEIGHT) return 1;
        return (int)mLevelData[row * LEVEL_A_WIDTH + col];
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

    if (IsKeyPressed(KEY_ENTER))
    {
        float dxBt = nx - mBartenderPos.x, dyBt = ny - mBartenderPos.y;
        float dxOf = nx - mOffsetPos.x,    dyOf = ny - mOffsetPos.y;
        float dxGb = nx - mGamblerPos.x,   dyGb = ny - mGamblerPos.y;

        if      (mQuestStep == 0 && sqrtf(dxBt*dxBt+dyBt*dyBt) < TALK_RADIUS)
        { mDialogueActive = true; mDialogueLine = 0; mDialogueTimer = 0.0f; }
        else if (mQuestStep == 1 && sqrtf(dxOf*dxOf+dyOf*dyOf) < TALK_RADIUS)
        { mDialogueActive = true; mDialogueLine = 0; mDialogueTimer = 0.0f; }
        else if (mQuestStep == 2 && sqrtf(dxBt*dxBt+dyBt*dyBt) < TALK_RADIUS)
        { mDialogueActive = true; mDialogueLine = 0; mDialogueTimer = 0.0f; }
        else if (mQuestStep == 3 && sqrtf(dxGb*dxGb+dyGb*dyGb) < TALK_RADIUS)
        { mDialogueActive = true; mDialogueLine = 0; mDialogueTimer = 0.0f; }
        else if (mQuestStep == 5 && sqrtf(dxBt*dxBt+dyBt*dyBt) < TALK_RADIUS)
        { mDialogueActive = true; mDialogueLine = 0; mDialogueTimer = 0.0f; }
        else if (mQuestStep == 6 && sqrtf(dxOf*dxOf+dyOf*dyOf) < TALK_RADIUS)
        { mDialogueActive = true; mDialogueLine = 0; mDialogueTimer = 0.0f; }
    }


    mOffsetEnemy->animate(deltaTime);
    mBartender->animate(deltaTime);

    mPourTimer += deltaTime;
    if (!mPouring && mPourTimer >= POUR_INTERVAL) {
        mPouring = true; mPourTimer = 0.0f;
        mBartender->setTexture("assets/characters/Bartender-idle_pourup.png");
        mBartender->setSpriteSheetDimensions({7, 7});
    } else if (mPouring && mPourTimer >= POUR_DURATION) {
        mPouring = false; mPourTimer = 0.0f;
        mBartender->setTexture("assets/characters/Bartender-idle.png");
        mBartender->setSpriteSheetDimensions({7, 7});
    }

    mVIPGambler->animate(deltaTime);
    mSmokingGroup->animate(deltaTime);
    mMoshPit->animate(deltaTime);
    mDrinkers->animate(deltaTime);
}

void LevelA::render()
{
    ClearBackground({10, 8, 20, 255});

    float mapLeft = mOrigin.x - (LEVEL_A_WIDTH  * TILE_DIMENSION) / 2.0f;
    float mapTop  = mOrigin.y - (LEVEL_A_HEIGHT * TILE_DIMENSION) / 2.0f;

    DrawTexturePro(
        mBgTexture,
        {0, 0, (float)mBgTexture.width, (float)mBgTexture.height},
        {mapLeft, mapTop, LEVEL_A_WIDTH*TILE_DIMENSION, LEVEL_A_HEIGHT*TILE_DIMENSION},
        {0, 0}, 0.0f, WHITE
    );


    mDrinkers->render();
    mMoshPit->render();
    mSmokingGroup->render();
    mVIPGambler->render();
    mBartender->render();
    mOffsetEnemy->render();
    mGameState.player->render();

    Vector2 pp = mGameState.player->getPosition();
    auto bubble = [&](Vector2 npcPos, int requiredStep) {
        float dx = pp.x - npcPos.x, dy = pp.y - npcPos.y;
        if (mQuestStep == requiredStep && !mDialogueActive && !mTTTActive &&
            sqrtf(dx*dx+dy*dy) < TALK_RADIUS)
        {
            DrawTexturePro(mSpeechBubble,
                {0,0,(float)mSpeechBubble.width,(float)mSpeechBubble.height},
                {npcPos.x-30, npcPos.y-120, 60, 60},
                {0,0}, 0.0f, WHITE);
        }
    };
    bubble(mBartenderPos, 0);
    bubble(mOffsetPos,    1);
    bubble(mBartenderPos, 2);
    bubble(mGamblerPos,   3);
    bubble(mBartenderPos, 5);
    bubble(mOffsetPos,    6);

    if (mDialogueActive)
    {
        const DialogueLine *lines = dialogueLines(mQuestStep);
        if (lines)
        {
            const char *speaker = lines[mDialogueLine].speaker;
            const char *text    = lines[mDialogueLine].text;
            DrawRectangle(20, 450, 760, 130, {0,0,0,220});
            DrawRectangleLines(20, 450, 760, 130, {255,80,255,255});
            Color nameColor = (speaker[0]=='Y') ? YELLOW : (Color){255,80,255,255};
            DrawText(speaker, 36, 458, 22, nameColor);
            DrawText(text,    36, 490, 16, WHITE);
            DrawText("[ENTER] skip", 580, 565, 13, {150,150,150,255});
        }
    }

    if (mTTTActive)
    {
        DrawRectangle(0, 0, 800, 600, {0,0,0,160});
        DrawRectangle(200, 80, 400, 440, {20,10,40,245});
        DrawRectangleLines(200, 80, 400, 440, {255,80,255,255});
        DrawText("TIC TAC TOE", 270, 100, 24, {255,80,255,255});
        DrawText("YOU vs GAMBLER", 255, 130, 18, WHITE);

        int bx = 255, by = 165, cell = 100;
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                int idx = r*3+c;
                int x = bx + c*cell, y = by + r*cell;
                Color bg = (idx == mTTTCursor && mTTTPlayerTurn && !mTTTOver)
                           ? (Color){60,20,80,255} : (Color){30,10,50,255};
                DrawRectangle(x, y, cell-4, cell-4, bg);
                DrawRectangleLines(x, y, cell-4, cell-4, {100,60,140,255});
                if (mTTTBoard[idx] == 1)
                    DrawText("X", x+30, y+25, 40, {255,255,0,255});
                else if (mTTTBoard[idx] == 2)
                    DrawText("O", x+28, y+25, 40, {255,80,80,255});
            }
        }

        if (mTTTOver)
        {
            const char *msg = (mTTTWinner==1) ? "YOU WIN! +$200" :
                              (mTTTWinner==2) ? "YOU LOSE. Try again." : "DRAW. Try again.";
            Color mc = (mTTTWinner==1) ? GREEN : RED;
            DrawText(msg, 230, 475, 20, mc);
            DrawText("[ENTER] Continue", 255, 505, 16, {150,150,150,255});
        }
        else if (!mTTTPlayerTurn)
            DrawText("Gambler is thinking...", 240, 480, 18, {150,150,150,255});
        else
            DrawText("Arrow keys to move  ENTER to place", 210, 480, 14, {180,180,180,255});
    }
}

void LevelA::shutdown()
{
    StopMusicStream(mBgMusic);
    UnloadMusicStream(mBgMusic);
    if (mBgTexture.id != 0)    UnloadTexture(mBgTexture);
    if (mSpeechBubble.id != 0) UnloadTexture(mSpeechBubble);
    if (mDrinkers)     { delete mDrinkers;     mDrinkers     = nullptr; }
    if (mMoshPit)      { delete mMoshPit;      mMoshPit      = nullptr; }
    if (mSmokingGroup) { delete mSmokingGroup; mSmokingGroup = nullptr; }
    if (mVIPGambler)   { delete mVIPGambler;   mVIPGambler   = nullptr; }
    if (mBartender)    { delete mBartender;    mBartender    = nullptr; }
    if (mOffsetEnemy)  { delete mOffsetEnemy;  mOffsetEnemy  = nullptr; }
    if (mGameState.player)  { delete mGameState.player;    mGameState.player  = nullptr; }
    if (mGameState.enemies) { delete[] mGameState.enemies; mGameState.enemies = nullptr; }
    if (mGameState.map)     { delete mGameState.map;       mGameState.map     = nullptr; }
}