#include "game.hpp"
#include "assets.hpp"

using namespace blit;

// Note: if want to go hires, with lores graphics:
// screen.sprite(uint16_t sprite, const Point &position, const Point &origin, float scale, uint8_t transform)
// origin can be Point(0,0) and transform can be SpriteTransform::NONE, scale is 2

// e.g. screen.sprite(id, Point(x, y), Point(0, 0), 2.0f, SpriteTransform::NONE

//#define RESET_SAVE_DATA


#define VERSION_MAJOR 0
#define VERSION_MINOR 6
#define VERSION_BUILD 10



const uint16_t SCREEN_WIDTH = 160;
const uint16_t SCREEN_HEIGHT = 120;

const uint8_t LEVEL_COUNT = 3;

const float FRAME_LENGTH = 0.15f;
const float TRANSITION_FRAME_LENGTH = 0.1f;
const float TRANSITION_CLOSE_LENGTH = 0.5f;

const uint8_t TILE_ID_EMPTY = 255;
const uint8_t TILE_ID_COIN = 112;
const uint8_t TILE_ID_PLAYER_1 = 64;
const uint8_t TILE_ID_PLAYER_2 = 68;
const uint8_t TILE_ID_HEART = 128;
const uint8_t TILE_ID_CAMERA = 253;
const uint8_t TILE_ID_TRANSITION = 224;
const uint8_t TILE_ID_FINISH = 144;
const uint8_t TILE_ID_LEVEL_TRIGGER = 132;
const uint8_t TILE_ID_ENEMY_1 = 80;
const uint8_t TILE_ID_ENEMY_2 = 84;
const uint8_t TILE_ID_ENEMY_3 = 88;
const uint8_t TILE_ID_ENEMY_4 = 92;
const uint8_t TILE_ID_ENEMY_5 = 96;
const uint8_t TILE_ID_ENEMY_PROJECTILE = 130;
const uint8_t TILE_ID_HUD_LIVES = 134;
const uint8_t TILE_ID_HUD_COINS = 136;
const uint8_t TILE_ID_HUD_ENEMIES_KILLED = 137;
const uint8_t TILE_ID_HUD_TIME_TAKEN = 138;

const float CAMERA_SCALE_X = 10.0f;
const float CAMERA_SCALE_Y = 5.0f;
const float CAMERA_PAN_TIME = 7.0f;
const float CAMERA_RESPAWN_LOCK_MIN = 1.0f;

const float LEVEL_DEATH_BOUNDARY_SCALE = 1.5f;

const uint8_t SPRITE_SIZE = 8;


const float ENTITY_DEATH_PARTICLE_COUNT = 100;
const float ENTITY_DEATH_PARTICLE_GRAVITY = 40.0f;
const float ENTITY_DEATH_PARTICLE_AGE = 0.8f;
const float ENTITY_DEATH_PARTICLE_SPEED = 60.0f;


const float GRAVITY = 600.0f;
const float GRAVITY_MAX = 200.0f;
const float PROJECTILE_GRAVITY = 55.0f;
const float PROJECTILE_GRAVITY_MAX = 100.0f;


const uint8_t PLAYER_START_LIVES = 3;
const uint8_t PLAYER_MAX_HEALTH = 3;

const float PLAYER_MAX_JUMP = 190.0f;
//const float  PLAYER_ATTACK_JUMP = 90.0f;
const float PLAYER_ATTACK_JUMP_SCALE = 0.5f;
const float PLAYER_ATTACK_JUMP_MIN = 70.0f;
const float PLAYER_MAX_SPEED = 85.0f;
const float PLAYER_IMMUNE_TIME = 2.5f;
const float PLAYER_ACCELERATION = 400.0f;

const float ENTITY_IDLE_SPEED = 40.0f;
const float ENTITY_PURSUIT_SPEED = 55.0f;
const float ENTITY_JUMP_SPEED = 160.0f;

const float RANGED_MAX_RANGE = 64.0f;
const float RANGED_RELOAD_TIME = 2.0f;
const float RANGED_PROJECTILE_X_VEL_SCALE = 0.8f;
const float RANGED_PROJECTILE_Y_VEL_SCALE = 0.5f;

const float PURSUIT_MAX_RANGE = 48.0f;

const float LEVEL_INFO_MAX_RANGE = SPRITE_SIZE * 4;

const float TEXT_FLASH_TIME = 0.8f;

const float TEXT_JUMP_VELOCITY = 80.0f;
const float TEXT_GRAVITY = 280.0f;

const uint8_t NO_LEVEL_SELECTED = 255;


const uint8_t MESSAGE_STRINGS_COUNT = 4;
const uint8_t INPUT_TYPE_COUNT = 2;

const uint16_t BYTE_SIZE = 256;


// NOTE: all positions (x,y) mark TOP LEFT corner of sprites

// NOTE: issue with rendering (tiles on left are a pixel out sometimes) is due to integers being added to floats. Something along lines of (int)floorf(camera.x) etc is recommended, but when I tried it I got strange results.
// If I implement that again, remember that all float calcs should be done, *then* casted, rather than casting each to int then adding etc


const uint8_t SCREEN_MID_WIDTH = SCREEN_WIDTH / 2;
const uint8_t SCREEN_MID_HEIGHT = SCREEN_HEIGHT / 2;


const uint8_t SPRITE_HALF = SPRITE_SIZE / 2;
const uint8_t SPRITE_QUARTER = SPRITE_SIZE / 4;

const uint16_t SCREEN_TILE_SIZE = (SCREEN_WIDTH / SPRITE_SIZE) * (SCREEN_HEIGHT / SPRITE_SIZE);

const uint8_t enemyHealths[] = { 1, 1, 1, 1, 2};

const std::vector<uint8_t> coinFrames = { TILE_ID_COIN, TILE_ID_COIN + 1, TILE_ID_COIN + 2, TILE_ID_COIN + 3, TILE_ID_COIN + 2, TILE_ID_COIN + 1 };

const std::vector<uint8_t> finishFrames = { TILE_ID_FINISH, TILE_ID_FINISH + 1, TILE_ID_FINISH + 2, TILE_ID_FINISH + 3, TILE_ID_FINISH + 4, TILE_ID_FINISH + 5 };

//const std::vector<uint8_t> transitionFrames = { TILE_ID_TRANSITION + 3, TILE_ID_TRANSITION + 2, TILE_ID_TRANSITION + 1, TILE_ID_TRANSITION, TILE_ID_TRANSITION + 1, TILE_ID_TRANSITION + 2, TILE_ID_TRANSITION + 3 };
const std::vector<uint8_t> transitionFramesClose = { TILE_ID_TRANSITION, TILE_ID_TRANSITION + 1, TILE_ID_TRANSITION + 2, TILE_ID_TRANSITION + 3, TILE_ID_TRANSITION + 4, TILE_ID_TRANSITION + 6, TILE_ID_TRANSITION + 7 };
const std::vector<uint8_t> transitionFramesOpen = { TILE_ID_TRANSITION + 6, TILE_ID_TRANSITION + 5, TILE_ID_TRANSITION + 4, TILE_ID_TRANSITION + 3, TILE_ID_TRANSITION + 2, TILE_ID_TRANSITION + 1, TILE_ID_TRANSITION};

const float parallaxFactorLayersX[2] = {
    0.4,
    0.2
};

const float parallaxFactorLayersY[2] = {
    0.4,
    0.2
};

const uint8_t* asset_levels[] = {
    asset_level0,
    asset_level1,
    asset_level2,
    asset_level_title,
    asset_level_char_select,
    asset_level_level_select
};



const std::string messageStrings[MESSAGE_STRINGS_COUNT][INPUT_TYPE_COUNT] = {
    {
        "Press A to Start",
        "Press U to Start"
    },
    {
        "Press A to Skip",
        "Press U to Skip"
    },
    {
        "Press A",
        "Press U"
    },
    {
        "Press Y to Resume",
        "Press P to Resume"
    }
};


uint16_t levelDeathBoundary;

float dt;
uint32_t lastTime = 0;

Surface* background_image = Surface::load(asset_background);

bool menuBack = false; // tells menu to go backwards instead of forwards.
bool gamePaused = false; // used for determining if game is paused or not.

bool cameraIntro = false;
bool cameraRespawn = false;
uint16_t cameraStartX, cameraStartY;
uint16_t playerStartX, playerStartY;


float textFlashTimer = 0.0f;
uint8_t playerSelected = 0;

uint8_t currentLevelNumber = NO_LEVEL_SELECTED;

// struct to handle level data header...
// this will probably need to be revisited/reworked if 32blit-tools has *any* kind of update to it...
#pragma pack(push,1)
struct TMX {
    char head[4];
    uint8_t empty_tile;
    uint16_t width;
    uint16_t height;
    uint16_t layers;
    uint8_t data[];
};
#pragma pack(pop)

enum class GameState {
    STATE_INPUT_SELECT,
    STATE_CHARACTER_SELECT,
    STATE_MENU,
    STATE_LEVEL_SELECT,
    STATE_IN_GAME,
    STATE_LOST,
    STATE_WON
};
GameState gameState = GameState::STATE_INPUT_SELECT;

enum InputType {
    CONTROLLER = 0,
    KEYBOARD = 1
    //NO_INPUT_TYPE
};

struct ButtonStates {
    uint8_t UP;
    uint8_t DOWN;
    uint8_t LEFT;
    uint8_t RIGHT;
    uint8_t A;
    uint8_t B;
    uint8_t X;
    uint8_t Y;
    uint8_t JOYSTICK;
};
ButtonStates buttonStates = { 0 };

struct GameSaveData {
    uint16_t version;
    uint8_t inputType;
} gameSaveData;

struct PlayerSaveData {
    uint8_t levelReached;
};
PlayerSaveData allPlayerSaveData[2];

struct LevelSaveData {
    uint8_t score;
    uint8_t enemiesKilled;
    float time;
};
LevelSaveData allLevelSaveData[2][LEVEL_COUNT];


// 0 slot is gameSaveData
// 1 slot is player 1 saveData
// 2..257 slots are player 1 levelData
// 258 slot is player 2 saveData
// 259..514 slots are player 2 levelData


uint16_t get_version() {
    return VERSION_MAJOR * 256 + VERSION_MINOR * 16 + VERSION_BUILD;
}


void save_game_data() {
    // Write save data
    write_save(gameSaveData);
}

void save_level_data(uint8_t playerID, uint8_t levelNumber) {
    // Write level data
    write_save(allLevelSaveData[playerID][levelNumber], (playerID * (BYTE_SIZE + 1)) + 1 + levelNumber + 1);
}

void save_player_data(uint8_t playerID) {
    // Write level data
    write_save(allPlayerSaveData[playerID], (playerID * (BYTE_SIZE + 1)) + 1);
}

LevelSaveData load_level_data(uint8_t playerID, uint8_t levelNumber) {
    LevelSaveData levelSaveData;
    if (read_save(levelSaveData, (playerID * (BYTE_SIZE + 1)) + 1 + levelNumber + 1)) {
        // Success
    }
    else {
        // Set some defaults
        levelSaveData.score = 0;
        levelSaveData.enemiesKilled = 0;
        levelSaveData.time = 0.0f; // If time == 0.0f, game realises that it's a N/A time
    }
    return levelSaveData;
}

PlayerSaveData load_player_data(uint8_t playerID) {
    PlayerSaveData playerSaveData;
    if (read_save(playerSaveData, (playerID * (BYTE_SIZE + 1)) + 1)) {
        // Success
    }
    else {
        // Set some defaults
        playerSaveData.levelReached = 0;
    }
    return playerSaveData;
}

