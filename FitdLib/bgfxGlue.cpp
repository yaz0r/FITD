#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bx/platform.h>
#include <backends/imgui_impl_sdl.h>
#include "imguiBGFX.h"

SDL_Window* gWindowBGFX = nullptr;

int gFrameLimit = 60;
bool gCloseApp = false;

float gVolume = 1.f;

// Because SDL is not well defined when using cmake (it's using the old style config.h and is somewhat broken)
extern "C" {
    size_t
        wcslcpy(SDL_OUT_Z_CAP(maxlen) wchar_t* dst, const wchar_t* src, size_t maxlen)
    {
        size_t srclen = SDL_wcslen(src);
        if (maxlen > 0) {
            size_t len = SDL_min(srclen, maxlen - 1);
            SDL_memcpy(dst, src, len * sizeof(wchar_t));
            dst[len] = '\0';
        }
        return srclen;
    }

    size_t
        wcslcat(SDL_INOUT_Z_CAP(maxlen) wchar_t* dst, const wchar_t* src, size_t maxlen)
    {
        size_t dstlen = SDL_wcslen(dst);
        size_t srclen = SDL_wcslen(src);
        if (dstlen < maxlen) {
            SDL_wcslcpy(dst + dstlen, src, maxlen - dstlen);
        }
        return dstlen + srclen;
    }
}

int outputResolution[2] = { -1, -1 };

void StartFrame()
{
    int oldResolution[2];
    oldResolution[0] = outputResolution[0];
    oldResolution[1] = outputResolution[1];

    SDL_GetWindowSize(gWindowBGFX, &outputResolution[0], &outputResolution[1]);

    if ((oldResolution[0] != outputResolution[0]) || (oldResolution[1] != outputResolution[1]))
    {
        bgfx::reset(outputResolution[0], outputResolution[1]);
    }

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

    // Pull the input from SDL2 instead
    ImGui_ImplSDL2_NewFrame(gWindowBGFX);
    imguiBeginFrame(0, 0, 0, 0, outputResolution[0], outputResolution[1], -1);

    if (ImGui::BeginMainMenuBar())
    {
        ImGui::Text(" %.2f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("Volume", &gVolume, 0, 1);
        ImGui::PopItemWidth();

        ImGui::EndMainMenuBar();
    }

    bgfx::setViewRect(0, 0, 0, outputResolution[0], outputResolution[1]);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);
    bgfx::touch(0);
}

void EndFrame()
{
    imguiEndFrame();

    bgfx::frame();

    {
        static Uint64 last_time = SDL_GetPerformanceCounter();
        Uint64 now = SDL_GetPerformanceCounter();

        double freq = (double)SDL_GetPerformanceFrequency();
        double secs = (now - last_time) / freq;
        double timeToWait = ((1.f / gFrameLimit) - secs) * 1000;
        //timeToWait = 0;
        if (timeToWait > 0)
        {
            SDL_Delay((unsigned int)timeToWait);
        }

        last_time = SDL_GetPerformanceCounter();
    }
}

int initBgfxGlue(int argc, char* argv[])
{
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

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(gWindowBGFX, &wmi)) {
        return false;
    }

    bgfx::Init initparam;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    initparam.platformData.ndt = wmi.info.x11.display;
    initparam.platformData.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
    initparam.platformData.ndt = NULL;
    initparam.platformData.nwh = cbSetupMetalLayer(wmi.info.cocoa.window);
#elif BX_PLATFORM_WINDOWS
    initparam.platformData.ndt = NULL;
    initparam.platformData.nwh = wmi.info.win.window;
#elif BX_PLATFORM_STEAMLINK
    initparam.platformData.ndt = wmi.info.vivante.display;
    initparam.platformData.nwh = wmi.info.vivante.window;
#endif // BX_PLATFORM_

    initparam.type = bgfx::RendererType::OpenGL;
    //initparam.type = bgfx::RendererType::Vulkan;
    bgfx::init(initparam);

    imguiCreate();

    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplSDL2_InitForD3D(gWindowBGFX);

    return true;
}

void deleteBgfxGlue()
{
    imguiDestroy();
}
