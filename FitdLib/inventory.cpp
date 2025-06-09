#include "common.h"

s16 currentInventory = 0;
s16 numObjInInventoryTable[NUM_MAX_INVENTORY];
s16 inHandTable[NUM_MAX_INVENTORY];
s16 inventoryTable[NUM_MAX_INVENTORY][INVENTORY_SIZE];

int statusLeft;
int statusTop;
int statusRight;
int statusBottom;

int numInventoryActions;
s16 inventoryActionTable[5];

int DrawListObjets(int startIdx, int selectIdx, int selectColor)
{
    int y = WindowY1+1;
    int var_6 = startIdx;
    int var_8;
    int i;
    int currentObj;
    tWorldObject* objPtr;

	if(g_gameId <= JACK)
	{
		AffBigCadre(160,50,320,100);
		y = WindowY1+1;
	}
	else
	{
		SetClip(27,25,292,98);
		fillBox(27,25,292,98,0);

		WindowX1 = 30;
		WindowY1 = 27;
		WindowX2 = 288;
		WindowY2 = 95;

		y = 28;
	}

    for(i=0;i<5;i++)
    {
        if(startIdx>=numObjInInventoryTable[currentInventory])
            break;

        currentObj = inventoryTable[currentInventory][startIdx];

        objPtr = &ListWorldObjets[currentObj];

        if(startIdx == selectIdx)
        {
			if(g_gameId <= JACK)
			{
				if(selectColor == 15)
				{
					fillBox(0xA,y,0x135,y+0x10,0x64);
				}

				SelectedMessage(160,y,objPtr->foundName,selectColor,4);
			}
			else
			{
				SimpleMessage(160,y,objPtr->foundName,selectColor);
			}

            var_8 = currentObj;
        }
        else
        {
            SimpleMessage(160,y,objPtr->foundName,4);
        }

        y += fontHeight;
        startIdx++;
    }


	if(var_6>0)
	{
		AffSpfI(298,10,10,PtrCadre);
	}

    if(var_6+5 < numObjInInventoryTable[currentInventory])
    {
        AffSpfI(298,74,9,PtrCadre);
    }

    return(var_8);
}

void renderInventoryObject(int arg)
{
    SetClip(statusLeft,statusTop,statusRight,statusBottom);
    fillBox(statusLeft,statusTop,statusRight,statusBottom,0);

    statusVar1 -= 8;

    setCameraTarget(0,0,0,60,statusVar1,0,24000);
    AffObjet(0,0,0,0,0,0,currentFoundBody);

    if(arg!=-1)
    {
        u8 buffer[256];
        SetFont(PtrFont,4);
        sprintf((char*)buffer,"%d",vars[arg]);
        PrintFont(statusLeft+4,statusTop+4,logicalScreen,buffer);
    }

	switch(g_gameId)
	{
	case AITD2:
		redrawInventorySpriteAITD2();
		break;
	default:
		break;
	}

    menuWaitVSync();
}

void drawInventoryActions(int arg)
{
    int y = 0;

	if(g_gameId <= JACK)
	{
		AffBigCadre(240,150,160,100);
		y = 150 - ((numInventoryActions<<4)/2);
	}
	else
	{
		SetClip(162,100,292,174);
		fillBox(162,100,292,174, 0);

		WindowX1 = 166;
		WindowY1 = 104;
		WindowX2 = 288;
		WindowY2 = 170;

		y = 139 - ((numInventoryActions*fontHeight)/2);
	}

    for(int i=0;i<numInventoryActions;i++)
    {
        if(arg == i)
        {
			if(g_gameId <= JACK)
			{
				fillBox(170,y,309,y+16,100);
				SelectedMessage(240,y,inventoryActionTable[i],15,4);
			}
			else
			{
				SimpleMessage(240,y,inventoryActionTable[i],1);
			}
        }
        else
        {
            SimpleMessage(240,y,inventoryActionTable[i],4);
        }

        y += fontHeight;
    }

	switch(g_gameId)
	{
	case AITD2:
		redrawInventorySpriteAITD2();
		break;
	default:
		break;
	}
}