void reset_save() {
    gameSaveData.version = get_version();
    gameSaveData.inputType = InputType::CONTROLLER;
    save_game_data();

    allPlayerSaveData[0].levelReached = 0;
    save_player_data(0);

    allPlayerSaveData[1].levelReached = 0;
    save_player_data(1);

    for (uint8_t i = 0; i < LEVEL_COUNT; i++) {
        allLevelSaveData[0][i].score = 0;
        allLevelSaveData[0][i].enemiesKilled = 0;
        allLevelSaveData[0][i].time = 0.0f;
        save_level_data(0, i);

        allLevelSaveData[1][i].score = 0;
        allLevelSaveData[1][i].enemiesKilled = 0;
        allLevelSaveData[1][i].time = 0.0f;
        save_level_data(1, i);
    }
}


struct LevelData {
    uint16_t levelWidth, levelHeight;
} levelData;

//struct PositionF {
//    float x, y;
//};
//
//struct PositionI {
//    int x, y;
//};


void render_sprite(uint8_t id, Point point) {
    screen.sprite(id, point, Point(0, 0), 1.0f, SpriteTransform::NONE);
}

void render_sprite(uint8_t id, Point point, SpriteTransform transform) {
    screen.sprite(id, point, Point(0, 0), 1.0f, transform);
}




class Colour {
public:
    uint8_t r, g, b, a;

    Colour() {
        r = g = b = a = 255;
    }

    Colour(uint8_t r, uint8_t g, uint8_t b) {
        this->r = r;
        this->g = g;
        this->b = b;
        a = 255;
    }

    Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
};

// Particle colours
const std::vector<Colour> playerDeathParticleColours[2] = {
    { Colour(255, 255, 242), Colour(255, 204, 181), Colour(178, 53, 53) },
    { Colour(255, 255, 242), Colour(178, 214, 96), Colour(37, 124, 73) }
};
const std::vector<Colour> enemyDeathParticleColours[4] = {
    { Colour(255, 255, 242), Colour(184, 197, 216), Colour(25, 40, 102) },
    { Colour(255, 255, 242), Colour(255, 204, 181), Colour(165, 82, 139) },
    { Colour(255, 255, 242), Colour(255, 204, 181), Colour(229, 114, 57) },
    { Colour(255, 255, 242), Colour(184, 197, 216), Colour(62, 106, 178) }
};
const std::vector<Colour> levelTriggerParticleColours = { Colour(255, 255, 242), Colour(145, 224, 204), Colour(53, 130, 130) };

const Colour inputSelectColour = Colour(255, 199, 89);
const Colour hudBackground = Colour(7, 0, 14, 64);
const Colour defaultWhite = Colour(255, 255, 242);

class Camera {
public:
    float x, y;

    bool locked;

    Camera() {
        x = y = 0;
        locked = false;
    }

    void ease_out_to(float dt, float targetX, float targetY) {
        if (!locked) {
            x += (targetX - x) * CAMERA_SCALE_X * dt;
            y += (targetY - y) * CAMERA_SCALE_Y * dt;
        }
    }

    void linear_to(float dt, float startX, float startY, float targetX, float targetY, float time) {
        if (!locked) {
            if (std::abs(targetX - x) < std::abs(((targetX - startX) / time) * dt)) {
                x = targetX;
            }
            else {
                x += ((targetX - startX) / time) * dt;
            }

            if (std::abs(targetY - y) < std::abs(((targetY - startY) / time) * dt)) {
                y = targetY;
            }
            else {
                y += ((targetY - startY) / time) * dt;
            }
        }
    }
};
Camera camera;


class Particle {
public:
    float x, y;
    float xVel, yVel;
    float gravity;
    Colour colour;
    float age;

    Particle() {
        x = y = 0;
        xVel = yVel = 0;

        gravity = 0;
        colour = Colour(0, 0, 0);

        age = 0;
    }

    Particle(float xPosition, float yPosition, float xVelocity, float yVelocity, float particleGravity, Colour particleColour) {
        x = xPosition;
        y = yPosition;
        xVel = xVelocity;
        yVel = yVelocity;

        gravity = particleGravity;
        colour = particleColour;

        age = 0;
    }

    void render(Camera camera) {
        screen.pen = Pen(colour.r, colour.g, colour.b, colour.a);
        screen.pixel(Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
    }

    void update(float dt) {
        age += dt;
        colour.a = std::max(0.0f, colour.a - age * 10);

        y += gravity * dt;

        x += xVel * dt;
        y += yVel * dt;
    }
};



std::vector<Particle> generate_particles(float x, float y, float gravity, std::vector<Colour> colours, float speed, uint8_t count) {
    std::vector<Particle> particles;

    for (uint8_t i = 0; i < count; i++) {
        float angle = rand() % 360;

        float xVel = ((rand() % 100) / 100.0f) * std::cos(angle) * speed;
        float yVel = ((rand() % 100) / 100.0f) * std::sin(angle) * speed;

        particles.push_back(Particle(x, y, xVel, yVel, gravity, colours[rand() % colours.size()]));
    }

    return particles;
}



class Projectile {
public:
    float x, y;
    float xVel, yVel;
    uint8_t id;

    Projectile() {
        x = y = 0;
        xVel = yVel = 0;

        id = 0;
    }

    Projectile(float xPosition, float yPosition, float xVelocity, float yVelocity, uint8_t tileId) {
        x = xPosition;
        y = yPosition;
        xVel = xVelocity;
        yVel = yVelocity;
        id = tileId;
    }

    void update(float dt, ButtonStates buttonStates) {
        // Update gravity
        yVel += PROJECTILE_GRAVITY * dt;
        yVel = std::min(yVel, (float)PROJECTILE_GRAVITY_MAX);

        // Move entity y
        y += yVel * dt;

        // Move entity x
        x += xVel * dt;
    }

    void render(Camera camera) {
        render_sprite(id, Point(SCREEN_MID_WIDTH + x - camera.x - SPRITE_QUARTER, SCREEN_MID_HEIGHT + y - camera.y - SPRITE_QUARTER));
        //screen.sprite(id, Point(SCREEN_MID_WIDTH + x - camera.x - SPRITE_QUARTER, SCREEN_MID_HEIGHT + y - camera.y - SPRITE_QUARTER));
        //screen.rectangle(Rect(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y, 4, 4));
    }
};
std::vector<Projectile> projectiles;



class LevelObject {
public:
    uint16_t x, y;

    LevelObject() {
        x = y = 0;
    }

    LevelObject(uint16_t xPosition, uint16_t yPosition) {
        x = xPosition;
        y = yPosition;
    }

    virtual void update(float dt, ButtonStates buttonStates) = 0;

    virtual void render(Camera camera) = 0; 

protected:
};

class Tile : public LevelObject {
public:
    Tile() : LevelObject() {
        id = TILE_ID_EMPTY;
    }

    Tile(uint16_t xPosition, uint16_t yPosition, uint8_t tileID) : LevelObject(xPosition, yPosition) {
        id = tileID;
    }

    void update(float dt, ButtonStates buttonStates) {

    }

    void render(Camera camera)
    {
        //screen.sprite(id, Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
        render_sprite(id, Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
    }

protected:
    uint8_t id;
};
std::vector<Tile> foreground;
std::vector<Tile> background;
//std::vector<Tile> platforms;

class ParallaxTile : public Tile {
public:
    ParallaxTile() : Tile() {
        id = TILE_ID_EMPTY;
        layer = 0;
    }

    ParallaxTile(uint16_t xPosition, uint16_t yPosition, uint8_t tileID, uint8_t parallaxLayer) : Tile(xPosition, yPosition, tileID) {
        id = tileID;
        layer = parallaxLayer;
    }

    void update(float dt, ButtonStates buttonStates) {
        
    }

    void render(Camera camera)
    {
        //screen.sprite(id, Point(SCREEN_MID_WIDTH + x - (camera.x * parallaxFactorLayersX[layer]), SCREEN_MID_HEIGHT + y - (camera.y * parallaxFactorLayersY[layer])));
        // Not shifting sprite to center seems to give better coverage of parallax
        //screen.sprite(id, Point(x - (camera.x * parallaxFactorLayersX[layer]), y - (camera.y * parallaxFactorLayersY[layer])));
        render_sprite(id, Point(x - (camera.x * parallaxFactorLayersX[layer]), y - (camera.y * parallaxFactorLayersY[layer])));
    }

protected:
    uint8_t id;
    uint8_t layer;
};
std::vector<ParallaxTile> parallax;


class Pickup : public LevelObject {
public:
    bool collected;

    Pickup() : LevelObject() {
        collected = false;
    }

    Pickup(uint16_t xPosition, uint16_t yPosition) : LevelObject(xPosition, yPosition) {
        collected = false;
    }

    virtual void update(float dt, ButtonStates buttonStates) = 0;

    virtual void render(Camera camera) = 0;

protected:

};

class AnimatedPickup : public Pickup {
public:
    AnimatedPickup() : Pickup() {
        animationTimer = 0;
        currentFrame = 0;
    }

    AnimatedPickup(uint16_t xPosition, uint16_t yPosition, std::vector<uint8_t> animationFrames) : Pickup(xPosition, yPosition) {
        animationTimer = 0;
        currentFrame = 0;

        frames = animationFrames;
    }

    void update(float dt, ButtonStates buttonStates) {
        animationTimer += dt;

        if (animationTimer >= FRAME_LENGTH) {
            animationTimer -= FRAME_LENGTH;
            currentFrame++;
            currentFrame %= frames.size();
        }
    }

    void render(Camera camera) {
        if (!collected) {
            //screen.sprite(frames[currentFrame], Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
            render_sprite(frames[currentFrame], Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
        }
    }

protected:
    float animationTimer;
    std::vector<uint8_t> frames;
    uint8_t currentFrame;
};

class Coin : public AnimatedPickup {
public:
    Coin() : AnimatedPickup() {

    }

    Coin(uint16_t xPosition, uint16_t yPosition, std::vector<uint8_t> animationFrames) : AnimatedPickup(xPosition, yPosition, animationFrames) {

    }

    void update(float dt, ButtonStates buttonStates) {
        AnimatedPickup::update(dt, buttonStates);
    }

    void render(Camera camera) {
        AnimatedPickup::render(camera);
    }

protected:

};
std::vector<Coin> coins;

class Finish : public AnimatedPickup {
public:
    Finish() : AnimatedPickup() {

    }

    Finish(uint16_t xPosition, uint16_t yPosition, std::vector<uint8_t> animationFrames) : AnimatedPickup(xPosition, yPosition, animationFrames) {

    }

    void update(float dt, ButtonStates buttonStates) {
        AnimatedPickup::update(dt, buttonStates);
    }

    void render(Camera camera) {
        AnimatedPickup::render(camera);
    }
};
Finish finish;

class AnimatedTransition {
public:
    uint8_t x, y;
    float closedTimer;

    AnimatedTransition() {
        animationTimer = 0;
        currentFrame = 0;

        state = TransitionState::OPEN;
        x = y = 0;
        closedTimer = 0;
    }

    AnimatedTransition(uint16_t xPosition, uint16_t yPosition, std::vector<uint8_t> open, std::vector<uint8_t> close) {
        animationTimer = 0;
        currentFrame = 0;

        openingFrames = open;
        closingFrames = close;
        state = TransitionState::OPEN;
        x = xPosition;
        y = yPosition;

        closedTimer = 0;
    }

