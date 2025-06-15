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

#include "osystem.h"
#include <bgfx/bgfx.h>
#include <bx/platform.h>
#include "shaders/embeddedShaders.h"
#include "imguiBGFX.h"
#include <array>
#include <string>

unsigned int gameViewId = 1;
bgfx::TextureHandle g_backgroundTexture = BGFX_INVALID_HANDLE;
bgfx::TextureHandle g_paletteTexture = BGFX_INVALID_HANDLE;
extern int outputResolution[2];
extern bool debuggerVar_debugMenuDisplayed;

#define _USE_MATH_DEFINES
#include <math.h>

// globals
char backBuffer[512 * 256 * 3];

unsigned int ditherTexture = 0;

unsigned int    debugFontTexture = 0;

struct maskStruct
{
    bgfx::TextureHandle maskTexture = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
    int maskX1 = 0;
    int maskY1 = 0;
    int maskX2 = 0;
    int maskY2 = 0;
};

std::vector<std::vector<maskStruct>> maskTextures; // [room][mask]

//vertex buffers for rendering
struct polyVertex
{
    float X;
    float Y;
    float Z;

    float U;
    float V;
};

struct sphereVertex
{
    float X;
    float Y;
    float Z;

    float U;
    float V;

    float centerX;
    float centerY;
    float size;
    float material;
};

#define NUM_MAX_FLAT_VERTICES 5000*3
#define NUM_MAX_NOISE_VERTICES 2000*3
#define NUM_MAX_TRANSPARENT_VERTICES 1000*2
#define NUM_MAX_RAMP_VERTICES 3000*3
#define NUM_MAX_SPHERES_VERTICES 3000

std::array<polyVertex, NUM_MAX_FLAT_VERTICES> flatVertices;
std::array<polyVertex, NUM_MAX_NOISE_VERTICES> noiseVertices;
std::array<polyVertex, NUM_MAX_TRANSPARENT_VERTICES> transparentVertices;
std::array<polyVertex, NUM_MAX_RAMP_VERTICES> rampVertices;
std::array<sphereVertex, NUM_MAX_SPHERES_VERTICES> sphereVertices;
std::vector<polyVertex> g_lineVertices;

int numUsedFlatVertices = 0;
int numUsedNoiseVertices = 0;
int numUsedTransparentVertices = 0;
int numUsedRampVertices = 0;
int numUsedSpheres = 0;

//static unsigned long int zoom = 0;

float nearVal = 100;
float farVal = 100000;
float cameraZoom = 0;
float fov = 0;

char RGB_Pal[256 * 4];

unsigned int    backTexture;

int g_screenWidth = 0;
int g_screenHeight = 0;

void osystem_preinigGL()
{
}

void osystem_initGL(int screenWidth, int screenHeight)
{
#if 0
#ifndef __APPLE__
    gl3wInit();
#endif

    g_screenWidth = screenWidth;
    g_screenHeight = screenHeight;

    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_CULL_FACE);

    checkGL();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    checkGL();

    // glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
    glDepthFunc(GL_LESS);

    glViewport(0, 0, g_screenWidth, g_screenHeight);
    checkGL();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);       // Black Background
    checkGL();

    // generate textures
    {
        int i;
        int j;

        unsigned char ditherMap[256 * 256 * 4];

        unsigned char* tempPtr = ditherMap;

        for (i = 0; i < 256; i++)
        {
            for (j = 0; j < 256; j++)
            {
                unsigned char ditherValue = rand() % 0x50;

                *(tempPtr++) = ditherValue;
                *(tempPtr++) = ditherValue;
                *(tempPtr++) = ditherValue;
                *(tempPtr++) = 255;
            }
        }

        //glBlendFunc(GL_SRC_ALPHA,GL_ONE);

        glGenTextures(1, &ditherTexture);
        glBindTexture(GL_TEXTURE_2D, ditherTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, ditherMap);
        checkGL();

        glBindTexture(GL_TEXTURE_2D, 0);
        checkGL();

        //glEnable(GL_TEXTURE_2D);
        //checkGL();
    }

    checkGL();

    osystem_initBuffer();

    checkGL();

    GLuint vao;

    glGenVertexArrays(1, &vao); checkGL();
    glBindVertexArray(vao); checkGL();
#endif
}

void osystem_setPalette(u8* palette)
{
    memcpy(RGB_Pal, palette, 256 * 3);

    bgfx::updateTexture2D(g_paletteTexture, 0, 0, 0, 0, 3, 256, bgfx::copy(RGB_Pal, 256 * 3));
}

void osystem_setPalette(palette_t* palette)
{
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 3; j++) {
            RGB_Pal[i * 3 + j] = palette->at(i)[j];
        }
    }

    bgfx::updateTexture2D(g_paletteTexture, 0, 0, 0, 0, 3, 256, bgfx::copy(RGB_Pal, 256 * 3));
}

