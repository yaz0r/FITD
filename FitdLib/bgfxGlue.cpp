#include <SDL.h>
#include <bgfx/bgfx.h>
#include <bx/platform.h>
#include <backends/imgui_impl_sdl3.h>
#include "imguiBGFX.h"
#include "SDL3/SDL.h"

#if BX_PLATFORM_OSX
extern "C" {
	void* cbSetupMetalLayer(void*);
}
#elif BX_PLATFORM_WINDOWS
#include <windows.h>
#endif


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

    // Pull the input from SDL2 instead
    ImGui_ImplSDL3_NewFrame();
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

extern bool debuggerVar_debugMenuDisplayed;
void EndFrame()
{
    if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent, false))
    {
        debuggerVar_debugMenuDisplayed = !debuggerVar_debugMenuDisplayed;
    }

    if (debuggerVar_debugMenuDisplayed)
    {
        imguiEndFrame();
    }
    else
    {
        ImGui::Render();
}
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

bgfx::Init initparam;

void createBgfxInitParams()
{
#if BX_PLATFORM_LINUX
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
    {
        initparam.platformData.ndt = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(gWindowBGFX), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        initparam.platformData.nwh = (void*)SDL_GetNumberProperty(SDL_GetWindowProperties(gWindowBGFX), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    }
    else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0)
    {
        initparam.platformData.ndt = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(gWindowBGFX), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        initparam.platformData.nwh = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(gWindowBGFX), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
    }
#elif BX_PLATFORM_OSX
    initparam.platformData.ndt = NULL;
    initparam.platformData.nwh = cbSetupMetalLayer((void*)SDL_GetPointerProperty(SDL_GetWindowProperties(gWindowBGFX), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL));
#elif BX_PLATFORM_WINDOWS
    initparam.platformData.ndt = NULL;
    initparam.platformData.nwh = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(gWindowBGFX), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#endif // BX_PLATFORM_
}

int initBgfxGlue(int argc, char* argv[])
{
    createBgfxInitParams();
    //initparam.type = bgfx::RendererType::OpenGL;
    //initparam.type = bgfx::RendererType::Vulkan;
    initparam.type = bgfx::RendererType::Direct3D12;
    bgfx::init(initparam);

    imguiCreate();

    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    if ((bgfx::getRendererType() == bgfx::RendererType::Direct3D11) || (bgfx::getRendererType() == bgfx::RendererType::Direct3D12)) {
        ImGui_ImplSDL3_InitForD3D(gWindowBGFX);
    }
    else
    if (bgfx::getRendererType() == bgfx::RendererType::Metal) {
        ImGui_ImplSDL3_InitForMetal(gWindowBGFX);
    }
    else
    if (bgfx::getRendererType() == bgfx::RendererType::OpenGL) {
        ImGui_ImplSDL3_InitForOpenGL(gWindowBGFX, SDL_GL_GetCurrentContext());
    }
    else
    if (bgfx::getRendererType() == bgfx::RendererType::Vulkan) {
        ImGui_ImplSDL3_InitForVulkan(gWindowBGFX);
    }
    else {
        ImGui_ImplSDL3_InitForOther(gWindowBGFX);
    }

    return true;
}

void deleteBgfxGlue()
{
    imguiDestroy();
}