    void update(float dt, ButtonStates buttonStates) {
        if (state == TransitionState::CLOSING || state == TransitionState::OPENING) {
            animationTimer += dt;

            if (animationTimer >= TRANSITION_FRAME_LENGTH) {
                animationTimer -= TRANSITION_FRAME_LENGTH;
                currentFrame++;

                if (state == TransitionState::CLOSING) {
                    if (currentFrame == closingFrames.size()) {
                        state = TransitionState::CLOSED;
                        closedTimer = 0;
                    }
                }
                else {
                    if (currentFrame == openingFrames.size()) {
                        state = TransitionState::OPEN;
                    }
                }
            }
        }
        else if (state == TransitionState::CLOSED) {
            closedTimer += dt;
            if (closedTimer >= TRANSITION_CLOSE_LENGTH) {
                state = TransitionState::READY_TO_OPEN;
            }
        }
    }

    void render(Camera camera) {
        if (state == TransitionState::CLOSING) {
            //screen.sprite(closingFrames[currentFrame], Point(x, y));
            render_sprite(closingFrames[currentFrame], Point(x, y));
        }
        else if (state == TransitionState::OPENING) {
            //screen.sprite(openingFrames[currentFrame], Point(x, y), SpriteTransform::HORIZONTAL);
            render_sprite(openingFrames[currentFrame], Point(x, y), SpriteTransform::HORIZONTAL);
        }
        else if (state == TransitionState::CLOSED || state == TransitionState::READY_TO_OPEN) {
            //screen.sprite(closingFrames[closingFrames.size() - 1], Point(x, y));
            render_sprite(closingFrames[closingFrames.size() - 1], Point(x, y));
        }
        else if (state == TransitionState::OPEN) {
            // Don't do anything
        }
    }

    void close() {
        state = TransitionState::CLOSING;
        animationTimer = 0;
        currentFrame = 0;
    }

    void open() {
        state = TransitionState::OPENING;
        animationTimer = 0;
        currentFrame = 0;
    }

    bool is_closed() {
        return state == TransitionState::CLOSED;
    }

    bool is_ready_to_open() {
        return state == TransitionState::READY_TO_OPEN;
    }

    bool is_open() {
        return state == TransitionState::OPEN;
    }

protected:
    enum class TransitionState {
        OPENING,
        OPEN,
        CLOSING,
        CLOSED,
        READY_TO_OPEN
    } state;

    float animationTimer;
    std::vector<uint8_t> openingFrames, closingFrames;
    uint8_t currentFrame;
};
AnimatedTransition transition[SCREEN_TILE_SIZE];






class LevelTrigger {
public:
    uint16_t x, y;
    std::vector<Particle> particles;
    uint8_t levelNumber;
    bool visible;
    bool generateParticles;

    LevelTrigger(){
        x = y = 0;
        levelNumber = 0;

        visible = true;
        generateParticles = false;

        textY = textVelY = 0;
    }

    LevelTrigger(uint16_t xPosition, uint16_t yPosition, uint8_t levelTriggerNumber) {
        x = xPosition;
        y = yPosition;

        levelNumber = levelTriggerNumber;

        visible = true;
        generateParticles = false;

        textY = textVelY = 0;
    }

    void update(float dt, ButtonStates buttonStates) {
        textVelY += TEXT_GRAVITY * dt;
        if (textY > SPRITE_SIZE) {
            textY = SPRITE_SIZE;
            textVelY = -TEXT_JUMP_VELOCITY;
        }

        textY += textVelY * dt;

        if (!visible) {

            if (generateParticles) {
                if (particles.size() == 0) {
                    generateParticles = false;
                }
                else {
                    for (uint8_t i = 0; i < particles.size(); i++) {
                        particles[i].update(dt);
                    }

                    // Remove any particles which are too old
                    particles.erase(std::remove_if(particles.begin(), particles.end(), [](Particle particle) { return (particle.age >= ENTITY_DEATH_PARTICLE_AGE); }), particles.end());
                }
            }
            else {
                // Generate particles
                // TODO: change constants?
                particles = generate_particles(x, y, ENTITY_DEATH_PARTICLE_GRAVITY, levelTriggerParticleColours, ENTITY_DEATH_PARTICLE_SPEED, ENTITY_DEATH_PARTICLE_COUNT);
                generateParticles = true;
            }
        }
    }

    void render(Camera camera) {
        if (visible) {
            screen.pen = Pen(levelTriggerParticleColours[1].r, levelTriggerParticleColours[1].g, levelTriggerParticleColours[1].b, levelTriggerParticleColours[1].a);
            screen.text(std::to_string(levelNumber + 1), minimal_font, Point(SCREEN_MID_WIDTH + x - camera.x + SPRITE_HALF, SCREEN_MID_HEIGHT + y - camera.y - SPRITE_HALF * 3 + textY), true, TextAlign::center_center);
            //screen.sprite(TILE_ID_LEVEL_TRIGGER, Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
            render_sprite(TILE_ID_LEVEL_TRIGGER, Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
        }

        // Particles
        for (uint8_t i = 0; i < particles.size(); i++) {
            particles[i].render(camera);
        }
    }

    void set_active() {
        visible = false;
    }

protected:
    float textY;
    float textVelY;
};
std::vector<LevelTrigger> levelTriggers;





class Entity {
public:
    float x, y;
    uint8_t health;
    bool locked;
    std::vector<Particle> particles;
    uint8_t lastDirection;

    Entity() {
        x = y = 0;
        xVel = yVel = 0;

        anchorFrame = 0;

        lastDirection = 1; // 1 = right, 0 = left
        //state = IDLE;
        locked = false;
        deathParticles = false;

        health = 1;

        immuneTimer = 0;
    }

    Entity(uint16_t xPosition, uint16_t yPosition, uint8_t frame, uint8_t startHealth) {
        x = xPosition;
        y = yPosition;
        xVel = yVel = 0;

        anchorFrame = frame;

        lastDirection = 1;
        //state = IDLE;
        locked = false;
        deathParticles = false;

        health = startHealth;

        immuneTimer = 0;
    }

    void update(float dt, ButtonStates buttonStates) {

    }

    void update_collisions() {
        if (!locked) {
            // Update gravity
            yVel += GRAVITY * dt;
            yVel = std::min(yVel, (float)GRAVITY_MAX);

            // Move entity y
            y += yVel * dt;

            // Here check collisions...
            for (uint16_t i = 0; i < foreground.size(); i++) {
                if (colliding(foreground[i])) {
                    if (yVel > 0) {
                        // Collided from top
                        y = foreground[i].y - SPRITE_SIZE;
                    }
                    else {
                        // Collided from bottom
                        y = foreground[i].y + SPRITE_SIZE;
                    }
                    yVel = 0;
                }
            }

            // Move entity x
            x += xVel * dt;

            // Here check collisions...
            for (uint16_t i = 0; i < foreground.size(); i++) {
                if (colliding(foreground[i])) {
                    if (xVel > 0) {
                        // Collided from left
                        x = foreground[i].x - SPRITE_SIZE + 1;
                    }
                    else {
                        // Collided from right
                        x = foreground[i].x + SPRITE_SIZE - 1;
                    }
                    xVel = 0;
                }
            }

            if (xVel > 0) {
                lastDirection = 1;
            }
            else if (xVel < 0) {
                lastDirection = 0;
            }
        }
    }
    
    void render(Camera camera) {
        if (health != 0) {
            bool visible = false;

            if (immuneTimer) {
                uint16_t immuneTimer_ms = (uint16_t)(immuneTimer * 1000);
                if (immuneTimer_ms % 150 < 75) {
                    visible = true;
                }
            }
            else {
                visible = true;
            }

            if (visible) {
                uint8_t frame = anchorFrame;

                if (yVel < -50) {
                    frame = anchorFrame + 1;
                }
                else if (yVel > 160) {
                    frame = anchorFrame + 2;
                }

                /*if (immuneTimer) {
                    frame = anchorFrame + 3;
                }*/

                if (lastDirection == 1) {
                    //screen.sprite(frame, Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y), SpriteTransform::HORIZONTAL);
                    render_sprite(frame, Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y), SpriteTransform::HORIZONTAL);
                }
                else {
                    //screen.sprite(frame, Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
                    render_sprite(frame, Point(SCREEN_MID_WIDTH + x - camera.x, SCREEN_MID_HEIGHT + y - camera.y));
                }
            }
        }

        // Particles
        for (uint8_t i = 0; i < particles.size(); i++) {
            particles[i].render(camera);
        }
    }

    bool colliding(Tile tile) {
        // Replace use of this with actual code?
        return (tile.x + SPRITE_SIZE > x + 1 && tile.x < x + SPRITE_SIZE - 1 && tile.y + SPRITE_SIZE > y && tile.y < y + SPRITE_SIZE);
    }

    void set_immune() {
        immuneTimer = PLAYER_IMMUNE_TIME;
    }

    bool is_immune() {
        return immuneTimer;
    }

protected:
    float xVel, yVel;
    uint8_t anchorFrame;
    bool deathParticles;
    float immuneTimer;
};



class Enemy : public Entity {
public:
    float* playerX;
    float* playerY;

    Enemy() : Entity() {
        enemyType = EnemyType::BASIC;
        reloadTimer = 0;

        playerX = nullptr;
        playerY = nullptr;

        currentSpeed = ENTITY_IDLE_SPEED;
    }

    Enemy(uint16_t xPosition, uint16_t yPosition, uint8_t startHealth, uint8_t type) : Entity(xPosition, yPosition, TILE_ID_ENEMY_1 + type * 4, startHealth) {
        enemyType = (EnemyType)type;
        reloadTimer = 0;

        playerX = nullptr;
        playerY = nullptr;

        currentSpeed = ENTITY_IDLE_SPEED;
    }