void processInventory(void)
{
    int exitMenu = 0;
    int choice = 0;
    int firstTime = 1;
    unsigned int chrono;
    int lastSelectedObjectIdx;
    int selectedWorldObjectIdx;
    int selectedObjectIdx;
    int selectedActions;
    int firstObjectDisplayedIdx;
    int antiBounce;
    int modeSelect;
    int var_C;
    int numActionForObject;

    if(!numObjInInventoryTable[currentInventory])
        return;

    firstObjectDisplayedIdx = 0;
    lastSelectedObjectIdx = -1;
    selectedObjectIdx = 0;
    modeSelect = 0;
    antiBounce = 2;

    statusVar1 = 0;

    SaveTimerAnim();
    //pauseShaking();

    if(lightOff!=0)
    {
        //makeBlackPalette();
    }

	switch(g_gameId)
	{
	case AITD1:
	case JACK:
		AffBigCadre(80,150,160,100);

		statusLeft = WindowX1;
		statusTop = WindowY1;
		statusRight = WindowX2;
		statusBottom = WindowY2;

		setupCameraProjection(((statusRight-statusLeft)/2)+statusLeft,((statusBottom-statusTop)/2) + statusTop,128,400,390);

		break;
	case AITD2:
		drawInventoryAITD2();
		break;
	case AITD3:
		drawInventoryAITD3();
		break;
	default:
		assert(0);
	}

    while(!exitMenu)
    {
		/*
        osystem_CopyBlockPhys((unsigned char*)backbuffer,0,0,320,200);
        osystem_startFrame();
        osystem_cleanScreenKeepZBuffer();
		*/

        process_events();
		osystem_drawBackground();

        localKey = key;
        localJoyD = JoyD;
        localClick = Click;

        if(!localKey && !localJoyD && !localClick)
        {
            antiBounce = 0;
        }

        if(localKey == 1)
        {
            choice = 0;
            exitMenu = 1;
        }

        if(modeSelect == 0)
        {
            if(antiBounce<1)
            {
                if((localKey == 0x1C) || (localClick != 0) || (localJoyD == 0xC))
                {
                    DrawListObjets(firstObjectDisplayedIdx,selectedObjectIdx,14);
                    menuWaitVSync();
					osystem_CopyBlockPhys((unsigned char*)logicalScreen,0,0,320,200);
                    modeSelect = 1;
                    lastSelectedObjectIdx = -1;
                    selectedActions = 0;
					antiBounce = 2;
					continue;
                }
                else
                {
                    if(localJoyD&1 && selectedObjectIdx>0)
                    {   
                        selectedObjectIdx--;
                    }

                    if(localJoyD&2 && selectedObjectIdx < (numObjInInventoryTable[currentInventory]-1))
                    {
                        selectedObjectIdx++;
                    }

                    if(firstObjectDisplayedIdx+5 <= selectedObjectIdx)
                    {
                        firstObjectDisplayedIdx++;
                    }

                    if(selectedObjectIdx < firstObjectDisplayedIdx)
                    {
                        firstObjectDisplayedIdx--;
                    }

                    if(localKey || localJoyD || localClick)
                    {
                        if(antiBounce==0)
                        {
                            antiBounce = 1;
                            startChrono(&chrono);
                        }
                    }
                }
            }
            else
            {
                if(antiBounce == 1)
                {
                    if(evalChrono(&chrono)>0x280000)
                    {
                        antiBounce = -1;
                    }
                }
            }

            if(lastSelectedObjectIdx!=selectedObjectIdx)
            {
                selectedWorldObjectIdx = DrawListObjets(firstObjectDisplayedIdx,selectedObjectIdx,15);

                currentFoundBodyIdx = ListWorldObjets[selectedWorldObjectIdx].foundBody;

                currentFoundBody = HQR_Get(listBody,currentFoundBodyIdx);

                var_C = ListWorldObjets[selectedWorldObjectIdx].foundFlag;

                numInventoryActions = 0;
                numActionForObject = 0;

                while(numActionForObject<11)
                {
                    if(var_C & (1<<numActionForObject))
                    {
                        if(numInventoryActions<5)
                        {
                            inventoryActionTable[numInventoryActions++] = numActionForObject+23;
                        }
                    }
                    numActionForObject ++;
                }

                drawInventoryActions(-1);
                //osystem_flip(NULL);

                lastSelectedObjectIdx = selectedObjectIdx;
            }
        }
        else // actions
        {
            if(antiBounce<1)
            {
                if(localKey == 0x1C || localClick)
                {
                    selectedObjectIdx = inventoryTable[currentInventory][selectedObjectIdx];
                    action = 1 << (inventoryActionTable[selectedActions] - 23);
                    choice = 1;
                    exitMenu = 1;
                }

                if(localJoyD & 0xC)
                {
                    drawInventoryActions(-1);
                    modeSelect = 0;
                    lastSelectedObjectIdx = -1;
                    antiBounce = 2;
                    continue;
                }

                if(localJoyD&1 && selectedActions>0)
                {
                    selectedActions --;
                }

                if(localJoyD&2 && selectedActions<(numInventoryActions-1))
                {
                    selectedActions++;
                }

                if(localKey || localJoyD || localClick)
                {
                    if(antiBounce==0)
                    {
                        antiBounce = 1;
                        startChrono(&chrono);
                    }
                }
            }
            else
            {
                if(antiBounce==1)
                {
                    if(evalChrono(&chrono)>0x280000)
                    {
                        antiBounce = -1;
                    }
                }

                if(lastSelectedObjectIdx != selectedActions)
                {
                    lastSelectedObjectIdx = selectedActions;
                    drawInventoryActions(lastSelectedObjectIdx);
                    menuWaitVSync();
                }
            }
        }
        renderInventoryObject(ListWorldObjets[selectedWorldObjectIdx].floorLife);

        if(firstTime)
        {
            firstTime = 0;
            if(lightOff)
            {
                FadeInPhys(0x40,0);
            }
        }

		osystem_CopyBlockPhys((unsigned char*)logicalScreen,0,0,320,200);
        //osystem_flip(NULL);
    }

    RestoreTimerAnim();

    flagInitView = 1;

    while(Click || key || JoyD)
    {
      process_events();
    }

    localJoyD = 0;
    localKey = 0;
    localClick = 0;

    if(choice == 1)
    {
        executeFoundLife(selectedObjectIdx);
    }

    //updateShaking();
}
