#pragma once

using namespace std;
#include <fstream>
#include <vector>
#include <stack>
#include "SDL.h"
#include "SDL_TTF.h"
#include "SDL_mixer.h"

const int WIDTH = 640;
const int HEIGHT = 480;

typedef struct _InputState {
    bool up = false;
    bool down = false;
    bool right = false;
    bool left = false;
    bool fire = false;
}InputState;

typedef struct _Sprite {
    SDL_Texture* texture;
    SDL_Rect dest;
    bool isVisible = true;
    SDL_Color fx_color = { 255, 255, 255, 0 }; //Te be used as FX color
    double angle = 0.0f; 
    SDL_RendererFlip flip = SDL_FLIP_NONE;
}Sprite;

typedef struct _Text {
    TTF_Font* font;
    SDL_Color color;
    SDL_Surface* surface;
    SDL_Texture* texture;
    SDL_Rect dest;
    bool isVisible = true;
}Text;

typedef struct _Bgm {
    Mix_Music* music;
}Bgm;

enum GAME_STAGES
{
    GS_INVALID = -1,
    GS_LOGO = 0,
    GS_MAIN_MENU,
    GS_GAMEPLAY,
    GS_GAMEPAUSE,
    GS_GAMEOVER,
    GS_CREDITS
};

typedef struct _GameStage {
    string stage_name;
    int game_stageID = GS_INVALID;
    int level;
}GameStage;


typedef vector<Sprite> SpriteAssets;
typedef vector<Text> TextAssets;
typedef vector<Bgm> BgmAssets;
typedef stack<GameStage> GameStages;

typedef struct _ResourceManager {
    SpriteAssets* spritesAssets;
    TextAssets* textAssets;
    BgmAssets* musicAssets;
    GameStages* gameStages;
    InputState* inputState;
    SDL_Renderer* renderer;
}ResourceManager;