    void update(float dt, ButtonStates buttonStates) {

        if (health > 0) {
            if (reloadTimer) {
                reloadTimer -= dt;
                if (reloadTimer < 0) {
                    reloadTimer = 0;
                }
            }

            if (enemyType == EnemyType::BASIC || enemyType == EnemyType::ARMOURED) {
                // Consider adding acceleration?
                if (lastDirection) {
                    xVel = currentSpeed;
                }
                else {
                    xVel = -currentSpeed;
                }

                Entity::update_collisions();


                bool reverseDirection = true;

                float tempX = lastDirection ? x + SPRITE_HALF : x - SPRITE_HALF;
                for (uint16_t i = 0; i < foreground.size(); i++) {
                    if (y + SPRITE_SIZE == foreground[i].y && foreground[i].x + SPRITE_SIZE > tempX + 1 && foreground[i].x < tempX + SPRITE_SIZE - 1) {
                        // About to be on block
                        reverseDirection = false;
                    }
                    if (foreground[i].y + SPRITE_SIZE > y && foreground[i].y < y + SPRITE_SIZE && (lastDirection ? x + SPRITE_SIZE - 1 : x - SPRITE_SIZE + 1) == foreground[i].x) {
                        // Walked into side of block
                        reverseDirection = true;
                        // Break because we definitely need to change direction, and don't want any other blocks resetting this to false
                        break;
                    }
                }

                if (reverseDirection) {
                    lastDirection = 1 - lastDirection;
                }

                if (health == 1) {
                    // EnemyType::ARMOURED has helmet on, and 2 hp
                    enemyType = EnemyType::BASIC;
                    anchorFrame = TILE_ID_ENEMY_1 + (int)enemyType * 4;
                }
            }
            else if (enemyType == EnemyType::RANGED) {
                Entity::update_collisions();

                lastDirection = *playerX < x ? 0 : 1;

                if (std::abs(x - *playerX) < RANGED_MAX_RANGE && std::abs(y - *playerY) < RANGED_MAX_RANGE && !reloadTimer) {
                    // fire!
                    // Maybe make these values constants?
                    projectiles.push_back(Projectile(x, y, RANGED_PROJECTILE_X_VEL_SCALE * (*playerX - x), -std::abs(x - *playerX) * RANGED_PROJECTILE_Y_VEL_SCALE + (*playerY - y) * RANGED_PROJECTILE_Y_VEL_SCALE, TILE_ID_ENEMY_PROJECTILE));
                    reloadTimer = RANGED_RELOAD_TIME;
                }
            }
            else if (enemyType == EnemyType::PURSUIT) {
                // Consider adding acceleration?
                if (lastDirection) {
                    xVel = currentSpeed;
                }
                else {
                    xVel = -currentSpeed;
                }
                // TODO use faster speed if pursuing
                Entity::update_collisions();


                if (std::abs(x - *playerX) < PURSUIT_MAX_RANGE && std::abs(y - *playerY) < PURSUIT_MAX_RANGE) {
                    // Pursue!
                    currentSpeed = ENTITY_PURSUIT_SPEED;

                    lastDirection = *playerX < x ? 0 : 1;

                    bool jump = true;

                    float tempX = lastDirection ? x + SPRITE_HALF : x - SPRITE_HALF;
                    for (uint16_t i = 0; i < foreground.size(); i++) {
                        if (y + SPRITE_SIZE == foreground[i].y && foreground[i].x + SPRITE_SIZE > tempX + 1 && foreground[i].x < tempX + SPRITE_SIZE - 1) {
                            // About to be on block
                            jump = false;
                        }
                        //if ((lastDirection ? x + SPRITE_SIZE - 1 : x - SPRITE_SIZE + 1) == foreground[i].x) {
                        //    // Walked into side of block
                        //    jump = true;
                        //    // Break because we definitely need to jump
                        //    break;
                        //}
                    }

                    if (jump) {
                        for (uint16_t i = 0; i < foreground.size(); i++) {
                            if (y + SPRITE_SIZE == foreground[i].y && foreground[i].x + SPRITE_SIZE > x && foreground[i].x < x + SPRITE_SIZE) {
                                // On top of block
                                // Jump
                                yVel = -ENTITY_JUMP_SPEED;
                                //state = JUMP;
                            }
                        }
                    }
                }
                else {
                    // Just patrol... (Same as basic enemy)
                    currentSpeed = ENTITY_IDLE_SPEED;

                    bool reverseDirection = true;

                    float tempX = lastDirection ? x + SPRITE_HALF : x - SPRITE_HALF;
                    for (uint16_t i = 0; i < foreground.size(); i++) {
                        if (y + SPRITE_SIZE == foreground[i].y && foreground[i].x + SPRITE_SIZE > tempX + 1 && foreground[i].x < tempX + SPRITE_SIZE - 1) {
                            // About to be on block
                            reverseDirection = false;
                        }
                        if (foreground[i].y + SPRITE_SIZE > y && foreground[i].y < y + SPRITE_SIZE && (lastDirection ? x + SPRITE_SIZE - 1 : x - SPRITE_SIZE + 1) == foreground[i].x) {
                            // Walked into side of block
                            reverseDirection = true;
                            // Break because we definitely need to change direction, and don't want any other blocks resetting this to false
                            break;
                        }
                    }

                    if (reverseDirection) {
                        lastDirection = 1 - lastDirection;
                    }
                }
            }
            else if (enemyType == EnemyType::FLYING) {
            // Should it be EnemyType::BOUNCING instead?
                // Consider adding acceleration?
                if (lastDirection) {
                    xVel = currentSpeed;
                }
                else {
                    xVel = -currentSpeed;
                }

                Entity::update_collisions();


                bool reverseDirection = false;

                float tempX = lastDirection ? x + SPRITE_HALF : x - SPRITE_HALF;
                for (uint16_t i = 0; i < foreground.size(); i++) {
                    if (foreground[i].y + SPRITE_SIZE > y && foreground[i].y < y + SPRITE_SIZE && (lastDirection ? x + SPRITE_SIZE - 1 : x - SPRITE_SIZE + 1) == foreground[i].x) {
                        // Walked into side of block
                        reverseDirection = true;
                        // Break because we definitely need to change direction, and don't want any other blocks resetting this to false
                        break;
                    }
                    if (y + SPRITE_SIZE == foreground[i].y && foreground[i].x + SPRITE_SIZE > x && foreground[i].x < x + SPRITE_SIZE) {
                        yVel = -ENTITY_JUMP_SPEED;
                    }
                }

                if (reverseDirection) {
                    lastDirection = 1 - lastDirection;
                }
            }/*
            else if (enemyType == EnemyType::ARMOURED) {

            }*/


            if (y > levelDeathBoundary) {
                health = 0;
                xVel = yVel = 0;
            }
        }

        if (health == 0) {
            //state = DEAD;

            if (deathParticles) {
                if (particles.size() == 0) {
                    // No particles left

                    health = 3; //remove?

                    // Reset player position
                    yVel = 0;
                    lastDirection = 1;
                    x = playerStartX;
                    y = playerStartY;

                    deathParticles = false;
                }
                else {
                    for (uint8_t i = 0; i < particles.size(); i++) {
                        particles[i].update(dt);
                    }

                    // Remove any particles which are too old
                    particles.erase(std::remove_if(particles.begin(), particles.end(), [](Particle particle) { return (particle.age >= ENTITY_DEATH_PARTICLE_AGE); }), particles.end());
                }
            }
            else {
                // Generate particles
                particles = generate_particles(x, y, ENTITY_DEATH_PARTICLE_GRAVITY, enemyDeathParticleColours[(uint8_t)enemyType], ENTITY_DEATH_PARTICLE_SPEED, ENTITY_DEATH_PARTICLE_COUNT);
                deathParticles = true;
            }
        }
    }

    void render(Camera camera) {
        Entity::render(camera);
    }

    void set_player_position(float* x, float* y) {
        playerX = x;
        playerY = y;
    }

protected:
    enum class EnemyType {
        BASIC, // type 1
        RANGED, // type 2
        PURSUIT, // type 3
        FLYING, // type 4
        ARMOURED // type 5
    } enemyType;

    float reloadTimer;

    float currentSpeed;

    //enum EntityState {
    //    IDLE,
    //    WALK,
    //    //RUN,
    //    JUMP,
    //    //CROUCH,
    //    //INJURED,
    //    DEAD
    //} state;
};
std::vector<Enemy> enemies;


class Player : public Entity {
public:
    uint8_t score;
    uint8_t enemiesKilled;
    float levelTimer;
    uint8_t id;
    uint8_t lives;

    Player() : Entity() {
        score = 0;
        enemiesKilled = 0;
        levelTimer = 0;

        id = 0;

        lives = PLAYER_START_LIVES;
    }

    Player(uint16_t xPosition, uint16_t yPosition, uint8_t colour) : Entity(xPosition, yPosition, TILE_ID_PLAYER_1 + colour * 4, PLAYER_MAX_HEALTH) {
        score = 0;
        enemiesKilled = 0;
        levelTimer = 0;

        id = colour;

        lives = PLAYER_START_LIVES;
    }

    void update(float dt, ButtonStates buttonStates) {
        if (immuneTimer) {
            immuneTimer -= dt;
            if (immuneTimer < 0) {
                immuneTimer = 0;
            }
        }

        if (health > 0) {

            if (!locked) {
                levelTimer += dt;

                if (buttonStates.A) {
                    for (uint16_t i = 0; i < foreground.size(); i++) {
                        if (y + SPRITE_SIZE == foreground[i].y && foreground[i].x + SPRITE_SIZE > x && foreground[i].x < x + SPRITE_SIZE) {
                            // On top of block
                            // Jump
                            yVel = -PLAYER_MAX_JUMP;
                            //state = JUMP;
                        }
                    }

                    // Allow player to jump on locked levelTriggers
                    for (uint16_t i = 0; i < levelTriggers.size(); i++) {
                        if (y + SPRITE_SIZE == levelTriggers[i].y && levelTriggers[i].x + SPRITE_SIZE > x && levelTriggers[i].x < x + SPRITE_SIZE) {
                            // On top of block
                            if (allPlayerSaveData[playerSelected].levelReached < levelTriggers[i].levelNumber) {
                                // LevelTrigger is locked
                                // Jump
                                yVel = -PLAYER_MAX_JUMP;
                                //state = JUMP;
                            }
                        }
                    }
                }


                if (buttonStates.LEFT) {
                    xVel -= PLAYER_ACCELERATION * dt;
                    if (xVel < -PLAYER_MAX_SPEED) {
                        xVel = -PLAYER_MAX_SPEED;
                    }
                }
                else if (buttonStates.RIGHT) {
                    xVel += PLAYER_ACCELERATION * dt;
                    if (xVel > PLAYER_MAX_SPEED) {
                        xVel = PLAYER_MAX_SPEED;
                    }
                }
                else {
                    if (xVel > 0) {
                        xVel -= PLAYER_ACCELERATION * dt;
                        if (xVel < 0) {
                            xVel = 0;
                        }
                    }
                    else if (xVel < 0) {
                        xVel += PLAYER_ACCELERATION * dt;
                        if (xVel > 0) {
                            xVel = 0;
                        }
                    }
                }
            }

            

            uint8_t coinCount = coins.size();

            // Remove coins if player jumps on them
            coins.erase(std::remove_if(coins.begin(), coins.end(), [this](Coin coin) { return (coin.x + SPRITE_SIZE > x && coin.x < x + SPRITE_SIZE && coin.y + SPRITE_SIZE > y && coin.y < y + SPRITE_SIZE); }), coins.end());

            // Add points to player score (1 point per coin which has been deleted)
            score += coinCount - coins.size();


            uint8_t enemyCount = enemies.size();

            // Remove enemies if no health left
            enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy enemy) { return (enemy.health == 0 && enemy.particles.size() == 0); }), enemies.end());

            enemiesKilled += enemyCount - enemies.size();

            // Now done in update_collisions
            //for (uint8_t i = 0; i < enemies.size(); i++) {
            //    if (enemies[i].x + SPRITE_SIZE > x && enemies[i].x < x + SPRITE_SIZE && enemies[i].y == y + SPRITE_SIZE && enemies[0].health) {
            //        enemies[i].health = 0; // or --?
            //    }
            //}

            update_collisions();


            if (y > levelDeathBoundary) {
                health = 0;
                xVel = yVel = 0;
            }
        }


        if (health == 0) {
            //state = DEAD;

            if (deathParticles) {
                if (particles.size() == 0) {
                    // No particles left, reset values which need to be

                    deathParticles = false;

                    // If player has lives left, respawn
                    if (lives) {
                        // Reset player position and health, maybe remove all this?
                        health = 3;
                        yVel = 0;// -PLAYER_MAX_JUMP;
                        lastDirection = 1;
                        x = playerStartX;
                        y = playerStartY;

                        // Stop player from moving while respawning
                        cameraRespawn = true;
                        locked = true;

                        // Make player immune when respawning?
                        //set_immune();

                        // Remove immunity when respawning
                        immuneTimer = 0;
                    }
                }
                else {
                    for (uint8_t i = 0; i < particles.size(); i++) {
                        particles[i].update(dt);
                    }

                    // Remove any particles which are too old
                    particles.erase(std::remove_if(particles.begin(), particles.end(), [](Particle particle) { return (particle.age >= ENTITY_DEATH_PARTICLE_AGE); }), particles.end());
                }
            }
            else if (lives) {
                // Generate particles
                particles = generate_particles(x, y, ENTITY_DEATH_PARTICLE_GRAVITY, playerDeathParticleColours[id], ENTITY_DEATH_PARTICLE_SPEED, ENTITY_DEATH_PARTICLE_COUNT);
                deathParticles = true;

                // Reduce player lives by one
                lives--;
            }
        }
    }

