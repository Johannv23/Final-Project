#include "Entity.h"


Entity::Entity() :
    mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f},
    mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
    mScale {DEFAULT_SIZE, DEFAULT_SIZE},
    mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE},
    mTexture {NULL}, mTextureType {SINGLE}, mAngle {0.0f},
    mSpriteSheetDimensions {}, mDirection {RIGHT},
    mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {0},
    mEntityType {NONE} {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
    EntityType entityType) :
    mPosition {position}, mVelocity {0.0f, 0.0f},
    mAcceleration {0.0f, 0.0f}, mScale {scale}, mMovement {0.0f, 0.0f},
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)},
    mTextureType {SINGLE}, mDirection {RIGHT}, mAnimationAtlas {{}},
    mAnimationIndices {}, mFrameSpeed {0}, mSpeed {DEFAULT_SPEED},
    mAngle {0.0f}, mEntityType {entityType} {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
    TextureType textureType, Vector2 spriteSheetDimensions,
    std::map<Direction, std::vector<int>> animationAtlas,
    EntityType entityType) :
    mPosition {position}, mVelocity {0.0f, 0.0f},
    mAcceleration {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, mScale {scale},
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)},
    mTextureType {ATLAS}, mSpriteSheetDimensions {spriteSheetDimensions},
    mAnimationAtlas {animationAtlas}, mDirection {RIGHT},
    mAnimationIndices {animationAtlas.at(RIGHT)},
    mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle {0.0f},
    mSpeed {DEFAULT_SPEED}, mEntityType {entityType} {}

Entity::~Entity() { UnloadTexture(mTexture); }


bool Entity::isColliding(Entity *other) const
{
    if (!other->isActive() || other == this) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) -
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) -
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    return (xDistance < 0.0f && yDistance < 0.0f);
}

void Entity::checkCollisionY(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *other = &collidableEntities[i];
        if (!isColliding(other)) continue;

        float yDist    = fabs(mPosition.y - other->mPosition.y);
        float yOverlap = fabs(yDist - (mColliderDimensions.y / 2.0f) -
                         (other->mColliderDimensions.y / 2.0f));

        if (mVelocity.y > 0)
        {
            mPosition.y       -= yOverlap;
            mVelocity.y        = 0;
            mIsCollidingBottom = true;
        }
        else if (mVelocity.y < 0)
        {
            mPosition.y     += yOverlap;
            mVelocity.y      = 0;
            mIsCollidingTop  = true;
        }
    }
}

void Entity::checkCollisionX(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *other = &collidableEntities[i];
        if (!isColliding(other)) continue;

        float yDist    = fabs(mPosition.y - other->mPosition.y);
        float yOverlap = fabs(yDist - (mColliderDimensions.y / 2.0f) -
                         (other->mColliderDimensions.y / 2.0f));

        if (yOverlap < Y_COLLISION_THRESHOLD) continue;

        float xDist    = fabs(mPosition.x - other->mPosition.x);
        float xOverlap = fabs(xDist - (mColliderDimensions.x / 2.0f) -
                         (other->mColliderDimensions.x / 2.0f));

        if (mVelocity.x > 0)
        {
            mPosition.x      -= xOverlap;
            mVelocity.x       = 0;
            mIsCollidingRight = true;
        }
        else if (mVelocity.x < 0)
        {
            mPosition.x     += xOverlap;
            mVelocity.x      = 0;
            mIsCollidingLeft = true;
        }
    }
}

void Entity::checkCollisionY(Map *map)
{
    if (map == nullptr) return;

    Vector2 topCentreProbe    = { mPosition.x,                              mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topLeftProbe      = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topRightProbe     = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };

    Vector2 bottomCentreProbe = { mPosition.x,                              mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomLeftProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomRightProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    if ((map->isSolidTileAt(topCentreProbe,    &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topLeftProbe,      &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topRightProbe,     &xOverlap, &yOverlap))
         && mVelocity.y < 0.0f)
    {
        mPosition.y    += yOverlap * 1.01f;
        mVelocity.y     = 0.0f;
        mIsCollidingTop = true;
    }

    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomLeftProbe,   &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomRightProbe,  &xOverlap, &yOverlap))
         && mVelocity.y > 0.0f)
    {
        mPosition.y       -= yOverlap * 1.01f;
        mVelocity.y        = 0.0f;
        mIsCollidingBottom = true;
    }
}

