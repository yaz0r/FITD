// seg 4

#include "common.h"

#ifdef PCLIKE
#include "SDL.h"
#endif


//////////// stuff to move


void blitScreenTatou(void)
{
	/*
    int i;

    for(i=0;i<45120;i++)
    {
        frontBuffer[i] = backbuffer[i];
    }
	*/
}

void FastCopyScreen(void* source, void* dest)
{
	memcpy(dest, source, 64000);
}

void FadeInPhys(int step,int start)
{
    freezeTime();

    if(fadeState == 2) // only used for the ending ?
    {
    }
    else
    {
        for(int i=0;i<256;i+=step)
        {
			process_events();
            palette_t localPalette;
            computePalette(currentGamePalette,localPalette,i);
            setPalette(localPalette);
			osystem_refreshFrontTextureBuffer();
			osystem_drawBackground();
        }
    }

    fadeState = 1;

    unfreezeTime();
}

void FadeOutPhys(int var1, int var2)
{
    freezeTime();

    for(int i=256;i>=0;i-=var1)
    {
		process_events();
        palette_t localPalette;
		computePalette(currentGamePalette,localPalette,i);
		setPalette(localPalette);
		osystem_refreshFrontTextureBuffer();
		osystem_drawBackground();
    }

    unfreezeTime();
}

#ifdef PCLIKE
void process_events( void )
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
        case SDL_KEYDOWN:
            /* Handle key presses. */
            //            handle_key_down( &event.key.keysym );
            break;
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c). */
            exit( 0 );
            break;
        }

    }

}
#else
void process_events( void )
{
    osystem_endOfFrame();
	u32 timeIncrease = osystem_startOfFrame();
    assert(timeIncrease);
#ifdef FITD_DEBUGGER
	if(debuggerVar_fastForward)
	{
		timeGlobal+=8;
	}
	else
#endif
	{
        timeGlobal += timeIncrease;
	}
	timer=timeGlobal;
}
#endif

void startChrono(unsigned int* chrono)
{
    *chrono = timer;
}

int evalChrono(unsigned int* chrono)
{
    return(timer-*chrono);
}

// bp = x, bx = y, cx = z
// out
// xOut = dx, yOut = ax
void Rotate(unsigned int x, unsigned int y, unsigned int z, int* xOut, int* yOut)
{
    if(x)
    {
        int var1 = (((cosTable[(x+0x100)&0x3FF] * y) << 1) &0xFFFF0000) -  (((cosTable[x&0x3FF] *z) <<1) & 0xFFFF0000);
        int var2 = (((cosTable[x&0x3FF] * y) << 1) & 0xFFFF0000) + (((cosTable[(x+0x100)&0x3FF] *z) <<1) & 0xFFFF0000);

        *yOut = var1>>16;
        *xOut = var2>>16;
    }
    else
    {
        *xOut = z;
        *yOut = y;
    }
}

void setCameraTarget(int x,int y,int z,int alpha,int beta,int gamma,int time)
{
    int x1;
    int y1;
    int x2;
    int y2;

    Rotate(alpha+0x200,-time,0,&x1,&y1);
    Rotate(beta+0x200,y1,0,&x2,&y2);

    SetPosCamera(x2+x,-x1+y,y2+z);
    SetAngleCamera(alpha,beta,gamma);
}

void playSound(int num)
{
	if(num == -1)
		return;

    char sampleFileName[256] = "";
    if (g_gameId == TIMEGATE)
    {
        strcpy(sampleFileName, "SAMPLES");
    }
    else
    {
        strcpy(sampleFileName, "LISTSAMP");
    }

    int size = getPakSize(sampleFileName,num);
    assert(size);

    char* ptr = HQR_Get(listSamp,num);
    assert(ptr);

    osystem_playSample(ptr, size);
}

////////////////////////

