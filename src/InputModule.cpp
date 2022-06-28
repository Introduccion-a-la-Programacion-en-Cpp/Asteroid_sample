#include "InputModule.h"
#include "StructsDef.h"

void onKeyUp(int keyCode, InputState& input) {
    switch (keyCode) {
    case SDLK_UP:
        input.up = false;
        break;
    case SDLK_DOWN:
        input.down = false;
        break;
    case SDLK_LEFT:
        input.left = false;
        break;
    case SDLK_RIGHT:
        input.right = false;
        break;
    case SDLK_SPACE:
        input.fire = false;
        break;
    default:
        break;
    }
}

void onKeyDown(int keyCode, InputState& input) {
    switch (keyCode) {
    case SDLK_UP:
        input.up = true;
        break;
    case SDLK_DOWN:
        input.down = true;
        break;
    case SDLK_LEFT:
        input.left = true;
        break;
    case SDLK_RIGHT:
        input.right = true;
        break;
    case SDLK_SPACE:
        input.fire = true;
        break;
    default:
        break;
    }
}