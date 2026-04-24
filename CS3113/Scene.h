#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

struct GameState
{
    Entity *player  = nullptr;
    Entity *enemies = nullptr;
    int     numEnemies = 0;
    Map    *map     = nullptr;

    Music bgm;
    Sound jumpSound;
    Sound hitSound;
    Sound deathSound;

    int nextSceneID = 0;  // 0 = don't switch
};

class Scene
{
protected:
    GameState   mGameState;
    Vector2     mOrigin;
    const char *mBGColourHexCode = "#000000";

public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);
    virtual ~Scene() {}
    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState   getState()           const { return mGameState;       }
    GameState  *getStatePtr()              { return &mGameState;      }
    Vector2     getOrigin()          const { return mOrigin;          }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif // SCENE_H
