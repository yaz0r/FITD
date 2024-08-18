#include "common.h"

void drawStartupMenu(int selectedEntry)
{
    int currentY = 76;
    int currentTextNum = 0;

    AffBigCadre(160,100,320,80);

    while(currentTextNum<3)
    {
        if(currentTextNum == selectedEntry) // hilight selected entry
        {
            fillBox(10,currentY,309,currentY+16,100);
            SelectedMessage(160,currentY,currentTextNum+11,15,4);
        }
        else
        {
            SimpleMessage(160,currentY,currentTextNum+11,4);
        }

        currentY+=16; // next line
        currentTextNum++; // next text
    }
}

int processStartupMenu(void)
{
    int currentSelectedEntry = 0;
    unsigned int chrono;
    int selectedEntry = -1;

    flushScreen();

    drawStartupMenu(0);

    osystem_startFrame();
    osystem_stopFrame();
    osystem_CopyBlockPhys((unsigned char*)logicalScreen,0,0,320,200);

    osystem_flip(NULL);
    FadeInPhys(16,0);
    startChrono(&chrono);

    while(evalChrono(&chrono) <= 0x10000) // exit loop only if time out or if choice made
    {
        osystem_CopyBlockPhys((unsigned char*)logicalScreen,0,0,320,200);
        osystem_startFrame();

        if(selectedEntry!=-1 || evalChrono(&chrono) > 0x10000)
        {
            break;
        }

        process_events();
		osystem_drawBackground();

        if(JoyD&1) // up key
        {
            currentSelectedEntry--;

            if(currentSelectedEntry<0)
            {
                currentSelectedEntry = 2;
            }

            drawStartupMenu(currentSelectedEntry);
            osystem_flip(NULL);
            //      menuWaitVSync();

            startChrono(&chrono);

            while(JoyD)
            {
                process_events();
            }
        }


        if(JoyD&2) // down key
        {
            currentSelectedEntry++;

            if(currentSelectedEntry>2)
            {
                currentSelectedEntry = 0;
            }

            drawStartupMenu(currentSelectedEntry);
            //menuWaitVSync();
            osystem_flip(NULL);

            startChrono(&chrono);

            while(JoyD)
            {
                process_events();
            }
        } 

        if(key == 28 || (key != 28 && Click!=0)) // select current entry
        {
            selectedEntry = currentSelectedEntry;
        }
        osystem_stopFrame();
        osystem_flip(NULL);
    }

    if(selectedEntry==2) // if exit game, do not fade
    {
        FadeOutPhys(16,0);
    }

    while(JoyD)
    {
        process_events();
    }

    return(selectedEntry);
}
