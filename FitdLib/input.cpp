#include "common.h"
#include <SDL.h>

extern float nearVal;
extern float farVal;
extern float cameraZoom;
extern float fov;

extern bool debuggerVar_debugMenuDisplayed;

void handleKeyDown(SDL_Event& event)
{
    switch (event.key.keysym.scancode)
    {
    case SDL_SCANCODE_GRAVE:
        debuggerVar_debugMenuDisplayed ^= 1;
        break;
    }
}

void readKeyboard(void)
{
    SDL_Event event;
    int size;
    int j;
    const unsigned char *keyboard;

    JoyD = 0;
    Click = 0;
    key = 0;

    while (SDL_PollEvent(&event)) {

        switch (event.type) {
        case SDL_KEYDOWN:
            handleKeyDown(event);
            break;
        case SDL_QUIT:
            cleanupAndExit();
            break;
        }

    }

    debuggerVar_fastForward = false;

    keyboard = SDL_GetKeyboardState(&size);

    for (j = 0; j < size; j++)
    {
        if (keyboard[j])
        {
            switch (j)
            {
                /*        case SDLK_z:
                nearVal-=1;
                break;
                case SDLK_x:
                nearVal+=1;
                break;
                case SDLK_a:
                cameraZoom-=100;
                break;
                case SDLK_s:
                cameraZoom+=100;
                break;
                case SDLK_w:
                {
                float factor = (float)cameraY/(float)cameraZ;
                cameraY+=1;
                cameraZ=(float)cameraY/factor;
                break;
                }
                case SDLK_q:
                {
                float factor = (float)cameraY/(float)cameraZ;
                cameraY-=1;
                cameraZ=(float)cameraY/factor;
                break;
                }
                case SDLK_e:
                fov-=1;
                break;
                case SDLK_r:
                fov+=2;
                break; */
            case SDL_SCANCODE_RETURN:
                key = 0x1C;
                break;
            case SDL_SCANCODE_ESCAPE:
                key = 0x1B;
                break;
            case SDL_SCANCODE_UP:
                JoyD |= 1;
                break;

            case SDL_SCANCODE_DOWN:
                JoyD |= 2;
                break;

            case SDL_SCANCODE_RIGHT:
                JoyD |= 8;
                break;

            case SDL_SCANCODE_LEFT:
                JoyD |= 4;
                break;
            case SDL_SCANCODE_SPACE:
                Click = 1;
                break;
#ifdef FITD_DEBUGGER
            case SDL_SCANCODE_O:
                debufferVar_topCameraZoom += 100;
                break;
            case SDL_SCANCODE_P:
                debufferVar_topCameraZoom -= 100;
                break;
            case SDL_SCANCODE_T:
                debuggerVar_topCamera = true;
                backgroundMode = backgroundModeEnum_3D;
                break;
            case SDL_SCANCODE_Y:
                debuggerVar_topCamera = false;
                backgroundMode = backgroundModeEnum_2D;
                flagInitView = 1;
                break;
            case SDL_SCANCODE_C:
                debuggerVar_noHardClip = !debuggerVar_noHardClip;
                break;
            case SDL_SCANCODE_D:
                debugger_enterMainDebug();
                break;
            case SDL_SCANCODE_B:
                backgroundMode = backgroundModeEnum_3D;
                break;
            case SDL_SCANCODE_N:
                backgroundMode = backgroundModeEnum_2D;
                break;
            case SDL_SCANCODE_F:
                debuggerVar_fastForward = true;
                break;


#endif
            }
        }
    }
#ifdef FITD_DEBUGGER
    if (debuggerVar_topCamera)
    {
        backgroundMode = backgroundModeEnum_3D;
    }
#endif
}