struct s_vertexData
{
    float positions[3];
    float textures[2];
    float color[4];
};
s_vertexData gVertexArray[1024 * 1024];

bgfx::ShaderHandle loadBgfxShader(const std::string& filename)
{
    std::vector<u8> memBlob;
    FILE* fHandle = fopen(filename.c_str(), "rb");
    if (fHandle == nullptr)
        return BGFX_INVALID_HANDLE;
    fseek(fHandle, 0, SEEK_END);
    u32 size = ftell(fHandle);
    fseek(fHandle, 0, SEEK_SET);
    memBlob.resize(size);
    fread(&memBlob[0], 1, size, fHandle);
    fclose(fHandle);

    bgfx::ShaderHandle handle = bgfx::createShader(bgfx::copy(&memBlob[0], size));
    bgfx::setName(handle, filename.c_str());

    return handle;
}

bgfx::ProgramHandle getBackgroundShader()
{
    static bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(programHandle))
    {
        programHandle = loadBgfxProgram("background_vs", "background_ps");
    }

    return programHandle;
}

bgfx::ProgramHandle getMaskBackgroundShader()
{
    static bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(programHandle))
    {
        programHandle = loadBgfxProgram("maskBackground_vs", "maskBackground_ps");
    }

    return programHandle;
}

bgfx::ProgramHandle getFlatShader()
{
    static bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(programHandle))
    {
        programHandle = loadBgfxProgram("flat_vs", "flat_ps");
    }

    return programHandle;
}

bgfx::ProgramHandle getNoiseShader()
{
    static bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(programHandle))
    {
        programHandle = loadBgfxProgram("noise_vs", "noise_ps");
    }

    return programHandle;
}

bgfx::ProgramHandle getRampShader()
{
    static bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(programHandle))
    {
        programHandle = loadBgfxProgram("ramp_vs", "ramp_ps");
    }

    return programHandle;
}

bgfx::ProgramHandle getSphereShader()
{
    static bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(programHandle))
    {
        programHandle = loadBgfxProgram("sphere_vs", "sphere_ps");
    }

    return programHandle;
}

void osystem_drawBackground()
{
    if (backgroundMode == backgroundModeEnum_2D)
    {
        bgfx::VertexLayout layout;
        layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();

        bgfx::TransientVertexBuffer transientBuffer;
        bgfx::allocTransientVertexBuffer(&transientBuffer, 6, layout);

        struct sVertice
        {
            float position[3];
            float texcoord[2];
        };

        sVertice* pVertices = (sVertice*)transientBuffer.data;

        float quadVertices[6 * 3];
        float quadUV[6 * 2];

        // 0
        pVertices->position[0] = 0.f;
        pVertices->position[1] = 0.f;
        pVertices->position[2] = 1000.f;
        pVertices->texcoord[0] = 0.f;
        pVertices->texcoord[1] = 0.f;
        pVertices++;

        //2
        pVertices->position[0] = 320.f;
        pVertices->position[1] = 200.f;
        pVertices->position[2] = 1000.f;
        pVertices->texcoord[0] = 1.f;
        pVertices->texcoord[1] = 1.f;
        pVertices++;

        //1
        pVertices->position[0] = 320.f;
        pVertices->position[1] = 0.f;
        pVertices->position[2] = 1000.f;
        pVertices->texcoord[0] = 1.f;
        pVertices->texcoord[1] = 0.f;
        pVertices++;

        //------------------------
        //3
        pVertices->position[0] = 0.f;
        pVertices->position[1] = 0.f;
        pVertices->position[2] = 1000.f;
        pVertices->texcoord[0] = 0.f;
        pVertices->texcoord[1] = 0.f;
        pVertices++;

        //4
        pVertices->position[0] = 0.f;
        pVertices->position[1] = 200.f;
        pVertices->position[2] = 1000.f;
        pVertices->texcoord[0] = 0.f;
        pVertices->texcoord[1] = 1.f;
        pVertices++;

        //5
        pVertices->position[0] = 320.f;
        pVertices->position[1] = 200.f;
        pVertices->position[2] = 1000.f;
        pVertices->texcoord[0] = 1.f;
        pVertices->texcoord[1] = 1.f;
        pVertices++;

        static bgfx::UniformHandle backgroundTextureUniform = BGFX_INVALID_HANDLE;
        if (!bgfx::isValid(backgroundTextureUniform))
        {
            backgroundTextureUniform = bgfx::createUniform("s_backgroundTexture", bgfx::UniformType::Sampler);
        }
        static bgfx::UniformHandle paletteTextureUniform = BGFX_INVALID_HANDLE;
        if (!bgfx::isValid(paletteTextureUniform))
        {
            paletteTextureUniform = bgfx::createUniform("s_paletteTexture", bgfx::UniformType::Sampler);
        }


        bgfx::setState(0 | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_MSAA
        );

        bgfx::setVertexBuffer(0, &transientBuffer);

        bgfx::setTexture(0, backgroundTextureUniform, g_backgroundTexture);
        bgfx::setTexture(1, paletteTextureUniform, g_paletteTexture);
        bgfx::submit(gameViewId, getBackgroundShader());


    }
}

