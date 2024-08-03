#include "common.h"

void printString(int index, int textIndex, int selectedIndex)
{
    int topPosition;

    topPosition = (((currentMenuBottom - currentMenuTop) / 2) + currentMenuTop) + (index << 4) - 56;

    if(index == selectedIndex)
    {
        drawSlectedText(0xA0,topPosition,textIndex,0xF,4);
    }
    else
    {
        SimpleMessage(0xA0,topPosition,textIndex,4);
    }
}

void drawSystemMenuLayout(int selectedStringNumber)
{
    int backupTop;
    int backupBottom;
    int backupLeft;
    int backupRight;

    AffBigCadre(160,100,320,200);

    backupTop = currentMenuTop;
    backupBottom = currentMenuBottom;
    backupLeft = currentMenuLeft;
    backupRight = currentMenuRight;

    AffBigCadre(80,55,120,70);

    //scaleDownImage(40,35,aux2);

    currentMenuTop = backupTop;
    currentMenuBottom = backupBottom;
    currentMenuLeft = backupLeft;
    currentMenuRight = backupRight;

    SetClip(currentMenuLeft,currentMenuTop,currentMenuRight,currentMenuBottom);

    printString(0,48,selectedStringNumber);
    printString(1,45,selectedStringNumber);
    printString(2,46,selectedStringNumber);
    printString(3,41+musicEnabled,selectedStringNumber);
    printString(4,43/*+soundToggle*/,selectedStringNumber);
    printString(5,49/*+detailToggle*/,selectedStringNumber);
    printString(6,47,selectedStringNumber);

    menuWaitVSync();
}

void processSystemMenu(void)
{
    //int entry = -1;
    int exitMenu = 0;
    int currentSelectedEntry;

    freezeTime();
    //pauseShaking();

    if(lightOff)
    {
        //makeBlackPalette();
    }

    //clearScreenSystemMenu(unkScreenVar,aux2);

    currentSelectedEntry = 0;

    while(!exitMenu)
    {
        drawSystemMenuLayout(currentSelectedEntry);
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

            if(!input5)
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
                            flagInitView = 2;
                            unfreezeTime();
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

                        input5 = 1;
                    }
                    if(localJoyD == 2) // bottom
                    {
                        currentSelectedEntry++;

                        if(currentSelectedEntry>6)
                            currentSelectedEntry = 0;

                        input5 = 1;
                    }
                }
            }
            else
            {
                if(!localKey && !localJoyD)
                {
                    input5 = 0;
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
	flagInitView = 2;
	unfreezeTime();
}