    void update_collisions() {
        if (!locked) {
            // Update gravity
            yVel += GRAVITY * dt;
            yVel = std::min(yVel, GRAVITY_MAX);

            // Move entity y
            y += yVel * dt;

            // Here check collisions...

            for (uint16_t i = 0; i < foreground.size(); i++) {
                if (colliding(foreground[i])) {
                    if (yVel > 0) {
                        // Collided from top
                        y = foreground[i].y - SPRITE_SIZE;
                    }
                    else {
                        // Collided from bottom
                        y = foreground[i].y + SPRITE_SIZE;
                    }
                    yVel = 0;
                }
            }

            for (uint16_t i = 0; i < levelTriggers.size(); i++) {
                if (levelTriggers[i].visible && colliding(levelTriggers[i])) {
                    if (yVel > 0 && y < levelTriggers[i].y + SPRITE_HALF) {
                        if (allPlayerSaveData[playerSelected].levelReached >= levelTriggers[i].levelNumber) {
                            // Level is unlocked

                            // Collided from top
                            y = levelTriggers[i].y - SPRITE_SIZE;
                            //yVel = -PLAYER_ATTACK_JUMP;
                            yVel = -std::max(yVel * PLAYER_ATTACK_JUMP_SCALE, PLAYER_ATTACK_JUMP_MIN);

                            levelTriggers[i].set_active();
                        }
                        else {
                            // Level is locked, act as a solid object

                            // Collided from top
                            y = levelTriggers[i].y - SPRITE_SIZE;
                            yVel = 0;
                        }
                    }
                }
            }

            for (uint16_t i = 0; i < enemies.size(); i++) {
                if (enemies[i].health && colliding(enemies[i])) {
                    if (yVel > 0 && y < enemies[i].y + SPRITE_HALF) {
                        // Collided from top
                        y = enemies[i].y - SPRITE_SIZE;
                        //yVel = -PLAYER_ATTACK_JUMP;
                        yVel = -std::max(yVel * PLAYER_ATTACK_JUMP_SCALE, PLAYER_ATTACK_JUMP_MIN);

                        // Take health off enemy
                        enemies[i].health--;
                    }
                    //else {
                    //    // Collided from bottom
                    //    y = enemies[i].y + SPRITE_SIZE;
                    //}
                }
            }

            // Move entity x
            x += xVel * dt;

            // Here check collisions...
            for (uint16_t i = 0; i < foreground.size(); i++) {
                if (colliding(foreground[i])) {
                    if (xVel > 0) {
                        // Collided from left
                        x = foreground[i].x - SPRITE_SIZE + 1;
                    }
                    else {
                        // Collided from right
                        x = foreground[i].x + SPRITE_SIZE - 1;
                    }
                    xVel = 0;
                }
            }

            for (uint16_t i = 0; i < levelTriggers.size(); i++) {
                if (levelTriggers[i].visible && colliding(levelTriggers[i])) {
                    if (xVel > 0) {
                        // Collided from left
                        x = levelTriggers[i].x - SPRITE_SIZE;
                    }
                    else {
                        // Collided from right
                        x = levelTriggers[i].x + SPRITE_SIZE;
                    }
                    xVel = 0;
                }
            }

            //for (uint16_t i = 0; i < enemies.size(); i++) {
            //    if (colliding(enemies[i]) && enemies[i].health) {
            //        if (xVel > 0) {
            //            // Collided from left
            //            x = enemies[i].x - SPRITE_SIZE;
            //        }
            //        else {
            //            // Collided from right
            //            x = enemies[i].x + SPRITE_SIZE;
            //        }
            //        xVel = 0;
            //    }
            //}

            if (!immuneTimer) {
                for (uint16_t i = 0; i < enemies.size(); i++) {
                    if (colliding(enemies[i]) && enemies[i].health) {
                        health--;
                        set_immune();
                    }
                }
            }

            if (xVel > 0) {
                lastDirection = 1;
            }
            else if (xVel < 0) {
                lastDirection = 0;
            }
        }
    }

    void render(Camera camera) {
        Entity::render(camera);
    }

    bool colliding(Tile tile) {
        // Replace use of this with actual code?
        return (tile.x + SPRITE_SIZE > x + 1 && tile.x < x + SPRITE_SIZE - 1 && tile.y + SPRITE_SIZE > y && tile.y < y + SPRITE_SIZE);
    }

    bool colliding(Enemy enemy) {
        // Replace use of this with actual code?
        return (enemy.x + SPRITE_SIZE > x && enemy.x < x + SPRITE_SIZE && enemy.y + SPRITE_SIZE > y && enemy.y < y + SPRITE_SIZE);
    }

    bool colliding(LevelTrigger levelTrigger) {
        // Replace use of this with actual code?
        return (levelTrigger.x + SPRITE_SIZE > x && levelTrigger.x < x + SPRITE_SIZE && levelTrigger.y + SPRITE_SIZE > y && levelTrigger.y < y + SPRITE_SIZE);
    }

protected:
    //enum EntityState {
    //    IDLE,
    //    WALK,
    //    //RUN,
    //    JUMP,
    //    //CROUCH,
    //    //INJURED,
    //    DEAD
    //} state;
};
Player player;


void background_rect(uint8_t position) {
    if (position) {
        screen.pen = Pen(hudBackground.r, hudBackground.g, hudBackground.b, hudBackground.a);
        screen.rectangle(Rect(0, SCREEN_HEIGHT - (SPRITE_SIZE + 12), SCREEN_WIDTH, SPRITE_SIZE + 12));
    }
    else {
        screen.pen = Pen(hudBackground.r, hudBackground.g, hudBackground.b, hudBackground.a);
        screen.rectangle(Rect(0, 0, SCREEN_WIDTH, SPRITE_SIZE + 12));
    }
}

void display_stats() {
    screen.text("Coins collected:", minimal_font, Point(SPRITE_SIZE, SCREEN_MID_HEIGHT - SPRITE_SIZE * 2), true, TextAlign::center_left);
    screen.text("Enemies defeated:", minimal_font, Point(SPRITE_SIZE, SCREEN_MID_HEIGHT), true, TextAlign::center_left);
    screen.text("Time taken:", minimal_font, Point(SPRITE_SIZE, SCREEN_MID_HEIGHT + SPRITE_SIZE * 2), true, TextAlign::center_left);


    screen.text(std::to_string(player.score), minimal_font, Point(SCREEN_WIDTH - SPRITE_SIZE * 2, SCREEN_MID_HEIGHT - SPRITE_SIZE * 2), true, TextAlign::center_right);
    screen.text(std::to_string(player.enemiesKilled), minimal_font, Point(SCREEN_WIDTH - SPRITE_SIZE * 2, SCREEN_MID_HEIGHT), true, TextAlign::center_right);
    //screen.text(std::to_string((int)player.levelTimer), minimal_font, Point(SCREEN_WIDTH - SPRITE_SIZE * 2, SCREEN_MID_HEIGHT + SPRITE_SIZE * 2), true, TextAlign::center_right);

    // Trim player.levelTimer to 2dp
    std::string levelTimerString = std::to_string(player.levelTimer);
    levelTimerString = levelTimerString.substr(0, levelTimerString.find('.') + 3);
    screen.text(levelTimerString, minimal_font, Point(SCREEN_WIDTH - SPRITE_SIZE * 2, SCREEN_MID_HEIGHT + SPRITE_SIZE * 2), true, TextAlign::center_right);


    render_sprite(TILE_ID_HUD_COINS, Point(SCREEN_WIDTH - SPRITE_HALF * 3, SCREEN_MID_HEIGHT - SPRITE_HALF * 5));
    render_sprite(TILE_ID_HUD_ENEMIES_KILLED, Point(SCREEN_WIDTH - SPRITE_HALF * 3, SCREEN_MID_HEIGHT - SPRITE_HALF));
    render_sprite(TILE_ID_HUD_TIME_TAKEN, Point(SCREEN_WIDTH - SPRITE_HALF * 3, SCREEN_MID_HEIGHT + SPRITE_HALF * 3));
}





void open_transition() {
    for (uint16_t i = 0; i < SCREEN_TILE_SIZE; i++) {
        transition[i].open();
    }
}

void close_transition() {
    for (uint16_t i = 0; i < SCREEN_TILE_SIZE; i++) {
        transition[i].close();
    }
}

void render_transition() {
    for (uint16_t i = 0; i < SCREEN_TILE_SIZE; i++) {
        transition[i].render(camera);
    }
}

void update_transition(float dt, ButtonStates buttonStates) {
    for (uint16_t i = 0; i < SCREEN_TILE_SIZE; i++) {
        transition[i].update(dt, buttonStates);
    }
}


void render_tiles(std::vector<Tile> tiles) {
    for (uint32_t i = 0; i < tiles.size(); i++) {
        tiles[i].render(camera);
    }
}

void render_parallax(std::vector<ParallaxTile> parallax) {
    for (uint32_t i = 0; i < parallax.size(); i++) {
        parallax[i].render(camera);
    }
}

void render_coins() {
    for (uint8_t i = 0; i < coins.size(); i++) {
        coins[i].render(camera);
    }
}

void render_enemies() {
    for (uint8_t i = 0; i < enemies.size(); i++) {
        enemies[i].render(camera);
    }
}

void render_level_triggers() {
    for (uint8_t i = 0; i < levelTriggers.size(); i++) {
        levelTriggers[i].render(camera);
    }
}

void render_projectiles() {
    for (uint8_t i = 0; i < projectiles.size(); i++) {
        projectiles[i].render(camera);
    }
}

void render_finish() {
    finish.render(camera);
}

void render_background() {
    screen.blit(background_image, Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), Point(0, 0), false);
}

void render_level() {
    render_parallax(parallax);

    render_tiles(background);
    render_tiles(foreground);

    render_coins();
}

void render_entities() {
    render_enemies();

    player.render(camera);

    render_projectiles();
}

void render_hud() {
    screen.pen = Pen(hudBackground.r, hudBackground.g, hudBackground.b, hudBackground.a);
    screen.rectangle(Rect(0, 0, SCREEN_WIDTH, SPRITE_HALF * 2 + 2));


    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);

    // Player health
    for (uint8_t i = 0; i < PLAYER_MAX_HEALTH; i++) {
        if (i < player.health) {
            render_sprite(TILE_ID_HEART, Point(2 + i * SPRITE_SIZE, 2));
            //screen.sprite(TILE_ID_HEART, Point(2 + i * SPRITE_SIZE, 2));
        }
        else {
            render_sprite(TILE_ID_HEART + 1, Point(2 + i * SPRITE_SIZE, 2));
            //screen.sprite(TILE_ID_HEART + 1, Point(2 + i * SPRITE_SIZE, 2));
        }
    }

    // Player score
    blit::screen.text(std::to_string(player.score), minimal_font, Point(SCREEN_WIDTH - SPRITE_SIZE - 2, 2), true, blit::TextAlign::top_right);

    render_sprite(TILE_ID_HUD_COINS, Point(SCREEN_WIDTH - SPRITE_SIZE, 2));
    //screen.sprite(TILE_ID_HUD_COINS, Point(SCREEN_WIDTH - SPRITE_SIZE, 2));


    // Player lives
    blit::screen.text(std::to_string(player.lives), minimal_font, Point(2 + 6 * SPRITE_SIZE - 2, 2), true, blit::TextAlign::top_right);

    render_sprite(TILE_ID_HUD_LIVES + playerSelected, Point(2 + 6 * SPRITE_SIZE, 2));
    //screen.sprite(TILE_ID_HUD_LIVES + playerSelected, Point(2 + 6 * SPRITE_SIZE, 2));
}