bool g_bgfxMainResourcesInitialized = false;

bgfx::FrameBufferHandle fieldModelInspector_FB = BGFX_INVALID_HANDLE;
bgfx::TextureHandle fieldModelInspector_Texture = BGFX_INVALID_HANDLE;
bgfx::TextureHandle fieldModelInspector_Depth = BGFX_INVALID_HANDLE;

void initBgfxMainResources()
{
    // create background texture
    g_backgroundTexture = bgfx::createTexture2D(320, 200, false, 1, bgfx::TextureFormat::R8U);
    g_paletteTexture = bgfx::createTexture2D(3, 256, false, 1, bgfx::TextureFormat::R8U);
}

ImVec2 gameResolution = { 320, 200 };

void renderGameWindow()
{
    if (ImGui::Begin("Game"))
    {
        if (bgfx::getCaps()->originBottomLeft)
        {
            ImGui::Image(fieldModelInspector_Texture, gameResolution, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        }
        else
        {
            ImGui::Image(fieldModelInspector_Texture, gameResolution, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        }
    }
    ImGui::End();
}

void osystem_startFrame()
{
    if (!g_bgfxMainResourcesInitialized)
    {
        initBgfxMainResources();
        g_bgfxMainResourcesInitialized = true;
    }

    static ImVec2 oldWindowSize = { -1,-1 };

    if (debuggerVar_debugMenuDisplayed)
    {
        gameViewId = 1;
        if (ImGui::Begin("Game"))
        {
            ImVec2 currentWindowSize = ImGui::GetContentRegionAvail();

            currentWindowSize[0] = std::max<int>(currentWindowSize[0], 1);
            currentWindowSize[1] = std::max<int>(currentWindowSize[1], 1);

            gameResolution = currentWindowSize;
        }
        else
        {
            gameResolution = { 320, 200 };
        }
        ImGui::End();

        if ((gameResolution[0] != oldWindowSize[0]) || (gameResolution[1] != oldWindowSize[1]))
        {
            oldWindowSize = gameResolution;

            if (bgfx::isValid(fieldModelInspector_FB))
            {
                bgfx::destroy(fieldModelInspector_FB);
            }

            const uint64_t tsFlags = 0
                //| BGFX_SAMPLER_MIN_POINT
                //| BGFX_SAMPLER_MAG_POINT
                //| BGFX_SAMPLER_MIP_POINT
                | BGFX_SAMPLER_U_CLAMP
                | BGFX_SAMPLER_V_CLAMP
                ;

            fieldModelInspector_Texture = bgfx::createTexture2D(gameResolution[0], gameResolution[1], false, 0, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | tsFlags);
            fieldModelInspector_Depth = bgfx::createTexture2D(gameResolution[0], gameResolution[1], false, 0, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT | tsFlags);
            std::array<bgfx::Attachment, 2> attachements;
            attachements[0].init(fieldModelInspector_Texture);
            attachements[1].init(fieldModelInspector_Depth);
            fieldModelInspector_FB = bgfx::createFrameBuffer(2, &attachements[0], true);
        }
        bgfx::setViewFrameBuffer(gameViewId, fieldModelInspector_FB);
        bgfx::setViewRect(gameViewId, 0, 0, gameResolution[0], gameResolution[1]);
    }
    else
    {
        gameViewId = 0;
        gameResolution[0] = outputResolution[0];
        gameResolution[1] = outputResolution[1];
        bgfx::setViewFrameBuffer(gameViewId, BGFX_INVALID_HANDLE); //bind the backbuffer
    }

    {
        bgfx::setViewClear(gameViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 255);

        bgfx::setViewName(gameViewId, "Game");
        bgfx::setViewMode(gameViewId, bgfx::ViewMode::Sequential);

        bgfx::touch(gameViewId);
    }

    osystem_drawBackground();
}

unsigned char frontBuffer[320 * 200];
unsigned char physicalScreen[320 * 200];

void osystem_CopyBlockPhys(unsigned char* videoBuffer, int left, int top, int right, int bottom)
{
    unsigned char* in = (unsigned char*)&videoBuffer[0] + left + top * 320;

    while ((right - left) % 4)
    {
        right++;
    }

    while ((bottom - top) % 4)
    {
        bottom++;
    }

    for (int i = top; i < bottom; i++)
    {
        in = (unsigned char*)&videoBuffer[0] + left + i * 320;
        unsigned char* out2 = physicalScreen + left + i * 320;
        for (int j = left; j < right; j++)
        {
			*(out2++) = *(in++);
        }
    }

    bgfx::updateTexture2D(g_backgroundTexture, 0, 0, 0, 0, 320, 200, bgfx::copy(physicalScreen, 320 * 200));
}

void osystem_refreshFrontTextureBuffer()
{
}

void osystem_initBuffer()
{
    memset(backBuffer, 0x0, 512 * 256 * 3);
}

void gameScreenToViewport(float* X, float* Y)
{
    (*X) = (*X) * g_screenWidth / 320.f;
    (*Y) = (*Y) * g_screenHeight / 200.f;

    (*Y) = g_screenHeight - (*Y);
}

void osystem_setClip(float left, float top, float right, float bottom)
{
    float x1 = left - 1;
    float y1 = bottom + 1;
    float x2 = right + 1;
    float y2 = top - 1;

    gameScreenToViewport(&x1, &y1);
    gameScreenToViewport(&x2, &y2);

    float width = x2 - x1;
    float height = y2 - y1;

    float currentScissor[4];
    currentScissor[0] = ((left - 1) / 320.f) * gameResolution[0];
    currentScissor[1] = ((top - 1) / 200.f) * gameResolution[1];
    currentScissor[2] = ((right - left + 2) / 320.f) * gameResolution[0];
    currentScissor[3] = ((bottom - top + 2) / 200.f) * gameResolution[1];

    currentScissor[0] = std::max<float>(currentScissor[0], 0);
    currentScissor[1] = std::max<float>(currentScissor[1], 0);

    bgfx::setScissor(currentScissor[0], currentScissor[1], currentScissor[2], currentScissor[3]);
}

void osystem_clearClip()
{
    bgfx::setScissor(0, 0, gameResolution[0], gameResolution[1]);
}

void osystem_stopFrame()
{
}

void osystem_startModelRender()
{

}

void osystem_stopModelRender()
{
    osystem_flushPendingPrimitives();
}

void osystem_flushPendingPrimitives()
{
    if (numUsedFlatVertices)
    {
        bgfx::VertexLayout layout;
        layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();

        bgfx::TransientVertexBuffer transientBuffer;
        bgfx::allocTransientVertexBuffer(&transientBuffer, numUsedFlatVertices, layout);

        memcpy(transientBuffer.data, &flatVertices[0], sizeof(polyVertex) * numUsedFlatVertices);

        bgfx::setState(0 | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LEQUAL
            | BGFX_STATE_MSAA
        );

        static bgfx::UniformHandle paletteTextureUniform = BGFX_INVALID_HANDLE;
        if (!bgfx::isValid(paletteTextureUniform))
        {
            paletteTextureUniform = bgfx::createUniform("s_paletteTexture", bgfx::UniformType::Sampler);
        }

        bgfx::setTexture(1, paletteTextureUniform, g_paletteTexture);

        bgfx::setVertexBuffer(0, &transientBuffer);
        bgfx::submit(gameViewId, getFlatShader());
    }

    if (numUsedNoiseVertices)
    {
        bgfx::VertexLayout layout;
        layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();

        bgfx::TransientVertexBuffer transientBuffer;
        bgfx::allocTransientVertexBuffer(&transientBuffer, numUsedNoiseVertices, layout);

        memcpy(transientBuffer.data, &noiseVertices[0], sizeof(polyVertex) * numUsedNoiseVertices);

        bgfx::setState(0 | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LEQUAL
            | BGFX_STATE_MSAA
        );

        static bgfx::UniformHandle paletteTextureUniform = BGFX_INVALID_HANDLE;
        if (!bgfx::isValid(paletteTextureUniform))
        {
            paletteTextureUniform = bgfx::createUniform("s_paletteTexture", bgfx::UniformType::Sampler);
        }

        bgfx::setTexture(1, paletteTextureUniform, g_paletteTexture);

        bgfx::setVertexBuffer(0, &transientBuffer);
        bgfx::submit(gameViewId, getNoiseShader());
    }

    if (numUsedRampVertices)
    {
        bgfx::VertexLayout layout;
        layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();

        bgfx::TransientVertexBuffer transientBuffer;
        bgfx::allocTransientVertexBuffer(&transientBuffer, numUsedRampVertices, layout);

        memcpy(transientBuffer.data, &rampVertices[0], sizeof(polyVertex) * numUsedRampVertices);

        bgfx::setState(0 | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LEQUAL
            | BGFX_STATE_MSAA
        );

        static bgfx::UniformHandle paletteTextureUniform = BGFX_INVALID_HANDLE;
        if (!bgfx::isValid(paletteTextureUniform))
        {
            paletteTextureUniform = bgfx::createUniform("s_paletteTexture", bgfx::UniformType::Sampler);
        }

        bgfx::setTexture(1, paletteTextureUniform, g_paletteTexture);
        bgfx::setVertexBuffer(0, &transientBuffer);
        bgfx::submit(gameViewId, getRampShader());
    }

    if(numUsedSpheres)
    {
        bgfx::VertexLayout layout;
        layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord1, 4, bgfx::AttribType::Float)
            .end();

        bgfx::TransientVertexBuffer transientBuffer;
        bgfx::allocTransientVertexBuffer(&transientBuffer, numUsedSpheres, layout);

        memcpy(transientBuffer.data, &sphereVertices[0], sizeof(sphereVertex)* numUsedSpheres);

        bgfx::setState(0 | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LEQUAL
            | BGFX_STATE_MSAA
        );

        static bgfx::UniformHandle paletteTextureUniform = BGFX_INVALID_HANDLE;
        if (!bgfx::isValid(paletteTextureUniform))
        {
            paletteTextureUniform = bgfx::createUniform("s_paletteTexture", bgfx::UniformType::Sampler);
        }

        bgfx::setTexture(1, paletteTextureUniform, g_paletteTexture);
        bgfx::setVertexBuffer(0, &transientBuffer);
        bgfx::submit(gameViewId, getSphereShader());
    }

    if (g_lineVertices.size()) {
        bgfx::VertexLayout layout;
        layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();

        bgfx::TransientVertexBuffer transientBuffer;
        bgfx::allocTransientVertexBuffer(&transientBuffer, g_lineVertices.size(), layout);

        memcpy(transientBuffer.data, g_lineVertices.data(), sizeof(polyVertex) * g_lineVertices.size());

        bgfx::setState(0 | BGFX_STATE_PT_LINES
            | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LEQUAL
            | BGFX_STATE_MSAA
        );

        static bgfx::UniformHandle paletteTextureUniform = BGFX_INVALID_HANDLE;
        if (!bgfx::isValid(paletteTextureUniform))
        {
            paletteTextureUniform = bgfx::createUniform("s_paletteTexture", bgfx::UniformType::Sampler);
        }

        bgfx::setTexture(1, paletteTextureUniform, g_paletteTexture);

        bgfx::setVertexBuffer(0, &transientBuffer);
        bgfx::submit(gameViewId, getFlatShader());
    }
#if 0
    if (numUsedTransparentVertices)
    {
        /*
        glEnable(GL_BLEND); checkGL();
        glVertexPointer(3, GL_FLOAT, sizeof(polyVertex), &transparentVertices->X); checkGL();
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(polyVertex), &transparentVertices->R); checkGL();
        glDrawArrays(GL_TRIANGLES, 0, numUsedTransparentVertices); checkGL();
        glDisable(GL_BLEND); checkGL();
        */
    }
#endif
    numUsedFlatVertices = 0;
    numUsedNoiseVertices = 0;
    numUsedRampVertices = 0;
    numUsedSpheres = 0;
    numUsedTransparentVertices = 0;
    g_lineVertices.clear();
}

void osystem_fillPoly(float* buffer, int numPoint, unsigned char color, u8 polyType)
{
#define MAX_POINTS_PER_POLY 50
    float UVArray[MAX_POINTS_PER_POLY];

    assert(numPoint < MAX_POINTS_PER_POLY);

    // compute the polygon bounding box
    float polyMinX = 320.f;
    float polyMaxX = 0.f;
    float polyMinY = 200.f;
    float polyMaxY = 0.f;

    for (int i = 0; i < numPoint; i++)
    {
        float X = buffer[3 * i + 0];
        float Y = buffer[3 * i + 1];

        if (X > polyMaxX)
            polyMaxX = X;
        if (X < polyMinX)
            polyMinX = X;

        if (Y > polyMaxY)
            polyMaxY = Y;
        if (Y < polyMinY)
            polyMinY = Y;
    }

    float polyWidth = polyMaxX - polyMinX;
    float polyHeight = polyMaxY - polyMinY;

    if (polyWidth <= 0.f)
        polyWidth = 1;
    if (polyHeight <= 0.f)
        polyHeight = 1;

    switch (polyType)
    {
    default:
    case 0: // flat (triste)
    {
        polyVertex* pVertex = &flatVertices[numUsedFlatVertices];
        numUsedFlatVertices += (numPoint - 2) * 3;
        assert(numUsedFlatVertices < NUM_MAX_FLAT_VERTICES);

        for (int i = 0; i < numPoint; i++)
        {
            if (i >= 3)
            {
                memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
                pVertex++;
                memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
                pVertex++;
            }

            pVertex->X = buffer[i * 3 + 0];
            pVertex->Y = buffer[i * 3 + 1];
            pVertex->Z = buffer[i * 3 + 2];

            int bank = (color & 0xF0) >> 4;
            int startColor = color & 0xF;
            float colorf = startColor;
            pVertex->U = colorf / 15.f;
            pVertex->V = bank / 15.f;
            pVertex++;
        }
        break;
    }
    case 1: // dither (pierre/tele)
    {
        polyVertex* pVertex = &noiseVertices[numUsedNoiseVertices];
        numUsedNoiseVertices += (numPoint - 2) * 3;
        assert(numUsedNoiseVertices < NUM_MAX_NOISE_VERTICES);

        for (int i = 0; i < numPoint; i++)
        {
            if (i >= 3)
            {
                memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
                pVertex++;
                memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
                pVertex++;
            }

            pVertex->X = buffer[i * 3 + 0];
            pVertex->Y = buffer[i * 3 + 1];
            pVertex->Z = buffer[i * 3 + 2];

            pVertex->U = (pVertex->X / 320.f) * 50.f + polyMinX * 1.2f + polyMaxX;
            pVertex->V = (pVertex->Y / 200.f) * 50.f + polyMinY * 0.7f + polyMaxY;

            int bank = (color & 0xF0) >> 4;
            int startColor = color & 0xF;
            float colorf = startColor;
            pVertex->U = colorf / 15.f;
            pVertex->V = bank / 15.f;
            pVertex++;
        }
        break;
    }
    case 2: // trans
    {
        polyVertex* pVertex = &transparentVertices[numUsedTransparentVertices];
        numUsedTransparentVertices += (numPoint - 2) * 3;
        assert(numUsedTransparentVertices < NUM_MAX_TRANSPARENT_VERTICES);

        for (int i = 0; i < numPoint; i++)
        {
            if (i >= 3)
            {
                memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
                pVertex++;
                memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
                pVertex++;
            }

            pVertex->X = buffer[i * 3 + 0];
            pVertex->Y = buffer[i * 3 + 1];
            pVertex->Z = buffer[i * 3 + 2];

            int bank = (color & 0xF0) >> 4;
            int startColor = color & 0xF;
            float colorf = startColor;
            pVertex->U = colorf / 15.f;
            pVertex->V = bank / 15.f;
            pVertex++;
        }
        break;
    }
    case 4: // copper (ramps top to bottom)
    case 5: // copper2 (ramps top to bottom, 2 scanline per color)
    {
        polyVertex* pVertex = &rampVertices[numUsedRampVertices];
        numUsedRampVertices += (numPoint - 2) * 3;
        assert(numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

        int bank = (color & 0xF0) >> 4;
        int startColor = color & 0xF;
        float colorStep = 1; // TODO: this should be the scanline ratio for the current resolution to original resolution
        if (polyType == 5)
        {
            colorStep *= 0.5; // to stretch the ramp by 2 for copper2
        }

        for (int i = 0; i < numPoint; i++)
        {
            if (i >= 3)
            {
                memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
                pVertex++;
                memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
                pVertex++;
            }

            pVertex->X = buffer[i * 3 + 0];
            pVertex->Y = buffer[i * 3 + 1];
            pVertex->Z = buffer[i * 3 + 2];

            float colorf = startColor + colorStep * (pVertex->Y - polyMinY);

            pVertex->U = colorf / 15.f;
            pVertex->V = bank / 15.f;
            pVertex++;
        }
        break;
    }
    case 3: // marbre (ramp left to right)
    {
        polyVertex* pVertex = &rampVertices[numUsedRampVertices];
        numUsedRampVertices += (numPoint - 2) * 3;
        assert(numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

        float colorStep = 15.f / polyWidth;

        int bank = (color & 0xF0) >> 4;
        int startColor = color & 0xF;

        assert(startColor == 0);

        for (int i = 0; i < numPoint; i++)
        {
            if (i >= 3)
            {
                memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
                pVertex++;
                memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
                pVertex++;
            }

            pVertex->X = buffer[i * 3 + 0];
            pVertex->Y = buffer[i * 3 + 1];
            pVertex->Z = buffer[i * 3 + 2];

            float colorf = startColor + colorStep * (pVertex->X - polyMinX);

            pVertex->U = colorf / 15.f;
            pVertex->V = bank / 15.f;
            pVertex++;
        }
        break;
    }
    case 6: // marbre2 (ramp right to left)
    {
        polyVertex* pVertex = &rampVertices[numUsedRampVertices];
        numUsedRampVertices += (numPoint - 2) * 3;
        assert(numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

        float colorStep = 15.f / polyWidth;

        int bank = (color & 0xF0) >> 4;
        int startColor = color & 0xF;

        assert(startColor == 0);

        for (int i = 0; i < numPoint; i++)
        {
            if (i >= 3)
            {
                memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
                pVertex++;
                memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
                pVertex++;
            }

            pVertex->X = buffer[i * 3 + 0];
            pVertex->Y = buffer[i * 3 + 1];
            pVertex->Z = buffer[i * 3 + 2];

            float colorf = startColor + colorStep * (pVertex->X - polyMinX);

            pVertex->U = 1.f - colorf / 15.f;
            pVertex->V = bank / 15.f;
            pVertex++;
        }
        break;
    }
    }
}

void osystem_draw3dLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned char color)
{
    polyVertex vertex1;
    polyVertex vertex2;

    vertex1.X = x1;
    vertex1.Y = y1;
    vertex1.Z = z1;

    vertex2.X = x2;
    vertex2.Y = y2;
    vertex2.Z = z2;

    int bank = (color & 0xF0) >> 4;
    int startColor = color & 0xF;
    float colorf = startColor;
    vertex1.U = vertex2.U = colorf / 15.f;
    vertex1.V = vertex2.V = bank / 15.f;

    g_lineVertices.push_back(vertex1);
    g_lineVertices.push_back(vertex2);

#if 0
    GLfloat lineVertices[2 * 3];
    GLubyte lineColor[2 * 4];

    for (int i = 0; i < 2; i++)
    {
        lineColor[4 * i + 0] = RGB_Pal[color * 3 + 0];
        lineColor[4 * i + 1] = RGB_Pal[color * 3 + 1];
        lineColor[4 * i + 2] = RGB_Pal[color * 3 + 2];
        lineColor[4 * i + 3] = 255;
    }
    lineVertices[0] = x1;
    lineVertices[1] = y1;
    lineVertices[2] = z1;
    lineVertices[3] = x2;
    lineVertices[4] = y2;
    lineVertices[5] = z2;

    static GLuint shaderprogram = 0;
    static GLuint vertexp = 0;
    static GLuint colorp = 0;
    if (shaderprogram == 0)
    {
        shaderprogram = compileShader(flatQuadVS, flatQuadPS);
        vertexp = glGetAttribLocation(shaderprogram, (const GLchar*)"in_Position");
        colorp = glGetAttribLocation(shaderprogram, (const GLchar*)"in_Color");
    }

    glUseProgram(shaderprogram);

    static GLuint vbo = 0;
    if (vbo == 0)
    {
        glGenBuffers(1, &vbo);
    }

    for (int i = 0; i < 2; i++)
    {
        gVertexArray[i].positions[0] = lineVertices[i * 3 + 0];
        gVertexArray[i].positions[1] = lineVertices[i * 3 + 1];
        gVertexArray[i].positions[2] = lineVertices[i * 3 + 2];

        gVertexArray[i].color[0] = lineColor[i * 4 + 0] / 255.f;
        gVertexArray[i].color[1] = lineColor[i * 4 + 1] / 255.f;
        gVertexArray[i].color[2] = lineColor[i * 4 + 2] / 255.f;
        gVertexArray[i].color[3] = lineColor[i * 4 + 3] / 255.f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo); checkGL();
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertexData) * 2, &gVertexArray[0], GL_STATIC_DRAW); checkGL();
    glVertexAttribPointer(vertexp, 3, GL_FLOAT, GL_FALSE, sizeof(s_vertexData), (void*)&((s_vertexData*)NULL)->positions); checkGL();
    glEnableVertexAttribArray(vertexp); checkGL();

    if (colorp != -1)
    {
        glVertexAttribPointer(colorp, 4, GL_FLOAT, GL_FALSE, sizeof(s_vertexData), (void*)&((s_vertexData*)NULL)->color);
        glEnableVertexAttribArray(colorp);
    }

    glDrawArrays(GL_LINE_LOOP, 0, 2); checkGL();

    checkGL();
#endif
}

void osystem_draw3dQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, unsigned char color, int transparency)
{
    osystem_draw3dLine(x1, y1, z1, x2, y2, z2, color);
    osystem_draw3dLine(x2, y2, z2, x3, y3, z3, color);
    osystem_draw3dLine(x3, y3, z3, x4, y4, z4, color);
    osystem_draw3dLine(x4, y4, z4, x1, y1, z1, color);
}

