#include "common.h"

/***************************************************************************
mainSDL.cpp  -  description
-------------------
begin                : Mon Jun 3 2002
copyright            : (C) 2002 by Yaz0r
email                : yaz0r@yaz0r.net
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "bgfxGlue.h"
#include <SDL.h>
#include <SDL_thread.h>
#include "osystem.h"
#include "osystemAL.h"
#include <backends/imgui_impl_sdl.h>

void detectGame(void);
void renderGameWindow();

int osystem_mouseRight;
int osystem_mouseLeft;

void osystem_delay(int time)
{
    SDL_Delay(time);
}

void osystem_updateImage()
{
}

/*void OSystem::getMouseStatus(mouseStatusStruct * mouseData)
{

SDL_GetMouseState(&mouseData->X, &mouseData->Y);

mouseData->left = mouseLeft;
mouseData->right = mouseRight;

mouseLeft = 0;
mouseRight = 0;
}*/

#ifdef WIN32
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif

void OPL_musicPlayer(void *udata, Uint8 *stream, int len)
{
    musicUpdate(udata, stream, len);
}

extern "C" {
    void Sound_Quit(void);
}

void Sound_Quit(void)
{
}

extern "C" {
    char homePath[256] = "";
    int FitdMain(void* unkused);
}

SDL_semaphore* startOfRender = NULL;
SDL_semaphore* endOfRender = NULL;

//SDL_sem* emptyCount = NULL;
//SDL_sem* fullCount = NULL;

bool bFirst = true;

int FitdInit(int argc, char* argv[])
{
#ifdef WIN32
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif
    startOfRender = SDL_CreateSemaphore(0);
    endOfRender = SDL_CreateSemaphore(0);

    osystem_init();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0)
    {
        assert(false);
    }

    unsigned int flags = 0;
    flags |= SDL_WINDOW_RESIZABLE;
    flags |= SDL_WINDOW_ALLOW_HIGHDPI;

#ifdef __IPHONEOS__
    flags |= SDL_WINDOW_FULLSCREEN;
#endif

    int resolution[2] = { 1280, 960 };

    gWindowBGFX = SDL_CreateWindow("FITD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resolution[0], resolution[1], flags);
    
    char version[256];

    getVersion(version);

    printf(version);

    detectGame();
    
    createBgfxInitParams();
    
    SDL_CreateThread(FitdMain, "FitdMainThread", NULL);

    unsigned long int t_start = SDL_GetTicks();
    unsigned long int t_lastUpdate = t_start;

    int FRAMES_PER_SECOND = 25;

    u32 startOfPreviousFrame = SDL_GetTicks();

    while (1)
    {
        u32 startOfFrame = SDL_GetTicks();

        assert(startOfPreviousFrame <= startOfFrame);

        u32 tickDifference = startOfFrame - startOfPreviousFrame;

        if (tickDifference < 1000 / FRAMES_PER_SECOND)
        {
            //Sleep the remaining frame time
            //SDL_Delay((1000 / FRAMES_PER_SECOND) - tickDifference);
        }

        startOfPreviousFrame = startOfFrame;

        unsigned long int t_sinceStart = SDL_GetTicks();

        int delta = 0;

        //if(t_sinceStart + 10 > t_lastUpdate)
        {
            delta = (t_sinceStart - t_lastUpdate);
            t_lastUpdate = t_sinceStart;
        }

        readKeyboard();

        osystemAL_udpate();

        SDL_PumpEvents();

        SDL_GL_MakeCurrent(NULL, NULL);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT:
                gCloseApp = true;
                break;
            default:
                break;
            }
        }
        
        SDL_SemPost(startOfRender);

        SDL_SemWait(endOfRender);
        
        //SDL_RenderPresent(SDL_GetRenderer(gWindowBGFX));
    }

    return 0;
}

u32 lastFrameTime = 0;

#define FRAMES_PER_SECOND 25

u32 osystem_startOfFrame()
{
    SDL_SemWait(startOfRender);

    StartFrame();
    osystem_startFrame();

    static bool firstFrame = true;
    if (firstFrame)
    {
        //
#ifdef USE_IMGUI
        //ImGui_ImplSdlGL3_Init(sdl_window);
#endif
        lastFrameTime = SDL_GetTicks();

        firstFrame = false;
    }

    u32 numFramesToAdvance = (SDL_GetTicks() - lastFrameTime) / (1000 / FRAMES_PER_SECOND);

    if (numFramesToAdvance == 0)
        numFramesToAdvance = 1;

    lastFrameTime = SDL_GetTicks();

#ifdef USE_IMGUI
    //ImGui_ImplSdlGL3_NewFrame(sdl_window);
#endif

    return numFramesToAdvance;
}

void osystem_endOfFrame()
{
    osystem_flushPendingPrimitives();

#ifdef FITD_DEBUGGER
    debugger_draw();
#endif

#ifdef USE_IMGUI
    //ImGui::Render();
#endif

   // osystem_flip(NULL);

    renderGameWindow();

    EndFrame();

    if (bFirst)
        bFirst = false;

    SDL_SemPost(endOfRender);
    //SDL_SemPost(emptyCount);

}

int fileExists(const char* name)
{
    FILE* fHandle;

    fHandle = fopen(name, "rb");

    if (fHandle)
    {
        fclose(fHandle);
        return 1;
    }
    return 0;
}

void osystem_init()  // that's the constructor of the system dependent
// object used for the SDL port
{
    const unsigned char *keyboard;
    int size;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        assert(0);
    }

    // SDL_ShowCursor (SDL_DISABLE);

    // SDL_EnableUNICODE (SDL_ENABLE); // not much used in fact

    SDL_PumpEvents();

    keyboard = SDL_GetKeyboardState(&size);

    //keyboard[SDLK_RETURN] = 0;

    int screen_width = 800;
    int screen_height = 600;

    Uint32 windowFlags = SDL_WINDOW_OPENGL;

#ifdef RUN_FULLSCREEN
    windowFlags |= SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI;
#endif

    osystem_mouseLeft = 0;
    osystem_mouseRight = 0;

    osystem_initGL(screen_width, screen_height);

    osystemAL_init();
}

int posInStream = 0;
volatile bool deviceStatus = false;



