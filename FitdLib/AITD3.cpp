#include "common.h"

#define AITD3_CADRE_SPF						0
#define AITD3_ITDFONT						1
#define AITD3_LETTRE						2
#define AITD3_LIVRE							3
#define AITD3_CARNET						4
#define AITD3_CYM00001						5
#define AITD3_CYM00007						6
#define AITD3_CYM00013						7
#define AITD3_CYM00014						8
#define AITD3_CYM01017						9
#define AITD3_CYM03011						10
#define AITD3_CYM13005						11
#define AITD3_CYM13013						12
#define AITD3_MENU3							13
#define AITD3_INVENTAIRE_CAVERNE			14
#define AITD3_INVENTAIRE_COWBOY				15
#define AITD3_INVENTAIRE_COUGUAR			16
#define AITD3_OPTION_SCREEN					17
#define AITD3_SPRITES_INVENTAIRE_CAVERNE	18
#define AITD3_SPRITES_INVENTAIRE_COWBOY		19
#define AITD3_SPRITES_INVENTAIRE_COUGUAR	20

#define MENU_PAL					47
#define MAP							48

#define NBSPRITES 3 /* amount of sprites by inventory */
#define NBDISPLAY 3 /* amount of inventory screens */

#define MENU_MESSAGES				11
#define MESSAGE_LOAD_GAME			46
#define MESSAGE_SAVE_GAME			45
#define START_OPTIONS_INVENTAIRE	23      /* num message.eng */

void startAITD3()
{
	g_gameUseCDA = true;
	startGame(1,0,0);
	
	while(1)
	{
		int startupMenuResult = processStartupMenu();

		switch(startupMenuResult)
		{
		case -1: // timeout
			{
				startGame(1,0,0);

				break;
			}
		case 0: // new game
			{
				startGame(0,12,1);

				break;
			}
		case 1: // continue
			{
				if(restoreSave(12,0))
				{
					//          updateShaking();

					flagInitView = 2;

					setupCamera();

					mainLoop(1, 1);

					//          freeScene();

					FadeOutPhys(8,0);
				}

				break;
			}
		case 2: // exit
			{
				freeAll();
				exit(-1);

				break;
			}
		}
	}
}

void drawInventoryAITD3()
{
	switch (CVars[getCVarsIdx(TYPE_INVENTAIRE)])
	{
	case 0:
		LoadPak("ITD_RESS", AITD3_INVENTAIRE_CAVERNE, logicalScreen);
		break;
	case 1:
		LoadPak("ITD_RESS", AITD3_INVENTAIRE_COWBOY, logicalScreen);
		break;
	case 2:
		LoadPak("ITD_RESS", AITD3_INVENTAIRE_COUGUAR, logicalScreen);
		break;
	default:
		assert(0);
	}

	statusLeft = 27;
	statusTop = 100;
	statusRight = 159;
	statusBottom = 174;

	setupCameraProjection(((statusRight-statusLeft)/2)+statusLeft,((statusBottom-statusTop)/2) + statusTop,128,400,390);
}