void osystem_drawSphere(float X, float Y, float Z, u8 color, u8 material, float size)
{
    osystem_drawPoint(X, Y, Z, color, material, size);
}

void osystem_drawPoint(float X, float Y, float Z, u8 color, u8 material, float size)
{
    std::array<sphereVertex, 4> corners;
    corners[0].X = X + size;
    corners[0].Y = Y + size;
    corners[0].Z = Z;

    corners[1].X = X + size;
    corners[1].Y = Y - size;
    corners[1].Z = Z;

    corners[2].X = X - size;
    corners[2].Y = Y - size;
    corners[2].Z = Z;

    corners[3].X = X - size;
    corners[3].Y = Y + size;
    corners[3].Z = Z;

    std::array<int, 2 * 3> mapping = { {
            0,1,2,
            0,2,3
    } };

    for(int i=0; i< mapping.size(); i++)
    {
        sphereVertex* pVertex = &sphereVertices[numUsedSpheres];
        numUsedSpheres++;
        assert(numUsedSpheres < NUM_MAX_SPHERES_VERTICES);

        pVertex->X = corners[mapping[i]].X;
        pVertex->Y = corners[mapping[i]].Y;
        pVertex->Z = corners[mapping[i]].Z;
        pVertex->U = (color & 0xF) / 15.f;
        pVertex->V = ((color & 0xF0) >> 4) / 15.f;
        pVertex->size = size;
        pVertex->centerX = X;
        pVertex->centerY = Y;
        pVertex->material = material;
    }
}