void render_nearby_level_info() {
    for (uint8_t i = 0; i < levelTriggers.size(); i++) {
        if (std::abs(player.x - levelTriggers[i].x) < LEVEL_INFO_MAX_RANGE && std::abs(player.y - levelTriggers[i].y) < LEVEL_INFO_MAX_RANGE) {
            background_rect(1);


            screen.pen = Pen(levelTriggerParticleColours[1].r, levelTriggerParticleColours[1].g, levelTriggerParticleColours[1].b);

            // Level number
            screen.text("Level " + std::to_string(levelTriggers[i].levelNumber + 1), minimal_font, Point(SPRITE_HALF, SCREEN_HEIGHT - 9 - SPRITE_HALF), true, TextAlign::center_left);


            screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);

            if (allPlayerSaveData[playerSelected].levelReached < levelTriggers[i].levelNumber) {
                // Level is locked
                screen.text("Level locked", minimal_font, Point(SCREEN_WIDTH - SPRITE_HALF, SCREEN_HEIGHT - 9 + SPRITE_HALF), true, TextAlign::center_right);
            }
            else if (allPlayerSaveData[playerSelected].levelReached == levelTriggers[i].levelNumber) {
                // Level is unlocked and has not been completed
                screen.text("No highscores", minimal_font, Point(SCREEN_WIDTH - SPRITE_HALF, SCREEN_HEIGHT - 9 + SPRITE_HALF), true, TextAlign::center_right);
            }
            else {
                // Level is unlocked and has been completed

                if (allLevelSaveData[playerSelected][levelTriggers[i].levelNumber].time == 0.0f) {
                    // If time == 0.0f, something's probably wrong (like no save slot for that level, but still save slot for saveData worked)

                    screen.text("Error loading highscores", minimal_font, Point(SCREEN_WIDTH - SPRITE_HALF, SCREEN_HEIGHT - 9 + SPRITE_HALF), true, TextAlign::center_right);
                }
                else {
                    screen.text(std::to_string(allLevelSaveData[playerSelected][levelTriggers[i].levelNumber].score), minimal_font, Point(SCREEN_WIDTH - SPRITE_HALF * 25, SCREEN_HEIGHT - 9 + SPRITE_HALF), true, TextAlign::center_right);
                    screen.text(std::to_string(allLevelSaveData[playerSelected][levelTriggers[i].levelNumber].enemiesKilled), minimal_font, Point(SCREEN_WIDTH - SPRITE_HALF * 16, SCREEN_HEIGHT - 9 + SPRITE_HALF), true, TextAlign::center_right);

                    // Trim time to 2dp
                    std::string timeString = std::to_string(allLevelSaveData[playerSelected][levelTriggers[i].levelNumber].time);
                    timeString = timeString.substr(0, timeString.find('.') + 3);
                    screen.text(timeString, minimal_font, Point(SCREEN_WIDTH - SPRITE_HALF * 4, SCREEN_HEIGHT - 9 + SPRITE_HALF), true, TextAlign::center_right);


                    render_sprite(TILE_ID_HUD_COINS, Point(SCREEN_WIDTH - SPRITE_HALF * 24, SCREEN_HEIGHT - 9));
                    render_sprite(TILE_ID_HUD_ENEMIES_KILLED, Point(SCREEN_WIDTH - SPRITE_HALF * 15, SCREEN_HEIGHT - 9));
                    render_sprite(TILE_ID_HUD_TIME_TAKEN, Point(SCREEN_WIDTH - SPRITE_HALF * 3, SCREEN_HEIGHT - 9));
                }
            }

            //screen.text(std::to_string(saveData.scores[levelTriggers[i].levelNumber]), minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 9), true, TextAlign::center_center);
        }
    }
}

