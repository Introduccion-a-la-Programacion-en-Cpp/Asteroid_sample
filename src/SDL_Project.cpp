// SDL_Project.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_TTF.h"
#include "SDL_mixer.h"
#include "InputModule.h"
#include "GSLogoState.h"

using namespace std;

///////// Definicion de estructuras /////////////
#include "StructsDef.h"
///////// Definicion de estructuras /////////////


///////// Variables y Constantes Globales /////////////
const int WIDTH = 640;
const int HEIGHT = 480;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_bool isGameRunning = SDL_TRUE;
float time_multiplier = 1.0f;
InputState gameInputState;
SpriteAssets spritesAssets;
TextAssets textAssets;
BgmAssets musicAssets;
GameStages gameStages;

ResourceManager resourceManager;

///////// Variables y Constantes Globales /////////////

///////// Funciones de inicializacion y destruccion /////////////
void initEngine()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

    TTF_Init();

    int initted = Mix_Init(MIX_INIT_MP3);
    if ( (initted & MIX_INIT_MP3) != MIX_INIT_MP3) {
        cout << "Mix_Init: Failed to init required ogg and mod support!" << endl;
        cout << "Mix_Init: " << Mix_GetError() << endl;
        // handle error
    }

    window = SDL_CreateWindow("SDL2 Template Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Initializing Resource Manager
    resourceManager.spritesAssets = &spritesAssets;
    resourceManager.textAssets = &textAssets;
    resourceManager.musicAssets = &musicAssets;
    resourceManager.gameStages = &gameStages;
    resourceManager.inputState = &gameInputState;

    // Starting Game stage
    GameStage logoGameStage;
    logoGameStage.game_stageID = GS_LOGO;
    logoGameStage.stage_name = "Logo";

    gameStages.push(logoGameStage);
}

void destroyEngine() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

///////// Funciones de inicializacion y destruccion /////////////


///////// Funciones de carga y liberacion de recursos /////////////

void loadAssets() {

    // Cargo el Logo principal.
    string filePath = "assets/img/logo.png";
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
    SDL_Rect dest;
    dest.x = WIDTH >> 2;
    dest.y = 0;
    dest.w = WIDTH >> 1;
    dest.h = HEIGHT >> 1;

    Sprite logoSprite;
    logoSprite.dest = dest;
    logoSprite.texture = texture;
    spritesAssets.push_back(logoSprite);

    // Cargo el texto...
    string fontfilePath = "assets/fonts/arial.ttf";

    TTF_Font* Sans = TTF_OpenFont(fontfilePath.c_str(), 24); //this opens a font style and sets a size

    SDL_Color White = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "Project ready...", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

    SDL_Rect Message_rect; //create a rect
    Message_rect.w = WIDTH * 0.65; // controls the width of the rect
    Message_rect.h = HEIGHT * 0.10; // controls the height of the rect
    Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1);  //controls the rect's x coordinate 
    Message_rect.y = HEIGHT >> 1; // controls the rect's y coordinte

    Text mainText;
    mainText.font = Sans;
    mainText.color = White;
    mainText.surface = surfaceMessage;
    mainText.texture = Message;
    mainText.dest = Message_rect;

    textAssets.push_back(mainText);

    // Cargo Sonidos y BGM
    string soundFilePath = "assets/bgm/littleidea.mp3";
    Mix_Music* music;
    music = Mix_LoadMUS(soundFilePath.c_str());
    
    Bgm bgm01;
    bgm01.music = music;

    musicAssets.push_back(bgm01);

}

void unloadAssets() {
    for (int i = 0; i < spritesAssets.size(); i++) {
        SDL_DestroyTexture(spritesAssets[i].texture);
    }

    for (int i = 0; i < textAssets.size(); i++) {
        SDL_FreeSurface(textAssets[i].surface);
        SDL_DestroyTexture(textAssets[i].texture);
    }

    for (int i = 0; i < musicAssets.size(); i++) {
        Mix_FreeMusic(musicAssets[i].music);
    }
}

///////// Funciones de carga y liberacion de recursos /////////////

///////// Funciones de actualizacion y pintado /////////////

void inputUpdate() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        
        switch (event.type) {
        case SDL_KEYDOWN:
            onKeyDown(event.key.keysym.sym, gameInputState);
            break;
        case SDL_KEYUP:
            onKeyUp(event.key.keysym.sym, gameInputState);
            break;
        case SDL_MOUSEBUTTONDOWN:
            break;
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_QUIT:
            isGameRunning = SDL_FALSE;
            break;
        default:
            break;
        }
    }
}

// Para ser usado en distintos contadores..
float timer = 1.0f * 1000; // 1000 ms

void updateGame(float deltaTime) {
    const float BLINK_SPEED = 5.0f;

    timer -= BLINK_SPEED * deltaTime;

    // para efecto de parpadeo...
    if (timer <= 0.0f) {
        timer = 1.0f * 1000;
        textAssets[0].isVisible = !textAssets[0].isVisible;
    }

    // Small state machine using stack collection
    switch (gameStages.top().game_stageID)
    {
    case GS_LOGO:
        GSLogoStateUpdate(deltaTime, resourceManager);
        break;
    case GS_MAIN_MENU:
        break;
    case GS_GAMEPLAY:
        break;
    case GS_INVALID:
    default:
        break;
    }

}

void render()
{
    // Limpio la pantalla 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Pinto todos los sprites...
    for (int i = 0; i < spritesAssets.size(); i++) {
        if (spritesAssets[i].isVisible) {
            SDL_RenderCopy(renderer, spritesAssets[i].texture, NULL, &spritesAssets[i].dest);
        }
    }

    // Pinto todos los textos...
    for (int i = 0; i < textAssets.size(); i++) {
        if (textAssets[i].isVisible) {
            SDL_RenderCopy(renderer, textAssets[i].texture, NULL, &textAssets[i].dest);
        }
    }

    // Presento la imagen en pantalla
    SDL_RenderPresent(renderer);
}

///////// Funciones de actualizacion y pintado /////////////

///////// Funcione principal y GameLoop /////////////

int main(int argc, char* argv[])
{
    initEngine();

    // Cargo Assets
    loadAssets();

    Mix_PlayMusic(musicAssets[0].music, -1);

    Uint64 currentTime = SDL_GetTicks64();

    while (isGameRunning) {

        Uint64 previousTime = currentTime;

        currentTime = SDL_GetTicks64();

        Uint64 deltaTime = currentTime - previousTime;

        inputUpdate();

        updateGame(deltaTime * time_multiplier);

        render();
    }

    // Detendo la musica 
    Mix_HaltMusic();

    // Descargo Assets
    unloadAssets(); 
    destroyEngine();
    return 0;
}

///////// Funcione principal y GameLoop /////////////

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