void osystem_flip(unsigned char* videoBuffer)
{
    osystem_flushPendingPrimitives();
}

void osystem_createMask(const std::array<u8, 320 * 200>& mask, int roomId, int maskId, int maskX1, int maskY1, int maskX2, int maskY2)
{
    if (maskTextures.size() < roomId + 1)
    {
        maskTextures.resize(roomId + 1);
    }
    if (maskTextures[roomId].size() < maskId + 1)
    {
        maskTextures[roomId].resize(maskId + 1);
    }

    if (bgfx::isValid(maskTextures[roomId][maskId].maskTexture))
    {
        bgfx::destroy(maskTextures[roomId][maskId].maskTexture);
        maskTextures[roomId][maskId].maskTexture = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(maskTextures[roomId][maskId].vertexBuffer))
    {
        bgfx::destroy(maskTextures[roomId][maskId].vertexBuffer);
        maskTextures[roomId][maskId].vertexBuffer = BGFX_INVALID_HANDLE;
    }

    maskTextures[roomId][maskId].maskTexture = bgfx::createTexture2D(320, 200, false, 1, bgfx::TextureFormat::R8U, 0, bgfx::copy(mask.data(), 320 * 200));
    maskTextures[roomId][maskId].maskX1 = maskX1;
    maskTextures[roomId][maskId].maskX2 = maskX2 + 1;
    maskTextures[roomId][maskId].maskY1 = maskY1;
    maskTextures[roomId][maskId].maskY2 = maskY2 + 1;

    bgfx::VertexLayout layout;
    layout
        .begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    float X1 = maskTextures[roomId][maskId].maskX1;
    float X2 = maskTextures[roomId][maskId].maskX2;
    float Y1 = maskTextures[roomId][maskId].maskY1;
    float Y2 = maskTextures[roomId][maskId].maskY2;

    float maskZ = 0.f;

    struct sVertice
    {
        float position[3];
        float texcoord[2];
    } vertexBuffer[4];

    sVertice* pVertices = vertexBuffer;
    pVertices->position[0] = X1;
    pVertices->position[1] = Y2;
    pVertices->position[2] = maskZ;
    pVertices->texcoord[0] = X1 / 320.f;
    pVertices->texcoord[1] = Y2 / 200.f;
    pVertices++;
    pVertices->position[0] = X1;
    pVertices->position[1] = Y1;
    pVertices->position[2] = maskZ;
    pVertices->texcoord[0] = X1 / 320.f;
    pVertices->texcoord[1] = Y1 / 200.f;
    pVertices++;
    pVertices->position[0] = X2;
    pVertices->position[1] = Y2;
    pVertices->position[2] = maskZ;
    pVertices->texcoord[0] = X2 / 320.f;
    pVertices->texcoord[1] = Y2 / 200.f;
    pVertices++;
    pVertices->position[0] = X2;
    pVertices->position[1] = Y1;
    pVertices->position[2] = maskZ;
    pVertices->texcoord[0] = X2 / 320.f;
    pVertices->texcoord[1] = Y1 / 200.f;
    pVertices++;

    maskTextures[roomId][maskId].vertexBuffer = bgfx::createVertexBuffer(bgfx::copy(vertexBuffer, sizeof(vertexBuffer)), layout);
}

