#include "common.h"

// ITD_RESS mapping
#define JACK_CADRE_SPF					0
#define JACK_ITDFONT					1
#define JACK_LIVRE						2
#define JACK_IM_EXT_JACK				3

void startJACK()
{
	fontHeight = 16; // TODO: check
	startGame(16,1,1);
}

void JACK_ReadBook(int index, int type)
{
	switch(type)
	{
	case 1: // READ_BOOK
		{
			unsigned char* pImage = (unsigned char*)loadPak("ITD_RESS", JACK_LIVRE);
			memcpy(aux, pImage, 320*200);
			unsigned char* lpalette = pImage+320*200; 
			convertPaletteIfRequired(lpalette);
			copyPalette(lpalette,currentGamePalette);
			setPalette(lpalette);
			free(pImage);
			turnPageFlag = 1;
			Lire(index, 60, 10, 245, 190, 0, 124, 124);
			break;
		}
	default:
		assert(0);
	}
}