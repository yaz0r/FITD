#pragma once

#include <SDL.h>

int initBgfxGlue(int argc, char* argv[]);
void deleteBgfxGlue();

void StartFrame();
void EndFrame();

extern int gFrameLimit;
extern bool gCloseApp;

extern SDL_Window* gWindowBGFX;