void osystem_drawMask(int roomId, int maskId)
{
    if (g_gameId == TIMEGATE)
        return;

    if (!bgfx::isValid(maskTextures[roomId][maskId].maskTexture))
        return;

    if (!bgfx::isValid(maskTextures[roomId][maskId].vertexBuffer))
        return;

#ifdef FITD_DEBUGGER
    if (backgroundMode != backgroundModeEnum_2D)
        return;
#endif

    static bgfx::UniformHandle backgroundTextureUniform = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(backgroundTextureUniform))
    {
        backgroundTextureUniform = bgfx::createUniform("s_backgroundTexture", bgfx::UniformType::Sampler);
    }
    static bgfx::UniformHandle paletteTextureUniform = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(paletteTextureUniform))
    {
        paletteTextureUniform = bgfx::createUniform("s_paletteTexture", bgfx::UniformType::Sampler);
    }
    static bgfx::UniformHandle maskTextureUniform = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(maskTextureUniform))
    {
        maskTextureUniform = bgfx::createUniform("s_maskTexture", bgfx::UniformType::Sampler);
    }

    bgfx::setState(0 | BGFX_STATE_WRITE_RGB
        | BGFX_STATE_MSAA
        | BGFX_STATE_PT_TRISTRIP
    );

    bgfx::setVertexBuffer(0, maskTextures[roomId][maskId].vertexBuffer);

    bgfx::setTexture(2, backgroundTextureUniform, g_backgroundTexture);
    bgfx::setTexture(1, paletteTextureUniform, g_paletteTexture);
    bgfx::setTexture(0, maskTextureUniform, maskTextures[roomId][maskId].maskTexture);
    bgfx::submit(gameViewId, getMaskBackgroundShader());
}