void Entity::checkCollisionX(Map *map)
{
    if (map == nullptr) return;

    Vector2 leftCentreProbe  = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y                               };
    Vector2 leftTopProbe     = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 leftBottomProbe  = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };

    Vector2 rightCentreProbe = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y                               };
    Vector2 rightTopProbe    = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 rightBottomProbe = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    if ((map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(rightTopProbe,    &xOverlap, &yOverlap) ||
         map->isSolidTileAt(rightBottomProbe, &xOverlap, &yOverlap))
         && mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x      -= xOverlap * 1.01f;
        mVelocity.x       = 0.0f;
        mIsCollidingRight = true;
    }

    if ((map->isSolidTileAt(leftCentreProbe,  &xOverlap, &yOverlap) ||
         map->isSolidTileAt(leftTopProbe,     &xOverlap, &yOverlap) ||
         map->isSolidTileAt(leftBottomProbe,  &xOverlap, &yOverlap))
         && mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x    += xOverlap * 1.01f;
        mVelocity.x     = 0.0f;
        mIsCollidingLeft = true;
    }
}


void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mDirection);
    mAnimationTime   += deltaTime;

    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;
        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}


void Entity::AIWander()
{
    moveLeft();
}

void Entity::AIFollow(Entity *target)
{
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 300.0f)
            mAIState = WALKING;
        break;

    case WALKING:
        if (mPosition.x > target->getPosition().x) moveLeft();
        else                                        moveRight();
        break;

    default:
        break;
    }
}

void Entity::AILerp(Entity *target, float deltaTime)
{
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 350.0f)
            mAIState = FOLLOWING;
        break;

    case FOLLOWING:
        {
            float t = mLerpFactor * deltaTime;
            mPosition.x = mPosition.x + (target->getPosition().x - mPosition.x) * t;
            mPosition.y = mPosition.y + (target->getPosition().y - mPosition.y) * t;

            if (target->getPosition().x < mPosition.x) setDirection(LEFT);
            else                                        setDirection(RIGHT);
        }
        break;

    default:
        break;
    }
}

void Entity::AIActivate(Entity *target, float deltaTime)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;

    case FOLLOWER:
        AIFollow(target);
        break;

    case LERPER:
        AILerp(target, deltaTime);
        break;

    default:
        break;
    }
}


void Entity::update(float deltaTime, Entity *player, Map *map,
    Entity *collidableEntities, int collisionCheckCount)
{
    if (mEntityStatus == INACTIVE) return;

    if (mEntityType == NPC) AIActivate(player, deltaTime);

    resetColliderFlags();

    // LERPER moves via direct position — skip normal physics pipeline
    if (mAIType != LERPER)
    {
        mVelocity.x  = mMovement.x * mSpeed;
        mVelocity.x += mAcceleration.x * deltaTime;
        mVelocity.y += mAcceleration.y * deltaTime;

        if (mIsJumping)
        {
            mIsJumping   = false;
            mVelocity.y -= mJumpingPower;
        }

        mPosition.x += mVelocity.x * deltaTime;
        checkCollisionX(collidableEntities, collisionCheckCount);
        checkCollisionX(map);

        mPosition.y += mVelocity.y * deltaTime;
        checkCollisionY(collidableEntities, collisionCheckCount);
        checkCollisionY(map);
    }

    if (mTextureType == ATLAS)
    {
        bool shouldAnimate = (mAIType == LERPER) ||
                             (GetLength(mMovement) != 0 && mIsCollidingBottom);
        if (shouldAnimate) animate(deltaTime);
    }
}


void Entity::render()
{
    if (mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
    case SINGLE:
        textureArea = {
            0.0f, 0.0f,
            (float)mTexture.width,
            (float)mTexture.height
        };
        break;

    case ATLAS:
        textureArea = getUVRectangle(
            &mTexture,
            mAnimationIndices[mCurrentFrameIndex],
            (int)mSpriteSheetDimensions.y,  // rows
            (int)mSpriteSheetDimensions.x   // cols
        );
        break;

    default:
        break;
    }

    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        (float)mScale.x,
        (float)mScale.y
    };

    Vector2 originOffset = {
        (float)mScale.x / 2.0f,
        (float)mScale.y / 2.0f
    };

    DrawTexturePro(mTexture, textureArea, destinationArea, originOffset,
                   mAngle, WHITE);
}

void Entity::displayCollider()
{
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,
        mPosition.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y
    };

    DrawRectangleLines(
        colliderBox.x, colliderBox.y,
        colliderBox.width, colliderBox.height,
        GREEN
    );
}