void load_level(uint8_t levelNumber) {
    uint8_t levelTriggerCount = 0;

    // Variables for finding start and finish positions
    uint16_t finishX, finishY;

    playerStartX = playerStartY = 0;
    cameraStartX = cameraStartY = 0;
    finishX = finishY = 0;


    // Get a pointer to the map header
    TMX* tmx = (TMX*)asset_levels[levelNumber];

    uint16_t levelWidth = tmx->width;
    uint16_t levelHeight = tmx->height;
    uint32_t levelSize = levelWidth * levelHeight;

    levelData.levelWidth = levelWidth;
    levelData.levelHeight = levelHeight;

    levelDeathBoundary = levelData.levelHeight * SPRITE_SIZE * LEVEL_DEATH_BOUNDARY_SCALE;

    foreground.clear();
    background.clear();
    parallax.clear();
    coins.clear();
    enemies.clear();
    levelTriggers.clear();

    // Foreground Layer
    for (uint32_t i = 0; i < levelSize; i++) {
        if (tmx->data[i] == TILE_ID_EMPTY) {
            // Is a blank tile, don't do anything
        }
        else if (tmx->data[i] == TILE_ID_COIN) {
            coins.push_back(Coin((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, coinFrames));
        }
        else {
            foreground.push_back(Tile((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, tmx->data[i]));
        }
    }


    // Background Layer
    for (uint32_t i = 0; i < levelSize; i++) {
        uint32_t index = i + levelSize * 2;

        if (tmx->data[index] == TILE_ID_EMPTY) {
            // Is a blank tile, don't do anything
        }
        else {
            // Background tiles are non-solid. If semi-solidity (can jump up but not fall through) is required, use platforms (will be a separate layer).
            background.push_back(Tile((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, tmx->data[index]));
        }
    }

    // Entity Spawns Layer
    for (uint32_t i = 0; i < levelSize; i++) {
        uint32_t index = i + levelSize;

        if (tmx->data[index] == TILE_ID_EMPTY) {
            // Is a blank tile, don't do anything
        }
        else if (tmx->data[index] == TILE_ID_PLAYER_1) {
            playerStartX = (i % levelWidth) * SPRITE_SIZE;
            playerStartY = (i / levelWidth) * SPRITE_SIZE;
        }
        else if (tmx->data[index] == TILE_ID_CAMERA) {
            cameraStartX = (i % levelWidth) * SPRITE_SIZE;
            cameraStartY = (i / levelWidth) * SPRITE_SIZE;
        }
        else if (tmx->data[index] == TILE_ID_FINISH) {
            finishX = (i % levelWidth) * SPRITE_SIZE;
            finishY = (i / levelWidth) * SPRITE_SIZE;
        }
        else if (tmx->data[index] == TILE_ID_LEVEL_TRIGGER) {
            levelTriggers.push_back(LevelTrigger((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, levelTriggerCount++));
        }
        else if (tmx->data[index] == TILE_ID_ENEMY_1) {
            enemies.push_back(Enemy((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, enemyHealths[0], 0));
        }
        else if (tmx->data[index] == TILE_ID_ENEMY_2) {
            enemies.push_back(Enemy((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, enemyHealths[1], 1));
        }
        else if (tmx->data[index] == TILE_ID_ENEMY_3) {
            enemies.push_back(Enemy((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, enemyHealths[2], 2));
        }
        else if (tmx->data[index] == TILE_ID_ENEMY_4) {
            enemies.push_back(Enemy((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, enemyHealths[3], 3));
        }
        else if (tmx->data[index] == TILE_ID_ENEMY_5) {
            enemies.push_back(Enemy((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, enemyHealths[4], 4));
        }
        else {
            // Background tiles are non-solid
            background.push_back(Tile((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, tmx->data[index]));
        }
    }

    // maybe adjust position of tile so that don't need to bunch all up in corner while designing level

    // go backwards through parallax layers so that rendering is correct

    // Parallax Background Layer
    for (uint32_t i = 0; i < levelSize; i++) {
        uint32_t index = i + levelSize * 4;

        if (tmx->data[index] == TILE_ID_EMPTY) {
            // Is a blank tile, don't do anything
        }
        else {
            parallax.push_back(ParallaxTile((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, tmx->data[index], 1));
        }
    }

    // Parallax Foreground Layer
    for (uint32_t i = 0; i < levelSize; i++) {
        uint32_t index = i + levelSize * 3;

        if (tmx->data[index] == TILE_ID_EMPTY) {
            // Is a blank tile, don't do anything
        }
        else {
            parallax.push_back(ParallaxTile((i % levelWidth) * SPRITE_SIZE, (i / levelWidth) * SPRITE_SIZE, tmx->data[index], 0));
        }
    }


    // Reset player attributes
    player = Player(playerStartX, playerStartY, playerSelected);

    finish = Finish(finishX, finishY, finishFrames);
    
    // Reset camera position
    camera.x = cameraStartX;
    camera.y = cameraStartY;

    for (uint8_t i = 0; i < enemies.size(); i++) {
        enemies[i].set_player_position(&player.x, &player.y);
    }


    // Check there aren't any levelTriggers which have levelNumber >= LEVEL_COUNT
    levelTriggers.erase(std::remove_if(levelTriggers.begin(), levelTriggers.end(), [](LevelTrigger levelTrigger) { return levelTrigger.levelNumber >= LEVEL_COUNT; }), levelTriggers.end());

}

void start_level(uint8_t levelNumber) {
    gameState = GameState::STATE_IN_GAME;

    // Load level
    load_level(levelNumber);


    // Make sure player attribute setting is done after load_level call, since player is reassigned in that method
    player.locked = true;
    cameraIntro = true;

    gamePaused = false;

    open_transition();
}

void start_input_select() {
    gameState = GameState::STATE_INPUT_SELECT;

    open_transition();
}

void start_character_select() {
    gameState = GameState::STATE_CHARACTER_SELECT;

    // Load character select level
    load_level(LEVEL_COUNT + 1);

    if (playerSelected) {
        player = Player(playerStartX + SPRITE_SIZE * 7, playerStartY, 1);
    }
    else {
        player = Player(playerStartX, playerStartY, 0);
    }

    open_transition();
}

void start_menu() {
    gameState = GameState::STATE_MENU;

    // Load menu level
    load_level(LEVEL_COUNT);

    open_transition();
}

void start_level_select() {
    gameState = GameState::STATE_LEVEL_SELECT;

    // Load level select level
    load_level(LEVEL_COUNT + 2);

    if (currentLevelNumber != NO_LEVEL_SELECTED) {
        // Must have just completed a level
        // Place player next to finished level
        for (uint8_t i = 0; i < levelTriggers.size(); i++) {
            if (levelTriggers[i].levelNumber == currentLevelNumber) {
                playerStartX = levelTriggers[i].x + SPRITE_HALF * 3;
                playerStartY = levelTriggers[i].y;
                player.x = playerStartX;
                player.y = playerStartY;
                camera.x = player.x;
                camera.y = player.y;
            }
        }
    }
    else {
        // Must have just come from title/menu screen
        for (uint8_t i = 0; i < levelTriggers.size(); i++) {
            if (levelTriggers[i].levelNumber == allPlayerSaveData[playerSelected].levelReached) {
                playerStartX = levelTriggers[i].x + SPRITE_HALF * 3;
                playerStartY = levelTriggers[i].y;
                player.x = playerStartX;
                player.y = playerStartY;
                camera.x = player.x;
                camera.y = player.y;
            }
        }
    }

    // Make sure player attribute setting is done after load_level call, since player is reassigned in that method
    player.locked = true;

    open_transition();

    // Reset currentLevelNumber so no level is pre-selected
    currentLevelNumber = NO_LEVEL_SELECTED;
}

void start_game_lost() {
    gameState = GameState::STATE_LOST;

    open_transition();
}

void start_game_won() {
    gameState = GameState::STATE_WON;

    if (currentLevelNumber == allPlayerSaveData[playerSelected].levelReached) {
        allPlayerSaveData[playerSelected].levelReached = currentLevelNumber + 1;
    }

    if (allLevelSaveData[playerSelected][currentLevelNumber].score < player.score) {
        allLevelSaveData[playerSelected][currentLevelNumber].score = player.score;
    }
    if (allLevelSaveData[playerSelected][currentLevelNumber].enemiesKilled < player.enemiesKilled) {
        allLevelSaveData[playerSelected][currentLevelNumber].enemiesKilled = player.enemiesKilled;
    }
    if (allLevelSaveData[playerSelected][currentLevelNumber].time > player.levelTimer || allLevelSaveData[playerSelected][currentLevelNumber].time == 0.0f) {
        allLevelSaveData[playerSelected][currentLevelNumber].time = player.levelTimer;
    }

    // save level stats?

    //save_game_data();
    save_player_data(playerSelected);
    save_level_data(playerSelected, currentLevelNumber);

    open_transition();
}



void render_input_select() {
    render_background();

    background_rect(0);
    background_rect(1);


    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
    screen.text("Select Input Method", minimal_font, Point(SCREEN_MID_WIDTH, 10), true, TextAlign::center_center);

    screen.pen = gameSaveData.inputType == InputType::CONTROLLER ? Pen(inputSelectColour.r, inputSelectColour.g, inputSelectColour.b) : Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
    screen.text("Controller/32Blit", minimal_font, Point(SCREEN_MID_WIDTH, 50), true, TextAlign::center_center);

    screen.pen = gameSaveData.inputType == InputType::KEYBOARD ? Pen(inputSelectColour.r, inputSelectColour.g, inputSelectColour.b) : Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
    screen.text("Keyboard", minimal_font, Point(SCREEN_MID_WIDTH, 70), true, TextAlign::center_center);


    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);

    if (textFlashTimer < TEXT_FLASH_TIME * 0.6f) {
        screen.text(messageStrings[0][gameSaveData.inputType], minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 9), true, TextAlign::center_center);
    }
}

void render_character_select() {
    render_background();

    render_level();

    render_entities();

    if (playerSelected) {
        render_sprite(TILE_ID_PLAYER_1, Point(SCREEN_MID_WIDTH + playerStartX - camera.x, SCREEN_MID_HEIGHT + playerStartY - camera.y), SpriteTransform::HORIZONTAL);
        //screen.sprite(TILE_ID_PLAYER_1, Point(SCREEN_MID_WIDTH + playerStartX - camera.x, SCREEN_MID_HEIGHT + playerStartY - camera.y), SpriteTransform::HORIZONTAL);
    }
    else {
        render_sprite(TILE_ID_PLAYER_2, Point(SCREEN_MID_WIDTH + playerStartX - camera.x + SPRITE_SIZE * 7, SCREEN_MID_HEIGHT + playerStartY - camera.y));
        //screen.sprite(TILE_ID_PLAYER_2, Point(SCREEN_MID_WIDTH + playerStartX - camera.x + SPRITE_SIZE * 7, SCREEN_MID_HEIGHT + playerStartY - camera.y));
    }


    background_rect(0);
    background_rect(1);

    screen.pen = Pen(hudBackground.r, hudBackground.g, hudBackground.b, hudBackground.a);
    screen.rectangle(Rect(0, SCREEN_HEIGHT - (SPRITE_SIZE + 12 + 12), SCREEN_WIDTH, 12));

    screen.pen = Pen(levelTriggerParticleColours[1].r, levelTriggerParticleColours[1].g, levelTriggerParticleColours[1].b);
    screen.text("Player " + std::to_string(playerSelected + 1) + " (Save " + std::to_string(playerSelected + 1)  + ")", minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 10 - 12), true, TextAlign::center_center);

    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
    screen.text("Select Player", minimal_font, Point(SCREEN_MID_WIDTH, 10), true, TextAlign::center_center);

    if (textFlashTimer < TEXT_FLASH_TIME * 0.6f) {
        screen.text(messageStrings[0][gameSaveData.inputType], minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 9), true, TextAlign::center_center);
    }
}

void render_menu() {
    render_background();

    render_level();

    render_entities();


    background_rect(0);
    background_rect(1);

    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
    screen.text("Super Square Bros.", minimal_font, Point(SCREEN_MID_WIDTH, 10), true, TextAlign::center_center);

    if (textFlashTimer < TEXT_FLASH_TIME * 0.6f) {
        screen.text(messageStrings[0][gameSaveData.inputType], minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 9), true, TextAlign::center_center);
    }
}

void render_level_select() {
    render_background();

    render_level();

    render_entities();

    render_level_triggers();


    background_rect(0);

    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
    screen.text("Select level", minimal_font, Point(SCREEN_MID_WIDTH, 10), true, TextAlign::center_center);

    render_nearby_level_info();
}

void render_game() {
    render_background();

    render_level();

    render_finish();

    render_entities();


    if (cameraIntro) {
        // Level <num> message
        background_rect(0);
        screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
        screen.text("Level " + std::to_string(currentLevelNumber + 1), minimal_font, Point(SCREEN_MID_WIDTH, 10), true, TextAlign::center_center);

        // Press <key> to skip intro message
        background_rect(1);
        if (textFlashTimer < TEXT_FLASH_TIME * 0.6f) {
            screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
            screen.text(messageStrings[1][gameSaveData.inputType], minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 9), true, TextAlign::center_center);
        }
    }
    else if (gamePaused) {
        screen.pen = Pen(hudBackground.r, hudBackground.g, hudBackground.b, hudBackground.a);
        screen.rectangle(Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

        screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);
        screen.text("Game Paused", minimal_font, Point(SCREEN_MID_WIDTH, 10), true, TextAlign::center_center);

        if (textFlashTimer < TEXT_FLASH_TIME * 0.6f) {
            screen.text(messageStrings[3][gameSaveData.inputType], minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 9), true, TextAlign::center_center);
        }
    }
    else {
        render_hud();
    }
}



void render_game_lost() {
    render_background();

    background_rect(0);
    background_rect(1);

    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);

    screen.text("You lost.", minimal_font, Point(SCREEN_MID_WIDTH, 10), true, TextAlign::center_center);

    display_stats();

    if (textFlashTimer < TEXT_FLASH_TIME * 0.6f) {
        screen.text(messageStrings[2][gameSaveData.inputType], minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 9), true, TextAlign::center_center);
    }
}


void render_game_won() {
    render_background();

    background_rect(0);
    background_rect(1);

    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);

    screen.text("You won.", minimal_font, Point(SCREEN_MID_WIDTH, 10), true, TextAlign::center_center);

    display_stats();

    if (textFlashTimer < TEXT_FLASH_TIME * 0.6f) {
        screen.text(messageStrings[2][gameSaveData.inputType], minimal_font, Point(SCREEN_MID_WIDTH, SCREEN_HEIGHT - 9), true, TextAlign::center_center);
    }
}



void update_enemies(float dt, ButtonStates buttonStates) {
    for (int i = 0; i < enemies.size(); i++) {
        enemies[i].update(dt, buttonStates);
    }
}

void update_level_triggers(float dt, ButtonStates buttonStates) {
    for (int i = 0; i < levelTriggers.size(); i++) {
        levelTriggers[i].update(dt, buttonStates);
        if (!levelTriggers[i].visible && levelTriggers[i].particles.size() == 0) {
            currentLevelNumber = levelTriggers[i].levelNumber;
        }
    }

    levelTriggers.erase(std::remove_if(levelTriggers.begin(), levelTriggers.end(), [](LevelTrigger levelTrigger) { return (!levelTrigger.visible && levelTrigger.particles.size() == 0); }), levelTriggers.end());
}

void update_coins(float dt, ButtonStates buttonStates) {
    for (int i = 0; i < coins.size(); i++) {
        coins[i].update(dt, buttonStates);
    }
}


void update_projectiles(float dt, ButtonStates buttonStates) {
    for (uint8_t i = 0; i < projectiles.size(); i++) {
        projectiles[i].update(dt, buttonStates);
    }

    //  Allow enemies to get hit?
    /*for (uint8_t i = 0; i < enemies.size(); i++) {

    }*/

    if (!player.is_immune()) {
        uint8_t projectileCount = projectiles.size();
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](Projectile projectile) { return (projectile.x + SPRITE_HALF > player.x && projectile.x < player.x + SPRITE_SIZE && projectile.y + SPRITE_HALF > player.y && projectile.y < player.y + SPRITE_SIZE); }), projectiles.end());
        if (projectileCount - projectiles.size() > 0) {
            player.health -= 1;
            player.set_immune();
        }
    }
    

    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](Projectile projectile) { return (std::abs(projectile.x - player.x) > SCREEN_TILE_SIZE || std::abs(projectile.y - player.y) > SCREEN_HEIGHT); }), projectiles.end());

}


void update_input_select(float dt, ButtonStates buttonStates) {
    if (transition[0].is_ready_to_open()) {
        start_character_select();
    }
    else if (transition[0].is_open()) {
        if (gameSaveData.inputType == InputType::CONTROLLER) {
            if (buttonStates.DOWN) {
                gameSaveData.inputType = InputType::KEYBOARD;
            }
        }
        else if (gameSaveData.inputType == InputType::KEYBOARD) {
            if (buttonStates.UP) {
                gameSaveData.inputType = InputType::CONTROLLER;
            }
        }


        if (buttonStates.A == 2) {
            close_transition();

            // Save inputType
            save_game_data();
        }
    }
}

void update_character_select(float dt, ButtonStates buttonStates) {
    // Dummy states is used to make selected player continually jump (sending A key pressed).
    ButtonStates dummyStates = { 0 };
    dummyStates.A = 1;
    player.update(dt, dummyStates);


    if (buttonStates.RIGHT && !playerSelected) {
        playerSelected = 1;
        player = Player(playerStartX + SPRITE_SIZE * 7, playerStartY, 1);
        player.lastDirection = 0;
    }
    else if (buttonStates.LEFT && playerSelected) {
        playerSelected = 0;
        player = Player(playerStartX, playerStartY, 0);
        player.lastDirection = 1;
    }

    if (transition[0].is_ready_to_open()) {
        if (menuBack) {
            menuBack = false;
            start_input_select();
        }
        else {
            start_menu();
        }
    }
    else if (transition[0].is_open()) {
        if (buttonStates.A == 2) {
            close_transition();
        }
        else if (buttonStates.Y == 2) {
            menuBack = true;
            close_transition();
        }
    }
}

void update_menu(float dt, ButtonStates buttonStates) {
    update_coins(dt, buttonStates);


    if (transition[0].is_ready_to_open()) {
        if (menuBack) {
            menuBack = false;
            start_character_select();
        }
        else {
            start_level_select();
        }
    }
    else if (transition[0].is_open()) {
        if (buttonStates.A == 2) {
            close_transition();
        }
        else if (buttonStates.Y == 2) {
            menuBack = true;
            close_transition();
        }
    }
}

