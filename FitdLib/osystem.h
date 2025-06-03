#ifndef _OSYSTEM_H_
#define _OSYSTEM_H_

#ifdef __cplusplus
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef FITD_DEBUGGER
#include "debugFont.h"
#endif

#include "palette.h"

#endif

#define byte char
//#ifndef bool
//#define bool char
//#endif
#define u8 unsigned char
#define true 1
#define false 0

    enum e_rendererType
    {
        OPENGL_OLD,
        OPENGL_ES,
        OPENGL_3_2,
    };

    extern enum e_rendererType g_rendererType;

	void osystem_init();

	extern int osystem_mouseRight;
	extern int osystem_mouseLeft;

	void osystem_drawBackground();
	u32 osystem_startOfFrame();
	void osystem_endOfFrame();

	void osystem_initGL(int screenWidth, int screenHeight);

	void osystem_delay(int time);
	void osystem_fadeBlackToWhite();
	void osystem_updateImage();
	void osystem_initBuffer();
	void osystem_initVideoBuffer(char *buffer, int width, int height);
	void osystem_putpixel(int x, int y, int pixel);
	void osystem_setColor(byte i, byte R, byte G, byte B);
	void osystem_setPalette(unsigned byte * palette);
    void osystem_setPalette(palette_t* palette);
	void osystem_setPalette320x200(byte * palette);
	void osystem_flip(unsigned char *videoBuffer);
	void osystem_draw320x200BufferToScreen(unsigned char *videoBuffer);
	void osystem_CopyBlockPhys(unsigned char* videoBuffer, int left, int top, int right, int bottom);
	void osystem_refreshFrontTextureBuffer();
	void osystem_drawText(int X, int Y, char *text);
	void osystem_drawTextColor(int X, int Y, char *string, unsigned char R, unsigned char G, unsigned char B);
	void osystem_drawLine(int X1,int X2,int Y1,int Y2,unsigned char color, unsigned char* palette);
	void osystem_playSampleFromName(char* sampleName);
	void osystem_playSample(char* samplePtr,int size);
	//    void getMouseStatus(mouseStatusStruct * mouseData);

	void osystem_createMask(const std::array<u8, 320 * 200>& mask, int roomId, int maskId, unsigned char* refImage, int maskX1, int maskY1, int maskX2, int maskY2);
	void osystem_drawMask(int roomId, int maskId);

	void osystem_startFrame();
	void osystem_stopFrame();
	void osystem_startModelRender();
	void osystem_stopModelRender();

	// clip
	void osystem_setClip(float left, float top, float right, float bottom);
	void osystem_clearClip();

	void osystem_cleanScreenKeepZBuffer();

	void osystem_fillPoly(float* buffer, int numPoint, unsigned char color,u8 polyType);
	void osystem_draw3dLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned char color);
	void osystem_draw3dQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, unsigned char color, int transparency);
	void osystem_drawSphere(float X, float Y, float Z, u8 color, u8 material, float size);
	void osystem_drawPoint(float X, float Y, float Z, u8 color, u8 material, float size);
	void osystem_flushPendingPrimitives();

	void osystem_startBgPoly();
	void osystem_endBgPoly();
	void osystem_addBgPolyPoint(int x, int y);

	int osystem_playTrack(int trackId);
	void osystem_playAdlib();

#endif
