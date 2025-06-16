#include "common.h"

#define	NB_OPTIONS	7
#define SELECT_COUL 0xF
#define MENU_COUL 4
#define	SIZE_FONT 16

void AffOption(int n, int num, int selected)
{
    int y = WindowY1 + ((WindowY2 - WindowY1) / 2) - (NB_OPTIONS * SIZE_FONT) / 2 + (n * SIZE_FONT);

    if(n == selected)
    {
        SelectedMessage( 160, y, num, SELECT_COUL, MENU_COUL);
    }
    else
    {
        SimpleMessage( 160, y, num, MENU_COUL);
    }
}

void AffOptionList(int selectedStringNumber)
{
    AffBigCadre(160,100,320,200);

    int backupTop = WindowY1;
    int backupBottom = WindowY2;
    int backupLeft = WindowX1;
    int backupRight = WindowX2;

    AffBigCadre(80,55,120,70);

    //scaleDownImage(40,35,aux2);

    WindowY1 = backupTop;
    WindowY2 = backupBottom;
    WindowX1 = backupLeft;
    WindowX2 = backupRight;

    SetClip(WindowX1,WindowY1,WindowX2,WindowY2);

    AffOption(0,48,selectedStringNumber);
    AffOption(1,45,selectedStringNumber);
    AffOption(2,46,selectedStringNumber);
    AffOption(3,41+musicEnabled,selectedStringNumber);
    AffOption(4,43/*+soundToggle*/,selectedStringNumber);
    AffOption(5,49/*+detailToggle*/,selectedStringNumber);
    AffOption(6,47,selectedStringNumber);

    menuWaitVSync();
}

void processSystemMenu(void)
{
    //int entry = -1;
    int exitMenu = 0;
    int currentSelectedEntry;

    SaveTimerAnim();
    //pauseShaking();

    if(lightOff)
    {
        //makeBlackPalette();
    }

    //clearScreenSystemMenu(unkScreenVar,aux2);

    currentSelectedEntry = 0;

    while(!exitMenu)
    {
        AffOptionList(currentSelectedEntry);
        osystem_CopyBlockPhys((unsigned char*)logicalScreen,0,0,320,200);
        osystem_startFrame();
		process_events();
		flushScreen();
		osystem_drawBackground();

        if(lightOff)
        {
            FadeInPhys(0x40,0);
        }

        //  while(!exitMenu)
        {
            localKey = key;
            localClick = Click;
            localJoyD = JoyD;

            if(!AntiRebond)
            {
                if(localKey == 0x1C || localClick) // enter
                {
                    switch(currentSelectedEntry)
                    {
                    case 0: // exit menu
                        exitMenu = 1;
                        break;
                    case 1: // save
                        makeSave(45);
                        break;
                    case 2: // load
                        if(restoreSave(46,1))
                        {
                            FlagInitView = 2;
                            RestoreTimerAnim();
                            //updateShaking();
                            return;
                        }
                        break;

                    }
                }
                else
                {
                    if(localKey == 0x1B)
                    {
                        exitMenu = 1;
                    }
                    if(localJoyD == 1) // up
                    {
                        currentSelectedEntry--;

                        if(currentSelectedEntry<0)
                            currentSelectedEntry = 6;

                        AntiRebond = 1;
                    }
                    if(localJoyD == 2) // bottom
                    {
                        currentSelectedEntry++;

                        if(currentSelectedEntry>6)
                            currentSelectedEntry = 0;

                        AntiRebond = 1;
                    }
                }
            }
            else
            {
                if(!localKey && !localJoyD)
                {
                    AntiRebond = 0;
                }
            }
        }

        osystem_flip(NULL);
    }

	//fadeOut(32,2);
	while(key || JoyD || Click)
	{
		process_events();
	}
	localKey = localClick = localJoyD = 0;
	FlagInitView = 2;
	RestoreTimerAnim();
}