void update_level_select(float dt, ButtonStates buttonStates) {
    player.update(dt, buttonStates);

    update_enemies(dt, buttonStates);

    update_level_triggers(dt, buttonStates);

    //update_coins(dt, buttonStates);

    //update_projectiles(dt, buttonStates);

    //finish.update(dt, buttonStates);

    // Button handling

    //printf("%u, %u, %f, %f, %f, %f\n", player.health, player.lives, player.x, camera.x, player.y, camera.y);





    if (transition[0].is_ready_to_open()) {
        if (menuBack) {
            menuBack = false;
            start_menu();
        }
        else {
            start_level(currentLevelNumber);
        }
    }
    else if (transition[0].is_open()) {
        if (cameraRespawn) {
            camera.ease_out_to(dt, player.x, player.y);

            if (std::abs(player.x - camera.x) < CAMERA_RESPAWN_LOCK_MIN && std::abs(player.x - camera.x) < CAMERA_RESPAWN_LOCK_MIN) {
                // for respawns
                cameraRespawn = false;
                player.locked = false;

                // to stop player completely dying
                player.lives = 3;
            }
        }
        else {
            camera.ease_out_to(dt, player.x, player.y);
            player.locked = false;
        }


        if (currentLevelNumber != NO_LEVEL_SELECTED) {
            close_transition();
        }
        else if (buttonStates.Y == 2) {
            menuBack = true;
            close_transition();
        }
    }
}

void update_game(float dt, ButtonStates buttonStates) {
    if (!gamePaused) {
        // Game isn't paused, update it.
        player.update(dt, buttonStates);

        update_enemies(dt, buttonStates);

        update_coins(dt, buttonStates);

        update_projectiles(dt, buttonStates);

        finish.update(dt, buttonStates);

        if (player.x + SPRITE_SIZE > finish.x + 3 && player.x < finish.x + SPRITE_SIZE - 3 && player.y + SPRITE_SIZE > finish.y + 4 && player.y < finish.y + SPRITE_SIZE) {
            // lock player to finish
            player.x = finish.x;
            player.y = finish.y - 1;
        }


    }
    //if (player.x + SPRITE_SIZE > finish.x - SPRITE_SIZE && player.x < finish.x + SPRITE_SIZE * 2 && player.y + SPRITE_SIZE > finish.y - SPRITE_SIZE && player.y < finish.y + SPRITE_SIZE * 2) {
    //    // 'pull' player to finish

    //    player.x += (finish.x - player.x) * 4 * dt;
    //    player.y += (finish.y - player.y) * 4 * dt;
    //}

    if (transition[0].is_ready_to_open()) {
        if (player.lives) {
            // Player completed level
            start_game_won();
        }
        else {
            // Player failed level
            start_game_lost();
        }

        //start_level_select();
    }
    else if (transition[0].is_open()) {
        if (!gamePaused) {
            if (cameraIntro) {
                //camera.linear_to(dt, cameraStartX, cameraStartY, player.x, player.y, CAMERA_PAN_TIME);
                camera.linear_to(dt, cameraStartX, cameraStartY, player.x, player.y, CAMERA_PAN_TIME);

                //camera.ease_to(dt/5, player.x, player.y);

                if (player.x == camera.x && player.y == camera.y) {
                    cameraIntro = false;
                    player.locked = false;
                    // Make player immune when spawning?
                    //player.set_immune();
                }

                if (buttonStates.A == 2) {
                    cameraIntro = false;
                    cameraRespawn = true; // goes to player faster
                }
            }
            else if (cameraRespawn) {
                camera.ease_out_to(dt, player.x, player.y);

                if (std::abs(player.x - camera.x) < CAMERA_RESPAWN_LOCK_MIN && std::abs(player.x - camera.x) < CAMERA_RESPAWN_LOCK_MIN) {
                    // for respawns
                    cameraRespawn = false;
                    player.locked = false;
                }
            }
            else {
                camera.ease_out_to(dt, player.x, player.y);


                // Handle level end
                if (player.x + SPRITE_SIZE > finish.x + 3 && player.x < finish.x + SPRITE_SIZE - 3 && player.y + SPRITE_SIZE > finish.y + 4 && player.y < finish.y + SPRITE_SIZE) {
                    close_transition();
                }

                // Handle player life
                if (player.lives == 0 && player.particles.size() == 0) {
                    // Take player back to start? or just level select
                    close_transition();
                }
            }
        }



        // Allow player to pause game
        if (buttonStates.Y == 2) {
            gamePaused = !gamePaused;
        }
    }
}

void update_game_lost(float dt, ButtonStates buttonStates) {
    if (transition[0].is_ready_to_open()) {
        start_level_select();
    }
    else if (transition[0].is_open()) {
        if (buttonStates.A == 2) {
            close_transition();
        }
    }
}


void update_game_won(float dt, ButtonStates buttonStates) {
    if (transition[0].is_ready_to_open()) {
        start_level_select();
    }
    else if (transition[0].is_open()) {
        if (buttonStates.A == 2) {
            close_transition();
        }
    }
}




///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() {
    set_screen_mode(ScreenMode::lores);

    screen.sprites = Surface::load(asset_sprites);


    // Populate transition array
    for (uint8_t y = 0; y < SCREEN_HEIGHT / SPRITE_SIZE; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH / SPRITE_SIZE; x++) {
            transition[y * (SCREEN_WIDTH / SPRITE_SIZE) + x] = AnimatedTransition(x * SPRITE_SIZE, y * SPRITE_SIZE, transitionFramesOpen, transitionFramesClose);
        }
    }

    bool success = read_save(gameSaveData);

    // Load save data
    // Attempt to load the first save slot.
    if (success) {
#ifdef RESET_SAVE_DATA
        if (gameSaveData.version < get_version()) {
            printf("Warning: Saved game data is out of date, save version is %d, but firmware version is %d (v%d.%d.%d)\n", gameSaveData.version, get_version(), VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
            printf("Resetting save data...\n");

            success = false;
            reset_save();
        }
#endif
    }

    if (success) {

        // Loaded sucessfully!
        gameState = GameState::STATE_CHARACTER_SELECT;

        // Load character select level
        load_level(LEVEL_COUNT + 1);

    }
    else {
        // No save file or it failed to load, set up some defaults.
        gameSaveData.version = get_version();

        gameSaveData.inputType = InputType::CONTROLLER;

        // gameState is by default set to STATE_INPUT_SELECT

#ifdef TARGET_32BLIT_HW
        // If it's a 32blit, don't bother asking
        gameState = GameState::STATE_CHARACTER_SELECT;

        // Save inputType
        save_game_data();

        // Load character select level
        load_level(LEVEL_COUNT + 1);
#endif
    }


    allPlayerSaveData[0] = load_player_data(0);
    allPlayerSaveData[1] = load_player_data(1);

    // Load level data
    for (uint8_t i = 0; i < LEVEL_COUNT; i++) {
        allLevelSaveData[0][i] = load_level_data(0, i);
    }
    for (uint8_t i = 0; i < LEVEL_COUNT; i++) {
        allLevelSaveData[1][i] = load_level_data(1, i);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t time) {

    // clear the screen -- screen is a reference to the frame buffer and can be used to draw all things with the 32blit
    screen.clear();

    // draw some text at the top of the screen
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(defaultWhite.r, defaultWhite.g, defaultWhite.b);

    if (gameState == GameState::STATE_INPUT_SELECT) {
        render_input_select();
    }
    else if (gameState == GameState::STATE_CHARACTER_SELECT) {
        render_character_select();
    }
    else if (gameState == GameState::STATE_MENU) {
        render_menu();
    }
    else if (gameState == GameState::STATE_LEVEL_SELECT) {
        render_level_select();
    }
    else if (gameState == GameState::STATE_IN_GAME) {
        render_game();
    }
    else if (gameState == GameState::STATE_LOST) {
        render_game_lost();
    }
    else if (gameState == GameState::STATE_WON) {
        render_game_won();
    }

    render_transition();

    screen.pen = Pen(0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {
    // Get dt
    dt = (time - lastTime) / 1000.0;
    lastTime = time;

    textFlashTimer += dt;
    if (textFlashTimer >= TEXT_FLASH_TIME) {
        textFlashTimer -= TEXT_FLASH_TIME;
    }

    //printf("%f\n", dt);

    // Update buttonStates
    if (buttons & Button::A) {
        if (buttonStates.A == 2) {
            buttonStates.A = 1;
        }
        else if (buttonStates.A == 0) {
            buttonStates.A = 2;
        }
    }
    else {
        buttonStates.A = 0;
    }

    if (buttons & Button::B) {
        if (buttonStates.B == 2) {
            buttonStates.B = 1;
        }
        else if (buttonStates.B == 0) {
            buttonStates.B = 2;
        }
    }
    else {
        buttonStates.B = 0;
    }

    if (buttons & Button::X) {
        if (buttonStates.X == 2) {
            buttonStates.X = 1;
        }
        else if (buttonStates.X == 0) {
            buttonStates.X = 2;
        }
    }
    else {
        buttonStates.X = 0;
    }

    if (buttons & Button::Y) {
        if (buttonStates.Y == 2) {
            buttonStates.Y = 1;
        }
        else if (buttonStates.Y == 0) {
            buttonStates.Y = 2;
        }
    }
    else {
        buttonStates.Y = 0;
    }

    if (buttons & Button::DPAD_UP) {
        if (buttonStates.UP == 2) {
            buttonStates.UP = 1;
        }
        else if (buttonStates.UP == 0) {
            buttonStates.UP = 2;
        }
    }
    else {
        buttonStates.UP = 0;
    }

    if (buttons & Button::DPAD_DOWN) {
        if (buttonStates.DOWN == 2) {
            buttonStates.DOWN = 1;
        }
        else if (buttonStates.DOWN == 0) {
            buttonStates.DOWN = 2;
        }
    }
    else {
        buttonStates.DOWN = 0;
    }

    if (buttons & Button::DPAD_LEFT) {
        if (buttonStates.LEFT == 2) {
            buttonStates.LEFT = 1;
        }
        else if (buttonStates.LEFT == 0) {
            buttonStates.LEFT = 2;
        }
    }
    else {
        buttonStates.LEFT = 0;
    }

    if (buttons & Button::DPAD_RIGHT) {
        if (buttonStates.RIGHT == 2) {
            buttonStates.RIGHT = 1;
        }
        else if (buttonStates.RIGHT == 0) {
            buttonStates.RIGHT = 2;
        }
    }
    else {
        buttonStates.RIGHT = 0;
    }


    // Update game
    if (gameState == GameState::STATE_INPUT_SELECT) {
        update_input_select(dt, buttonStates);
    }
    else if (gameState == GameState::STATE_CHARACTER_SELECT) {
        update_character_select(dt, buttonStates);
    }
    else if (gameState == GameState::STATE_MENU) {
        update_menu(dt, buttonStates);
    }
    else if (gameState == GameState::STATE_LEVEL_SELECT) {
        update_level_select(dt, buttonStates);
    }
    else if (gameState == GameState::STATE_IN_GAME) {
        update_game(dt, buttonStates);
    }
    else if (gameState == GameState::STATE_LOST) {
        update_game_lost(dt, buttonStates);
    }
    else if (gameState == GameState::STATE_WON) {
        update_game_won(dt, buttonStates);
    }

    update_transition(dt, buttonStates);
}