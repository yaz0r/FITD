//////////////////////////
// FITD - Free in the Dark
//////////////////////////

// 22 septembre 2003 14:25

// seg002

#include "common.h"
#include "hybrid.h"
#include "anim2d.h"

#include <time.h>

#ifndef AITD_UE4
#include "bgfxGlue.h"
#include <bgfx/bgfx.h>
#endif

#include <array>
#include <filesystem>

extern "C" {
	extern char homePath[512];
}

FILE* Open(const char* filename, const char* mode) {
    std::filesystem::path path = std::filesystem::path(homePath) / filename;
    return fopen(path.string().c_str(), mode);
}

int AntiRebond;

int* currentCVarTable = NULL;

int getCVarsIdx(enumCVars searchedType) // TODO: optimize by reversing the table....
{
	for(int i=0;i<CVars.size();i++)
	{
		if(currentCVarTable[i] == -1)
		{
			ASSERT(0);
		}


		if(currentCVarTable[i] == searchedType)
			return i;
	}

	ASSERT(0);
	return 0;
}

int getCVarsIdx(int searchedType)
{
	return getCVarsIdx((enumCVars)searchedType);
}

const unsigned char defaultPalette[0x30] =
{
	0x00,
	0x00,
	0x00,
	0x3F,
	0x3F,
	0x3F,
	0x0C,
	0x0C,
	0x0E,
	0x30,
	0x2F,
	0x3F,
	0x23,
	0x2C,
	0x23,
	0x2A,
	0x1D,
	0x2A,
	0x2A,
	0x21,
	0x18,
	0x3F,
	0x05,
	0x2A,
	0x12,
	0x14,
	0x18,
	0x31,
	0x15,
	0x17,
	0x15,
	0x25,
	0x15,
	0x15,
	0x2F,
	0x3F,
	0x3F,
	0x22,
	0x15,
	0x2B,
	0x15,
	0x3F,
	0x3F,
	0x3F,
	0x21,
	0x3F,
	0x3F,
	0x3F
};

const unsigned char defaultPaletteAITD3[0x30]=
{
	0x00,
	0x00,
	0x00,
	0xFC,
	0xFC,
	0xFC,
	0x30,
	0x30,
	0x38,
	0xC0,
	0xBC,
	0xFC,
	0x78,
	0x58,
	0x3C,
	0x00,
	0x00,
	0x00,
	0xF0,
	0x70,
	0x10,
	0xFC,
	0xFC,
	0xFC,
	0x48,
	0x50,
	0x60,
	0xC4,
	0x54,
	0x5C,
	0x54,
	0x94,
	0x54,
	0x54,
	0xBC,
	0xFC,
	0xFC,
	0x88,
	0x54,
	0xAC,
	0x54,
	0xFC,
	0xFC,
	0xFC,
	0xFC,
	0xFC,
	0xFC,
	0xF8
};

void executeFoundLife(int objIdx)
{
	int var_2;
	int actorIdx;
	int lifeOffset;
	int currentActorIdx;
	int currentActorLifeIdx;
	int currentActorLifeNum;
	int foundLife;
	tObject* currentActorPtr;
	tObject* currentActorLifePtr;

	if(objIdx == -1)
		return;

	foundLife = ListWorldObjets[objIdx].foundLife;

	if(ListWorldObjets[objIdx].foundLife == -1)
		return;

	currentActorPtr = currentProcessedActorPtr;
	currentActorIdx = currentProcessedActorIdx;
	currentActorLifeIdx = currentLifeActorIdx;
	currentActorLifePtr = currentLifeActorPtr;
	currentActorLifeNum = currentLifeNum;

	if(currentLifeNum != -1)
	{
		lifeOffset = (currentLifePtr - HQR_Get(listLife,currentActorLifeNum))/2;
	}

	var_2 = 0;

	actorIdx = ListWorldObjets[objIdx].objIndex;

	if(actorIdx==-1)
	{
		tObject* currentActorEntryPtr = &ListObjets[NUM_MAX_OBJECT-1];
		int currentActorEntry = NUM_MAX_OBJECT-1;

		while(currentActorEntry>=0)
		{
			if(currentActorEntryPtr->indexInWorld == -1)
				break;

			currentActorEntryPtr--;
			currentActorEntry--;
		}

		if(currentActorEntry==-1) // no space, we will have to overwrite the last actor !
		{
			currentActorEntry = NUM_MAX_OBJECT-1;
			currentActorEntryPtr = &ListObjets[NUM_MAX_OBJECT-1];
		}

		actorIdx = currentActorEntry;
		var_2 = 1;

		currentProcessedActorPtr = &ListObjets[actorIdx];
		currentLifeActorPtr = &ListObjets[actorIdx];
		currentProcessedActorIdx = actorIdx;
		currentLifeActorIdx = actorIdx;

		currentProcessedActorPtr->indexInWorld = objIdx;
		currentProcessedActorPtr->life = -1;
		currentProcessedActorPtr->bodyNum = -1;
		currentProcessedActorPtr->objectType = 0;
		currentProcessedActorPtr->trackMode = -1;
		currentProcessedActorPtr->room = -1;
		currentProcessedActorPtr->lifeMode = -1;
		currentProcessedActorPtr->ANIM = -1;
	}

	processLife(foundLife, true);

	if(var_2)
	{
		currentProcessedActorPtr->indexInWorld = -1;
	}

	currentProcessedActorPtr = currentActorPtr;
	currentProcessedActorIdx = currentActorIdx;
	currentLifeActorIdx = currentActorLifeIdx;
	currentLifeActorPtr = currentActorLifePtr;

	if(currentActorLifeNum != -1)
	{
		currentLifeNum = currentActorLifeNum;
		currentLifePtr = HQR_Get(listLife,currentLifeNum) + lifeOffset * 2;
	}
}

void InitCopyBox(char* var0, char* var1)
{
	screenSm1 = var0;
	screenSm2 = var0;

	screenSm3 = var1;
	screenSm4 = var1;
	screenSm5 = var1;
}

void allocTextes(void)
{
	int currentIndex;
	u8* currentPosInTextes;
	int textCounter;
	int stringIndex;
	u8* stringPtr;
	int textLength;

	tabTextes = (textEntryStruct*)malloc(NUM_MAX_TEXT_ENTRY * sizeof(textEntryStruct)); // 2000 = 250 * 8

	ASSERT_PTR(tabTextes);

	if(!tabTextes)
	{
		fatalError(1,"TabTextes");
	}

	// setup languageNameString
	{
        for (int i=0; i<languageNameTable.size(); i++)
        {
            char tempString[20];

            strcpy(tempString, languageNameTable[i].c_str());
            strcat(tempString, ".PAK");

            if (fileExists(tempString))
            {
                languageNameString = languageNameTable[i].c_str();
                break;
            }
        }
	}

	if(!languageNameString.length())
	{
		printf("Unable to detect language file..\n");
		assert(0);
	}

	systemTextes = (u8*)CheckLoadMallocPak(languageNameString.c_str(), 0); // todo: use real language name
	textLength = getPakSize(languageNameString.c_str(), 0);

	for(currentIndex=0;currentIndex<NUM_MAX_TEXT_ENTRY;currentIndex++)
	{
		tabTextes[currentIndex].index = -1;
		tabTextes[currentIndex].textPtr = NULL;
		tabTextes[currentIndex].width = 0;
	}

	currentPosInTextes = systemTextes;

	textCounter = 0;

	while(currentPosInTextes<systemTextes+textLength)
	{
		currentIndex = *(currentPosInTextes++);

		if(currentIndex == 26)
			break;

		if(currentIndex == '@') // start of string marker
		{
			stringIndex = 0;

			while((currentIndex = *(currentPosInTextes++)) >= '0' && currentIndex <= '9') // parse string number
			{
				stringIndex = stringIndex * 10 + currentIndex - 48;
			}

			if(currentIndex == ':') // start of string
			{
				stringPtr = currentPosInTextes;

				do
				{
					currentPosInTextes ++;
				}while((unsigned char)*(currentPosInTextes-1) >= ' '); // detect the end of the string

				*(currentPosInTextes-1) = 0; // add the end of string

				tabTextes[textCounter].index = stringIndex;
				tabTextes[textCounter].textPtr = stringPtr;
				tabTextes[textCounter].width = ExtGetSizeFont(stringPtr);

				textCounter++;
			}

			if(currentIndex == 26)
			{
				return;
			}
		}
	}
}

void OpenProgram(void)
{
	//time_t ltime;
	FILE* fHandle;

	setupScreen();
	//setupInterrupt();
	//setupInterrupt2();
	//setupInterrupt3();

	//setupVideoMode();

//	time( &ltime );

//	srand(ltime);

	if(!initMusicDriver())
	{
		musicConfigured = 0;
		musicEnabled = 0;
	}

	// TODO: reverse sound init code


	aux = (char*)malloc(65068);
	if(!aux)
	{
		fatalError(1,"Aux");
	}

	aux2 = (char*)malloc(65068);
	if(!aux2)
	{
		fatalError(1,"Aux2");
	}

	InitCopyBox(aux2,logicalScreen);
	/*  InitCopyPlot(aux2);
	InitSpecialCopyPoly(aux2); */

    BufferAnim.resize(NB_BUFFER_ANIM);

    switch(g_gameId)
	{
	case AITD3:
        PtrFont = CheckLoadMallocPak("ITD_RESS", 1);
        break;
	case JACK:
	case AITD2:
		PtrFont = CheckLoadMallocPak("ITD_RESS", 1);
		break;
	case AITD1:
		PtrFont = CheckLoadMallocPak("ITD_RESS", 5);
		break;
    case TIMEGATE:
        PtrFont = CheckLoadMallocPak("ITD_RESS", 2);
        break;
	default:
		assert(0);
	}

	SetFont(PtrFont, 14);

	if(g_gameId == AITD1)
	{
		SetFontSpace(2,0);
	}
	else
	{
		SetFontSpace(2,1);
	}

	switch(g_gameId)
	{
	case JACK:
	case AITD2:
	case AITD3:
		{
			PtrCadre = CheckLoadMallocPak("ITD_RESS",0);
			break;
		}
	case AITD1:
		{
			PtrCadre = CheckLoadMallocPak("ITD_RESS",4);
			break;
		}
	}

	PtrPrioritySample = loadFromItd("PRIORITY.ITD");

    // read cvars definitions
    {
        fHandle = Open("DEFINES.ITD", "rb");
        if (!fHandle)
        {
            fatalError(0, "DEFINES.ITD");
        }
        for (int i = 0; i < CVars.size(); i++) {
            s16 cvarValue = 0;
            fread(&cvarValue, 2, 1, fHandle);
            CVars[i] = READ_BE_S16(&cvarValue);
        }
        fclose(fHandle);
    }

	allocTextes();

	//  if(musicConfigured)
	{
		listMus = HQR_InitRessource<char>("LISTMUS",110000,40);
	}

    char sampleFileName[256] = "";
    if (g_gameId == TIMEGATE)
    {
        strcpy(sampleFileName, "SAMPLES");
    }
    else
    {
        strcpy(sampleFileName, "LISTSAMP");
    }

	listSamp = HQR_InitRessource<char>(sampleFileName,64000,30);

	HQ_Memory = HQR_Init(10000,50);
}

void freeAll(void)
{
	/*  HQR_Free(hqrUnk);

	HQR_Free(listSamp);

	HQR_Free(listMus);

	free(languageData);

	free(tabTextes);

	free(priority);

	free(aitdBoxGfx);

	free(fontData);

	free(bufferAnim);

	if(aux != aux3)
	{
	free(aux);
	}

	free(aux2);*/

	//TODO: implement all the code that restore the interrupts & all
}

textEntryStruct* getTextFromIdx(int index)
{
	int currentIndex;

	for(currentIndex = 0; currentIndex < NUM_MAX_TEXT_ENTRY; currentIndex++)
	{
		if(tabTextes[currentIndex].index == index)
		{
			return(&tabTextes[currentIndex]);
		}
	}

	return(NULL);
}

void AffRect(int x1, int y1, int x2, int y2, char color) // fast recode. No RE
{
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;

	char* dest = logicalScreen + y1*320 + x1;
	char* dest2 = (char*)uiLayer.data() + y1*320 + x1;

	int i;
	int j;

	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			*(dest++)= color;
			*(dest2++)= color;
		}

		dest += 320-width;
		dest2 += 320-width;
	}
}

void loadPalette(void)
{
    palette_t localPalette;

	if(g_gameId == AITD2)
	{
		//loadPakToPtr("ITD_RESS",59,aux);
	}
	else
	{
		LoadPak("ITD_RESS",3,aux);
	}
	copyPalette(aux,currentGamePalette);

	copyPalette(currentGamePalette,localPalette);
	//  fadeInSub1(localPalette);

	// to finish
}

void turnPageForward()
{
}

void turnPageBackward()
{
}

void readBook(int index, int type)
{
	SaveTimerAnim();

	switch(g_gameId)
	{
	case AITD1:
		AITD1_ReadBook(index, type);
		break;
	case JACK:
		JACK_ReadBook(index, type);
		break;
	case AITD2:
		AITD2_ReadBook(index, type);
		break;
	default:
		assert(0);

	}

	RestoreTimerAnim();
}

int Lire(int index, int startx, int top, int endx, int bottom, int demoMode, int color, int shadow)
{
	bool lastPageReached = false;
	u8 tabString[] = "    ";
	int firstpage = 1;
	int page = 0;
	int quit = 0;
	int previousPage = -1;
	int var_1C3;
	std::array<u8*, 100> ptrpage;
	int currentTextIdx;
	int maxStringWidth;
	u8* textPtr;

	SetFont(PtrFont, color);

	maxStringWidth = endx - startx + 4;

	int textIndexMalloc = HQ_Malloc(HQ_Memory,getPakSize(languageNameString.c_str(),index)+300);
	textPtr = (u8*)HQ_PtrMalloc(HQ_Memory, textIndexMalloc);

	if(!LoadPak( languageNameString.c_str(), index, (char*)textPtr))
	{
		fatalError(1, languageNameString.c_str() );
	}

    ptrpage.fill(nullptr);
	ptrpage[0] = textPtr;

	//  LastSample = -1;
	//  LastPriority = -1;

	while(!quit)
	{
		u8* ptrt;
		int currentTextY;
		FastCopyScreen(aux,logicalScreen);
		process_events();
		SetClip(startx,top,endx,bottom);

		uiLayer.fill(0);
		ptrt = ptrpage[page];

		currentTextY = top;
		lastPageReached = false;

		while(currentTextY <= bottom - 16)
		{
			int line_type = 1;
			int var_1BA = 0;
			int currentStringWidth;
			int currentTextX;

			regularTextEntryStruct* currentText = textTable;

			int numWordInLine = 0;

			int interWordSpace = 0;

            while (true) {
                while (*ptrt == '#')
                {
                    //char* var_1BE = var_1C2;
                    ptrt++;

                    switch (*(ptrt++))
                    {
                    case 'P': // page change
                    {
                        if (currentTextY > top) // Hu ?
                            goto pageChange;
                        break;
                    }
                    case 'T': // tab
                    {
                        currentText->textPtr = tabString;
                        currentText->width = ExtGetSizeFont(currentText->textPtr) + 3;
                        var_1BA += currentText->width;
                        numWordInLine++;
                        currentText++;
                        break;
                    }
                    case 'C': // center
                    {
                        line_type &= ~1;
                        line_type |= 8;
                        break;
                    }
                    case 'G': // print number
                    {
                        currentTextIdx = 0;

                        while (*ptrt >= '0' && *ptrt <= '9')
                        {
                            currentTextIdx = (currentTextIdx * 10 + *ptrt - 48);
                            ptrt++;
                        }

                        if (LoadPak("ITD_RESS", 9, aux2))
                        {
                            assert(0); // when is this used?
                            /*  var_C = printTextSub3(currentTextIdx,aux2);
                            var_A = printTextSub4(currentTextIdx,aux2);

                            if(currentTextY + var_A > bottom)
                            {
                            var_1C2 = var_1BE;

                            goto pageChange;
                            }
                            else
                            {
                            printTextSub5((((right-left)/2)+left)-var_C, currentTextY, currentTextIdx, aux2);
                            currentTextY = var_A;
                            }*/
                        }

                        break;
                    }
                    }
                }

                currentText->textPtr = ptrt;

                do
                {
                    var_1C3 = *((unsigned char*)ptrt++);
                } while (var_1C3 > ' '); // go to the end of the string

                *(ptrt - 1) = 0; // add end of string marker to cut the word

                currentStringWidth = ExtGetSizeFont(currentText->textPtr) + 3;

                if (currentStringWidth > maxStringWidth) {
                    quit = 1;
                    break;
                }

                if (var_1BA + currentStringWidth > maxStringWidth)
                {
                    ptrt = currentText->textPtr;
                    break;
                }

                currentText->width = currentStringWidth;
                var_1BA += currentStringWidth;

                numWordInLine++;
                currentText++;

                // eval the character that caused the 'end of word' state
                if (var_1C3 == 26) {
                    line_type &= ~1;
                    line_type |= 4;
                    lastPageReached = true;
                    break;
                }

                if (((var_1C3 == 13) || (var_1C3 == 0)) && (*ptrt < ' ')) {
                    ++ptrt;
                    if (*ptrt == 0xD)
                    {
                        ptrt += 2;
                        line_type &= ~1;
                        line_type |= 2;
                        break;
                    }
                    if (*ptrt == '#')
                    {
                        line_type &= ~1;
                        break;
                    }
                }
            }

            if(line_type & 1) // stretch words on line
            {
                interWordSpace = (maxStringWidth - var_1BA) / (numWordInLine-1);
            }

            currentText = textTable;

            if (line_type & 8) // center
            {
                currentTextX = startx + ((maxStringWidth - var_1BA) / 2);
            }
            else
            {
                currentTextX = startx;
            }

            for (int i = 0; i < numWordInLine; i++) {
                PrintFont(currentTextX, currentTextY, logicalScreen, currentText->textPtr);
                currentTextX += currentText->width + interWordSpace; // add inter word space
                currentText++;
            }
            currentTextIdx = 0;


            if (line_type & 2) // font size
            {
                currentTextY += 8;
            }

            currentTextY += 16;

            if (lastPageReached)
                break;
        }

        pageChange:
        if(lastPageReached)
		{
			*(ptrt-1) = 0x1A; // rewrite End Of Text
		}
		else
		{
			ptrpage[page+1] = ptrt;
		}

		if(demoMode == 0)
		{
			if(page>0)
			{
				AffSpfI(startx-19,185,12,PtrCadre);
			}

			if(!lastPageReached)
			{
				AffSpfI(endx+4,185,11,PtrCadre);
			}
		}

		if(demoMode == 2)
		{
			if(page>0)
			{
				AffSpfI(startx-3,191,13,PtrCadre);
			}

			if(!lastPageReached)
			{
				AffSpfI(endx-10,191,14,PtrCadre);
			}
		} 

		if(firstpage)
		{
			if(demoMode!=1)
			{
				osystem_CopyBlockPhys((unsigned char*)logicalScreen,0,0,320,200);
				FadeInPhys(16,0);
			}
			else
			{
				if(turnPageFlag)
				{
					turnPageForward();
				}
				else
				{
					osystem_CopyBlockPhys((unsigned char*)logicalScreen,0,0,320,200);
				}
			}

			firstpage = 0;
		}
		else
		{
			if(turnPageFlag)
			{
				if(previousPage<page)
				{
					turnPageForward();
				}
				else
				{
					turnPageBackward();
				}
			}
			else
			{
				osystem_CopyBlockPhys((unsigned char*)logicalScreen,0,0,320,200);
			}
		}

		osystem_drawBackground();

		if(demoMode!=1) // mode != 1: normal behavior (user can flip pages)
		{
			do
			{
				process_events();
			}while(key || JoyD || Click);

			while(1)
			{
				process_events();
				localKey = key;
				localJoyD = JoyD;
				localClick = Click;

				if((localKey==1) || localClick)
				{
					quit = 1;
					break;
				}

                if ((demoMode == 2) && (localKey == 0x1C)) {
                    quit = 1;
                    break;
                }

                // flip to next page
                if (JoyD & 0xA || localKey == 0x1C)
                {
                    if (!lastPageReached)
                    {
                        previousPage = page;
                        page++;

                        if (demoMode == 2)
                        {
                            playSound(CVars[getCVarsIdx(SAMPLE_PAGE)]);
                            LastSample = -1;
                            LastPriority = -1;
                        }
                        break;
                    }
                    else {
                        if (localKey == 0x1C) {
                            quit = 1;
                            break;
                        }
                    }
                }

                // flip to previous page
                if (JoyD & 5) {
                    if (page > 0) {
                        previousPage = page;
                        page--;
                        if (demoMode == 2)
                        {
                            playSound(CVars[getCVarsIdx(SAMPLE_PAGE)]);
                            LastSample = -1;
                            LastPriority = -1;
                        }
                        break;
                    }
                }
			}
		}
		else // Demo mode: pages automatically flips
		{
			unsigned int var_6;
			startChrono(&var_6);

			do
			{
				process_events();
				if(evalChrono(&var_6) > 300)
				{
					break;
				}
			}while(!key && !Click);

			if(key || Click)
			{
				quit = 1;
			}

			if(!lastPageReached)
			{
				page++;
				playSound(CVars[getCVarsIdx(SAMPLE_PAGE)]);
				LastSample = -1;
			}
			else
			{
				quit = 1;
				demoMode = 0;
			}
		}
	}

	HQ_Free_Malloc(HQ_Memory, textIndexMalloc);

	return(demoMode);
}

extern "C" {
	extern char homePath[512];
}

void LoadWorld(void)
{
	u8* pObjectData;
	u8* pObjectDataBackup;
	unsigned long int objectDataSize;
	FILE* fHandle;
	int i;
	int choosePersoBackup;

	char objectPath[512];
	strcpy(objectPath, homePath);
	strcat(objectPath, "OBJETS.ITD");
	fHandle = fopen(objectPath,"rb");
    if (!fHandle) {
        fatalError(0, "OBJETS.ITD");
    }

	fseek(fHandle,0,SEEK_END);
	objectDataSize = ftell(fHandle);
	fseek(fHandle,0,SEEK_SET);

	pObjectDataBackup = pObjectData = (u8*)malloc(objectDataSize);
	ASSERT(pObjectData);

	fread(pObjectData,objectDataSize,1,fHandle);
	fclose(fHandle);

	maxObjects = READ_LE_U16(pObjectData);
	pObjectData+=2;

    ListWorldObjets.resize(maxObjects);

	for(i=0;i<maxObjects;i++)
	{
		ListWorldObjets[i].objIndex = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].body = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].flags = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].typeZV = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].foundBody = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].foundName = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].foundFlag = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].foundLife = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].x = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].y = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].z = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].alpha = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].beta = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].gamma = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].stage = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].room = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].lifeMode = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].life = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].floorLife = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].anim = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].frame = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].animType = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].animInfo = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].trackMode = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].trackNumber = READ_LE_U16(pObjectData);
		pObjectData+=2;

		ListWorldObjets[i].positionInTrack = READ_LE_U16(pObjectData);
		pObjectData+=2;

		if(g_gameId >= JACK)
		{
			ListWorldObjets[i].mark = READ_LE_U16(pObjectData);
			pObjectData+=2;
		}
		ListWorldObjets[i].flags |= 0x20;
	}

	free(pObjectDataBackup);

	vars = (s16*)loadFromItd("VARS.ITD");

	varSize = fileSize;

	if(g_gameId == AITD1)
	{
		choosePersoBackup = CVars[getCVarsIdx(CHOOSE_PERSO)]; // backup hero selection
	}

	char definestPath[512];
	strcpy(definestPath, homePath);
	strcat(definestPath, "DEFINES.ITD");	

	fHandle = fopen(definestPath,"rb");
	if(!fHandle)
	{
		fatalError(0,"DEFINES.ITD");
	}

	///////////////////////////////////////////////
	{
        fread(&CVars[0], CVars.size(), 2, fHandle);
        fclose(fHandle);

		for(i=0;i< CVars.size();i++)
		{
			CVars[i] = ((CVars[i]&0xFF)<<8) | ((CVars[i]&0xFF00)>>8);
		}
	}
	//////////////////////////////////////////////

	if(g_gameId == AITD1)
	{
		CVars[getCVarsIdx(CHOOSE_PERSO)] = choosePersoBackup;
	}

	listLife = HQR_InitRessource<char>("LISTLIFE", 65000, 100);
	listTrack = HQR_InitRessource<char>("LISTTRAK", 20000, 100);
    if (g_gameId >= JACK) {
        HQ_Hybrides = HQR_InitRessource<sHybrid>("LISTHYB", 20000, 10); // TODO: recheck size for other games
    }

	// TODO: missing dos memory check here

	if(g_gameId == AITD1)
	{
        HQ_Bodys = HQR_InitRessource<sBody>(listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]], 37000, 50); // was calculated from free mem size
        HQ_Anims = HQR_InitRessource<sAnimation>(listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]], 30000, 80); // was calculated from free mem size
	}
	else
	{
		HQ_Bodys = HQR_InitRessource<sBody>("LISTBODY", 37000, 50); // was calculated from free mem size
        HQ_Anims = HQR_InitRessource<sAnimation>("LISTANIM",30000, 80); // was calculated from free mem size

		HQ_Matrices = HQR_InitRessource<char>("LISTMAT",64000,5);
	}


	for(i=0;i<NUM_MAX_OBJECT;i++)
	{
		ListObjets[i].indexInWorld = -1;
	}

	if(g_gameId == AITD1)
	{
		currentWorldTarget = CVars[getCVarsIdx(WORLD_NUM_PERSO)];
	}
}

void initVarsSub1(void)
{
	int i;

	for(i=0;i<5;i++)
	{
		messageTable[i].string = NULL;
	}
}

void initVars()
{
	FlagGameOver = 0;

	currentInventory = 0;

	for(int i=0;i<NUM_MAX_INVENTORY;i++)
	{
		numObjInInventoryTable[i] = 0;
		inHandTable[i] = -1;
	}

	action = 0;

	genVar1 = genVar2;
	genVar3 = genVar4;

	NbPhysBoxs = 0;
	NbLogBoxs = 0;

	LastSample = -1;
	nextSample = -1;
	LastPriority = -1;
	currentMusic = -1;
	nextMusic = -1;

	lightOff = 0;
	lightVar2 = 0;

	currentCameraTargetActor = -1;
	currentWorldTarget = -1;

	statusScreenAllowed = 1;

	initVarsSub1();
}

void loadCamera(int cameraIdx)
{
	char name[16];
	int useSpecial = -1;

	sprintf(name,"CAMERA%02d",g_currentFloor);
	//strcat(name,".PAK");

	if(g_gameId == AITD1)
	{
		if(CVars[getCVarsIdx(KILLED_SORCERER)]==1)
		{
			switch(g_currentFloor)
			{
			case 6:
				{
					if(cameraIdx == 0)
					{
						useSpecial = AITD1_CAM06000;
					}
					if(cameraIdx == 5)
					{
						useSpecial = AITD1_CAM06005;
					}
					if(cameraIdx == 8)
					{
						useSpecial = AITD1_CAM06008;
					}
					break;
				}
			case 7:
				{
					if(cameraIdx == 0)
					{
						useSpecial = AITD1_CAM07000;
					}
					if(cameraIdx == 1)
					{
						useSpecial = AITD1_CAM07001;
					}
					break;
				}
			}
		}

		if(useSpecial != -1)
		{
			strcpy(name,"ITD_RESS");
			cameraIdx = useSpecial;
		}
	}

	if(!LoadPak(name,cameraIdx,aux))
	{
		fatalError(0,name);
	}

	if(g_gameId == AITD3)
	{
		//memmove(aux,aux+4,64000+0x300);
	}

	if(g_gameId >= JACK)
	{
		copyPalette(aux+64000,currentGamePalette);

		if(g_gameId == AITD3)
		{
            for (int i = 0; i < 16; i++) {
                currentGamePalette[i][0] = defaultPaletteAITD3[i * 3 + 0];
                currentGamePalette[i][1] = defaultPaletteAITD3[i * 3 + 1];
                currentGamePalette[i][2] = defaultPaletteAITD3[i * 3 + 2];
            }
		}
		else
		{
            for (int i = 0; i < 16; i++) {
                currentGamePalette[i][0] = defaultPalette[i * 3 + 0];
                currentGamePalette[i][1] = defaultPalette[i * 3 + 1];
                currentGamePalette[i][2] = defaultPalette[i * 3 + 2];
            }
		}
        convertPaletteIfRequired(currentGamePalette);

		osystem_setPalette(&currentGamePalette);
	}
}

struct sMaskStruct
{
	u16 x1;
	u16 y1;
	u16 x2;
	u16 y2;
	u16 deltaX;
	u16 deltaY;

	std::array<u8, 320*200> mask;
};

std::vector<std::vector<sMaskStruct>> g_maskBuffers;

void loadMask(int cameraIdx)
{
    if (g_gameId == TIMEGATE)
        return;

	char name[16];

    sprintf(name, "MASK%02d", g_currentFloor);

	if(g_MaskPtr)
	{
		free(g_MaskPtr);
	}

	g_MaskPtr = (unsigned char*)loadPak(name,cameraIdx);

    g_maskBuffers.clear();
    g_maskBuffers.resize(cameraDataTable[NumCamera]->numViewedRooms);
	for(int i=0; i<cameraDataTable[NumCamera]->numViewedRooms; i++)
	{
		cameraViewedRoomStruct* pRoomView = &cameraDataTable[NumCamera]->viewedRoomTable[i];
		unsigned char* pViewedRoomMask = g_MaskPtr + READ_LE_U32(g_MaskPtr + i*4);

        g_maskBuffers[i].reserve(pRoomView->masks.size());
		for(int j=0; j<pRoomView->masks.size(); j++)
		{
            sMaskStruct* pDestMask = &g_maskBuffers[i].emplace_back();
			unsigned char* pMaskData = pViewedRoomMask + READ_LE_U32(pViewedRoomMask + j*4);

            pDestMask->mask.fill(0);

			pDestMask->x1 = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->y1 = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->x2 = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->y2 = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->deltaX = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->deltaY = READ_LE_U16(pMaskData);
			pMaskData += 2;

			assert(pDestMask->deltaX == pDestMask->x2 - pDestMask->x1 + 1);
			assert(pDestMask->deltaY == pDestMask->y2 - pDestMask->y1 + 1);

			for(int k=0; k<pDestMask->deltaY; k++)
			{
				u16 uNumEntryForLine = READ_LE_U16(pMaskData);
				pMaskData += 2;

				unsigned char* pSourceBuffer = (unsigned char*)aux;

				int offset = pDestMask->x1 + pDestMask->y1 * 320 + k * 320;

				for(int l=0; l<uNumEntryForLine; l++)
				{
					unsigned char uNumSkip = *(pMaskData++);
					unsigned char uNumCopy = *(pMaskData++);

					offset += uNumSkip;

					for(int m=0; m<uNumCopy; m++)
					{						
                        pDestMask->mask[offset] = 0xFF;
						offset++;
					}
				}
			}

			osystem_createMask(pDestMask->mask, i, j, pDestMask->x1, pDestMask->y1, pDestMask->x2, pDestMask->y2);
		}
	}
}

void fillpoly(s16 * datas, int n, unsigned char c);
extern unsigned char* polyBackBuffer;

void createAITD1Mask()
{
	for(int viewedRoomIdx=0; viewedRoomIdx<cameraDataTable[NumCamera]->numViewedRooms; viewedRoomIdx++)
	{
		cameraViewedRoomStruct* pcameraViewedRoomData = &cameraDataTable[NumCamera]->viewedRoomTable[viewedRoomIdx];

		char* data2 = room_PtrCamera[NumCamera] + pcameraViewedRoomData->offsetToMask;
		char* data = data2;
		data+=2;

		int numMask = *(s16*)(data2);

		for(int maskIdx=0;maskIdx<numMask;maskIdx++)
		{
            sMaskStruct theMaskStruct;
            sMaskStruct* pDestMask = &theMaskStruct;
            pDestMask->mask.fill(0);
            polyBackBuffer = pDestMask->mask.data();

			int numMaskZone = READ_LE_U16(data);
			char* src = data2 + READ_LE_U16(data+2);

            int minX = 319;
            int maxX = 0;
            int minY = 199;
            int maxY = 0;

			/*if(isBgOverlayRequired( actorPtr->zv.ZVX1 / 10, actorPtr->zv.ZVX2 / 10,
			actorPtr->zv.ZVZ1 / 10, actorPtr->zv.ZVZ2 / 10,
			data+4,
			*(s16*)(data) ))*/
			{
				int numMaskPoly = READ_LE_U16(src);
				src += 2;

				for(int maskPolyIdx=0;maskPolyIdx<numMaskPoly;maskPolyIdx++)
				{
					int numPoints = READ_LE_U16(src);
					src+=2;

					memcpy(cameraBuffer, src, numPoints*4);

					fillpoly((short*)src, numPoints, 0xFF);

                    for (int verticeId = 0; verticeId <numPoints; verticeId++)
                    {
                        short verticeX = *(short*)(src + verticeId * 4 + 0);
                        short verticeY = *(short*)(src + verticeId * 4 + 2);

                        minX = std::min<int>(minX, verticeX);
                        minY = std::min<int>(minY, verticeY);
                        maxX = std::max<int>(maxX, verticeX);
                        maxY = std::max<int>(maxY, verticeY);
                    }

					src+=numPoints*4;
					//drawBgOverlaySub2(param);
				}

				//      blitOverlay(src);

                polyBackBuffer = nullptr;

			}

			osystem_createMask(pDestMask->mask, viewedRoomIdx, maskIdx, minX-1, minY-1, maxX+1, maxY+1);

			int numOverlay = READ_LE_U16(data);
			data+=2;
			data+=((numOverlay*4)+1)*2;
		}

		/*		unsigned char* pViewedRoomMask = g_MaskPtr + READ_LE_U32(g_MaskPtr + i*4);

		for(int j=0; j<pRoomView->numMask; j++)
		{
		unsigned char* pMaskData = pViewedRoomMask + READ_LE_U32(pViewedRoomMask + j*4);

		maskStruct* pDestMask = &g_maskBuffers[i][j];

		memset(pDestMask->mask, 0, 320*200);

		pDestMask->x1 = READ_LE_U16(pMaskData);
		pMaskData += 2;
		pDestMask->y1 = READ_LE_U16(pMaskData);
		pMaskData += 2;
		pDestMask->x2 = READ_LE_U16(pMaskData);
		pMaskData += 2;
		pDestMask->y2 = READ_LE_U16(pMaskData);
		pMaskData += 2;
		pDestMask->deltaX = READ_LE_U16(pMaskData);
		pMaskData += 2;
		pDestMask->deltaY = READ_LE_U16(pMaskData);
		pMaskData += 2;

		assert(pDestMask->deltaX == pDestMask->x2 - pDestMask->x1 + 1);
		assert(pDestMask->deltaY == pDestMask->y2 - pDestMask->y1 + 1);

		for(int k=0; k<pDestMask->deltaY; k++)
		{
		u16 uNumEntryForLine = READ_LE_U16(pMaskData);
		pMaskData += 2;

		unsigned char* pDestBuffer = pDestMask->mask;
		unsigned char* pSourceBuffer = (unsigned char*)aux;

		int offset = pDestMask->x1 + pDestMask->y1 * 320 + k * 320;

		for(int l=0; l<uNumEntryForLine; l++)
		{
		unsigned char uNumSkip = *(pMaskData++);
		unsigned char uNumCopy = *(pMaskData++);

		offset += uNumSkip;

		for(int m=0; m<uNumCopy; m++)
		{						
		pDestBuffer[offset] = 0xFF;
		offset++;
		}
		}
		}

		osystem_createMask(pDestMask->mask, i, j, (unsigned char*)aux, pDestMask->x1, pDestMask->y1, pDestMask->x2, pDestMask->y2);
		}*/
	}

	polyBackBuffer = NULL;
}


void SetAngleCamera(int x, int y, int z)
{
	transformX = x&0x3FF;
	if(transformX)
	{
		transformXCos = cosTable[transformX];
		transformXSin = cosTable[(transformX+0x100)&0x3FF];
		transformUseX = true;
	}
	else
	{
		transformUseX = false;
	}

	transformY = y&0x3FF;
	if(transformY)
	{
		transformYCos = cosTable[transformY];
		transformYSin = cosTable[(transformY+0x100)&0x3FF];
		transformUseY = true;
	}
	else
	{
		transformUseY = false;
	}

	transformZ = z&0x3FF;
	if(transformZ)
	{
		transformZCos = cosTable[transformZ];
		transformZSin = cosTable[(transformZ+0x100)&0x3FF];
		transformUseZ = true;
	}
	else
	{
		transformUseZ = false;
	}
}

void SetPosCamera(int x, int y, int z)
{
	translateX = x;
	translateY = y;
	translateZ = z;
}

void SetProjection(int centerX, int centerY, int x, int y, int z)
{
	cameraCenterX = centerX;
	cameraCenterY = centerY;

	cameraPerspective = x;
	cameraFovX = y;
	cameraFovY = z;
}

int isInViewList(int value)
{
	char* ptr = currentCameraVisibilityList;
	int var;

	while((var=*(ptr++))!=-1)
	{
		if(value == var)
		{
			return(1);
		}
	}

	return(0);
}

// setup visibility list
void InitViewedRoomList()
{
	u32 i;
	int j;
	int var_10;

	char* dataTabPos = currentCameraVisibilityList;

	*dataTabPos = -1;

	// visibility list: add linked rooms
	for(i=0;i<roomDataTable[currentRoom].numSceZone;i++)
	{
		if(roomDataTable[currentRoom].sceZoneTable[i].type == 0)
		{
			var_10 = roomDataTable[currentRoom].sceZoneTable[i].parameter;
			if(!isInViewList(var_10))
			{
				*(dataTabPos++) = var_10;
				*(dataTabPos) = -1;
			}
		}
	}

	// visibility list: add room seen by the current camera
	for(j=0;j<cameraDataTable[NumCamera]->numViewedRooms;j++)
	{
		if(!isInViewList(cameraDataTable[NumCamera]->viewedRoomTable[j].viewedRoomIdx))
		{
			*(dataTabPos++) = (char)cameraDataTable[NumCamera]->viewedRoomTable[j].viewedRoomIdx;
			*(dataTabPos) = -1;
		}
	}
}

void DeleteObjet(int index) // remove actor
{
	tObject *actorPtr = &ListObjets[index];

	if(actorPtr->indexInWorld == -2) // flow
	{
		actorPtr->indexInWorld = -1;

		if(actorPtr->ANIM == 4 )
		{
			CVars[getCVarsIdx(FOG_FLAG)] = 0;
		}

		HQ_Free_Malloc(HQ_Memory,actorPtr->frame);
	}
	else
	{
		if(actorPtr->indexInWorld >= 0)
		{
			tWorldObject* objectPtr = &ListWorldObjets[actorPtr->indexInWorld];

			objectPtr->objIndex = -1;
			actorPtr->indexInWorld = -1;

			objectPtr->body = actorPtr->bodyNum;
			objectPtr->anim = actorPtr->ANIM;
			objectPtr->frame = actorPtr->frame;
			objectPtr->animType = actorPtr->animType;
			objectPtr->animInfo = actorPtr->animInfo;
			objectPtr->flags = actorPtr->objectType & ~AF_BOXIFY;
			objectPtr->flags |= AF_SPECIAL * actorPtr->dynFlags; // ugly hack, need rewrite
			objectPtr->life = actorPtr->life;
			objectPtr->lifeMode = actorPtr->lifeMode;
			objectPtr->trackMode = actorPtr->trackMode;

			if(objectPtr->trackMode)
			{
				objectPtr->trackNumber = actorPtr->trackNumber;
				objectPtr->positionInTrack = actorPtr->positionInTrack;
				if(g_gameId != AITD1)
				{
					objectPtr->mark = actorPtr->MARK;
				}
			}

			objectPtr->x = actorPtr->roomX + actorPtr->stepX;
			objectPtr->y = actorPtr->roomY + actorPtr->stepY;
			objectPtr->z = actorPtr->roomZ + actorPtr->stepZ;

			objectPtr->alpha = actorPtr->alpha;
			objectPtr->beta = actorPtr->beta;
			objectPtr->gamma = actorPtr->gamma;

			objectPtr->stage = actorPtr->stage;
			objectPtr->room = actorPtr->room;

			FlagGenereAffList = 1;
		}
	}
}

bool pointRotateEnable = true;

int pointRotateCosX;
int pointRotateSinX;
int pointRotateCosY;
int pointRotateSinY;
int pointRotateCosZ;
int pointRotateSinZ;

void setupPointRotate(int alpha, int beta, int gamma)
{
	pointRotateEnable = true;

	pointRotateCosX = cosTable[alpha&0x3FF];
	pointRotateSinX = cosTable[((alpha&0x3FF) + 0x100) & 0x3FF];

	pointRotateCosY = cosTable[beta&0x3FF];
	pointRotateSinY = cosTable[((beta&0x3FF) + 0x100) & 0x3FF];

	pointRotateCosZ = cosTable[gamma&0x3FF];
	pointRotateSinZ = cosTable[((gamma&0x3FF) + 0x100) & 0x3FF];
}

void pointRotate(int x, int y, int z, int* destX, int* destY, int* destZ)
{
	if(pointRotateEnable)
	{
		{
			int tempX = x;
			int tempY = y;
			x = ((((tempX * pointRotateSinZ) - ( tempY * pointRotateCosZ)))>>16)<<1;
			y = ((((tempX * pointRotateCosZ) + ( tempY * pointRotateSinZ)))>>16)<<1;
		}

		{
			int tempX = x;
			int tempZ = z;

			x = ((((tempX * pointRotateSinY) - (tempZ * pointRotateCosY)))>>16)<<1;
			z = ((((tempX * pointRotateCosY) + (tempZ * pointRotateSinY)))>>16)<<1;
		}

		{
			int tempY = y;
			int tempZ = z;
			y = ((((tempY * pointRotateSinX ) - (tempZ * pointRotateCosX)))>>16)<<1;
			z = ((((tempY * pointRotateCosX ) + (tempZ * pointRotateSinX)))>>16)<<1;
		}

		*destX = x;
		*destY = y;
		*destZ = z;
	}
}

void CopyZV(ZVStruct* source, ZVStruct* dest)
{
	memcpy(dest,source,sizeof(ZVStruct));
}

void RefreshAux2(void)
{
	FastCopyScreen(aux,aux2);

	//TODO: implementer la suite
}

void InitDeplacement(int trackMode, int trackNumber)
{
	currentProcessedActorPtr->trackMode = trackMode;

	switch(trackMode)
	{
	case 2:
		{
			currentProcessedActorPtr->trackNumber = trackNumber;
			currentProcessedActorPtr->MARK = -1;
			break;
		}
	case 3:
		{
			currentProcessedActorPtr->trackNumber = trackNumber;
			currentProcessedActorPtr->positionInTrack = 0;
			currentProcessedActorPtr->MARK = -1;
			break;
		}
	}
}

s16 ObjSalleRel = 0;

int IsInCamera(int roomNumber)
{
	int numZone = cameraDataTable[NumCamera]->numViewedRooms;

	for(int i=0;i<numZone;i++)
	{
		if(cameraDataTable[NumCamera]->viewedRoomTable[i].viewedRoomIdx == roomNumber)
		{
			ObjSalleRel = i;
			return(1);
		}
	}

	ObjSalleRel = -1;

	return 0;
}

int IsInCamRectTestAITD2(int X, int Z) // TODO: not 100% exact
{
	// if(changeCameraSub1(X,X,Z,Z,&cameraDataTable[currentCamera]->cameraZoneDefTable[cameraVisibilityVar]))
	return 1;

	return 0;
}

void updateAllActorAndObjectsAITD2()
{
    for (int i = 0; i < NUM_MAX_OBJECT; i++)
    {
        tObject* pObject = &ListObjets[i];

        if (pObject->indexInWorld == -1)
        {
            continue;
        }

        pObject->lifeMode &= ~4;

        if (pObject->stage == g_currentFloor)
        {
            switch (pObject->lifeMode)
            {
            case 0: // OFF
                break;
            case 1: // STAGE
                continue;
            case 2: // ROOM
                if (pObject->room == currentRoom)
                {
                    continue;
                }
                break;
            case 3: // CAMERA
                if (isInViewList(pObject->room))
                {
                    continue;
                }
                break;
            default:
                break;
            }

            if (pObject->bodyNum != -1)
            {
                if (IsInCamera(pObject->room))
                {
                    if (IsInCamRectTestAITD2(pObject->roomX + pObject->stepX, pObject->roomZ + pObject->stepZ))
                    {
                        pObject->lifeMode |= 4;
                        continue;
                    }
                }
            }
        }
        DeleteObjet(i);
    }

    for (int i = 0; i < maxObjects; i++)
    {
        tWorldObject* currentObject = &ListWorldObjets[i];

        if (currentObject->objIndex != -1)
        {
            if (currentWorldTarget == i)
            {
                currentCameraTargetActor = currentObject->objIndex;
            }
            continue;
        }

        if (currentObject->stage != g_currentFloor)
            continue;

        bool isOk = false;
        switch (currentObject->lifeMode & 3)
        {
        case 0: // OFF
            isOk = false;
            break;
        case 1: // STAGE
            isOk = true;
            break;
        case 3: // CAMERA
            if (!isInViewList(currentObject->room))
            {
                isOk = false;
            }
            else
            {
                isOk = true;
            }
            break;
        case 2: // ROOM
            if (currentObject->room != currentRoom)
            {
                isOk = false;
            }
            else
            {
                isOk = true;
            }
            break;
        }

        if (!isOk) {
            if ((currentObject->body == -1) || (!IsInCamera(currentObject->room)) || (!IsInCamRectTestAITD2(currentObject->x, currentObject->z)))
            {
                continue;
            }
            else {
                currentObject->lifeMode |= 4;
            }
        }

        currentObject->objIndex = InitObjet(currentObject->body, currentObject->typeZV, currentObject->foundName,
            currentObject->flags & ~AF_SPECIAL,
            currentObject->x, currentObject->y, currentObject->z,
            currentObject->stage, currentObject->room,
            currentObject->alpha, currentObject->beta, currentObject->gamma,
            currentObject->anim,
            currentObject->frame, currentObject->animType, currentObject->animInfo);

        if (currentObject->objIndex != -1)
        {
            currentProcessedActorPtr = &ListObjets[currentObject->objIndex];
            currentProcessedActorIdx = currentObject->objIndex;

            if (currentWorldTarget == i)
            {
                currentCameraTargetActor = currentProcessedActorIdx;
            }

            currentProcessedActorPtr->dynFlags = (currentObject->flags & 0x20) / 0x20; // recheck
            currentProcessedActorPtr->life = currentObject->life;
            currentProcessedActorPtr->lifeMode = currentObject->lifeMode;

            currentProcessedActorPtr->indexInWorld = i;

            InitDeplacement(currentObject->trackMode, currentObject->trackNumber);

            currentProcessedActorPtr->positionInTrack = currentObject->positionInTrack;

            if (g_gameId != AITD1)
            {
                currentProcessedActorPtr->MARK = currentObject->mark;
            }

            FlagGenereAffList = 1;
        }
    }
    //FlagGenereActiveList = true;
}
					

void GenereActiveList()
{
	if(g_gameId > JACK)
	{
		updateAllActorAndObjectsAITD2();
		return;
	}

	for(int i=0;i<NUM_MAX_OBJECT;i++)
	{
        tObject* currentActor = &ListObjets[i];
        if (currentActor->indexInWorld == -1)
            continue;

		if(currentActor->stage == g_currentFloor)
		{
			if(currentActor->life != -1)
			{
				switch(currentActor->lifeMode)
				{
				case 0: // STAGE
                    continue;
				case 1: // ROOM
					if(currentActor->room == currentRoom)
					{
                        continue;
					}
					break;
				case 2: // CAMERA
					if(isInViewList(currentActor->room))
					{
                        continue;
					}
					break;
				default:
					{
						break;
					}
				}
			}
			else
			{
				if(isInViewList(currentActor->room))
				{
                    continue;
				}
			}
		}

        DeleteObjet(i);
	}

	for(int i=0;i<maxObjects;i++)
	{
        tWorldObject* currentObject = &ListWorldObjets[i];
		if(currentObject->objIndex != -1) {
			if(currentWorldTarget == i)
			{
				currentCameraTargetActor = currentObject->objIndex;
			}
            continue;
		}

        if (currentObject->stage != g_currentFloor) {
            continue;
        }

        if (currentObject->life != -1)
        {
            switch (currentObject->lifeMode)
            {
            case -1:
                continue;

            case 0: // Stage
                break;

            case 1: // Room
                if (currentObject->room != currentRoom)
                {
                    continue;
                }
                break;
            case 2: // Camera
                if (!isInViewList(currentObject->room))
                {
                    continue;
                }
                break;
            }
        }
        else
        {
            if (!isInViewList(currentObject->room)) {
                continue;
            }
        }

        currentObject->objIndex = InitObjet( currentObject->body, currentObject->typeZV, currentObject->foundName,
				currentObject->flags & ~AF_SPECIAL,
				currentObject->x, currentObject->y, currentObject->z,
				currentObject->stage, currentObject->room,
				currentObject->alpha, currentObject->beta, currentObject->gamma,
				currentObject->anim,
				currentObject->frame, currentObject->animType, currentObject->animInfo);

		if(currentObject->objIndex != -1)
		{
			currentProcessedActorPtr = &ListObjets[currentObject->objIndex];
			currentProcessedActorIdx = currentObject->objIndex;

			if(currentWorldTarget == i)
			{
				currentCameraTargetActor = currentProcessedActorIdx;
			}

			currentProcessedActorPtr->dynFlags = (currentObject->flags & 0x20) / 0x20; // recheck
			currentProcessedActorPtr->life = currentObject->life;
			currentProcessedActorPtr->lifeMode = currentObject->lifeMode;

			currentProcessedActorPtr->indexInWorld = i;

			InitDeplacement(currentObject->trackMode, currentObject->trackNumber);

			currentProcessedActorPtr->positionInTrack = currentObject->positionInTrack;

			FlagGenereAffList = 1;
		}
	}

	//  FlagGenereActiveList = 0;

	//TODO: object update
}

int checkActorInRoom(int room)
{
	int i;

	for(i=0;i<cameraDataTable[NumCamera]->numViewedRooms;i++)
	{
		if(cameraDataTable[NumCamera]->viewedRoomTable[i].viewedRoomIdx == room)
		{
			return(1);
		}
	}

	return(0);
}

void GenereAffList()
{
	NbAffObjets = 0;

	for(int i=0;i< ListObjets.size();i++)
	{
        tObject* actorPtr = &ListObjets[i];
		if(actorPtr->indexInWorld != -1 && actorPtr->bodyNum != -1)
		{
			if(checkActorInRoom(actorPtr->room))
			{
				Index[NbAffObjets] = i;
				if(!(actorPtr->objectType & (AF_SPECIAL & AF_ANIMATED)))
				{
					actorPtr->objectType |= AF_BOXIFY;
					//  FlagRefreshAux2 = 1;
				}
				NbAffObjets++;
			}
		}
	}
}

void InitView()
{
	int x;
	int y;
	int z;

	SaveTimerAnim();

    resetAnim2D();

	NumCamera = NewNumCamera;

	assert(NewNumCamera < roomDataTable[currentRoom].numCameraInRoom);

	loadCamera(roomDataTable[currentRoom].cameraIdxTable[NewNumCamera]);
	if(g_gameId >= JACK)
	{  
		loadMask(roomDataTable[currentRoom].cameraIdxTable[NewNumCamera]);
	}
	else
	{
		createAITD1Mask();
	}
	cameraBackgroundChanged = true;

    if (g_gameId >= JACK) {
        load2dAnims(roomDataTable[currentRoom].cameraIdxTable[NewNumCamera]);
    }

    cameraDataStruct* pCamera = cameraDataTable[NumCamera];

	SetAngleCamera(pCamera->alpha,pCamera->beta,pCamera->gamma);

#ifdef FITD_DEBUGGER
	if(debuggerVar_topCamera)
		SetAngleCamera(0x100,0,0);
#endif

	x = (pCamera->x - roomDataTable[currentRoom].worldX)*10;
	y = (roomDataTable[currentRoom].worldY - pCamera->y)*10;
	z = (roomDataTable[currentRoom].worldZ - pCamera->z)*10;

#ifdef FITD_DEBUGGER
	if(debuggerVar_topCamera)
	{
		if(currentCameraTargetActor != -1)
		{
			x = ListObjets[currentCameraTargetActor].worldX + ListObjets[currentCameraTargetActor].stepX;
			y = debufferVar_topCameraZoom;
			z = ListObjets[currentCameraTargetActor].worldZ + ListObjets[currentCameraTargetActor].stepZ;
		}
	}
#endif
	SetPosCamera(x,y,z); // setup camera position

	SetProjection(160,100,pCamera->focal1,pCamera->focal2,pCamera->focal3); // setup projection

#ifdef FITD_DEBUGGER
	if(debuggerVar_topCamera)
		SetProjection(160,100,1000,100,100);
#endif

	InitViewedRoomList();
	GenereActiveList();
	GenereAffList();
	//  TriListObjet();
	RefreshAux2();
	/*  InitScreenCoors();
	*/
	if(FlagInitView==2)
	{
		flagRedraw = 2;
	}
	else
	{
		if(flagRedraw!=2)
		{
			flagRedraw = 1;
		}
	}

	FlagInitView = 0;
	RestoreTimerAnim();

    for (int i = 0; i < currentCameraZoneList[NumCamera]->hybrids.size(); i++) {
        startAnim2d(i);
    }
}

s16 GiveDistance2D(int x1, int z1, int x2, int z2)
{
	//int axBackup = x1;
	x1 -= x2;
	if((s16)x1 < 0)
	{
		x1 = -(s16)x1;
	}

	z1 -= z2;
	if((s16)z1 < 0)
	{
		z1 = -(s16)z1;
	}

	if((x1+z1)> 0xFFFF)
	{
		return(0x7D00);
	}
	else
	{
		return(x1+z1);
	}
}

void InitRealValue(s16 startValue, s16 endValue, s16 time, RealValue* realValue)
{
	realValue->startValue = startValue;
	realValue->endValue = endValue;
	realValue->numSteps = time;
	realValue->memoTicks = timer;
}

s16 updateActorRotation(RealValue* rotatePtr)
{
	int timeDif;
	int angleDif;

	if(!rotatePtr->numSteps)
		return(rotatePtr->endValue);

	timeDif = timer - rotatePtr->memoTicks;

	if(timeDif>rotatePtr->numSteps)
	{
		rotatePtr->numSteps = 0;
		return(rotatePtr->endValue);
	}

	angleDif = (rotatePtr->endValue&0x3FF) - (rotatePtr->startValue&0x3FF);

	if(angleDif<=0x200)
	{
		if(angleDif>=-0x200)
		{
			int angle = (rotatePtr->endValue&0x3FF) - (rotatePtr->startValue&0x3FF);
			return (rotatePtr->startValue&0x3FF) + (angle*timeDif)/rotatePtr->numSteps;
		}
		else
		{
			s16 angle = ((rotatePtr->endValue&0x3FF)+0x400) - ((rotatePtr->startValue&0x3FF));
			return (((rotatePtr->startValue&0x3FF)) + ((angle*timeDif)/rotatePtr->numSteps));
		}
	}
	else
	{
		int angle = (rotatePtr->endValue&0x3FF) - ((rotatePtr->startValue&0x3FF)+0x400);
		return ((angle*timeDif)/rotatePtr->numSteps) + ((rotatePtr->startValue&0x3FF));
	}
}

void removeFromBGIncrust(int actorIdx)
{
	tObject* actorPtr = &ListObjets[actorIdx];

	actorPtr->objectType &= ~AF_BOXIFY;

	//  FlagRefreshAux2 = 1;

	BBox3D1 = actorPtr->screenXMin;

	if(BBox3D1 > -1)
	{
		BBox3D2 = actorPtr->screenYMin;
		BBox3D3 = actorPtr->screenXMax;
		BBox3D4 = actorPtr->screenYMax;

		//deleteSubSub();
	}
}

int findObjectInInventory(int objIdx)
{
	for(int i=0;i<numObjInInventoryTable[currentInventory];i++)
	{
		if(inventoryTable[currentInventory][i] == objIdx)
		{
			return(i);
		}
	}

	return(-1);
}

void DeleteInventoryObjet(int objIdx)
{
	int inventoryIdx;

	inventoryIdx = findObjectInInventory(objIdx);

	if(inventoryIdx != -1)
	{
		memmove(&inventoryTable[currentInventory][inventoryIdx],&inventoryTable[currentInventory][inventoryIdx+1],(30-inventoryIdx-1)*2);

		numObjInInventoryTable[currentInventory]--;
	}

	ListWorldObjets[objIdx].foundFlag &= 0x7FFF;
}

void deleteObject(int objIdx)
{
	tWorldObject* objPtr;
	int actorIdx;
	tObject* actorPtr;

	objPtr = &ListWorldObjets[objIdx];
	actorIdx = objPtr->objIndex;

	if(actorIdx != -1)
	{
		actorPtr = &ListObjets[actorIdx];

		actorPtr->room = -1;
		actorPtr->stage = -1;

		//    FlagGenereActiveList = 1;

		if(actorPtr->objectType & AF_BOXIFY)
		{
			removeFromBGIncrust(actorIdx);
		}
	}

	objPtr->room = -1;
	objPtr->stage = -1;

	DeleteInventoryObjet(objIdx);
}

#ifdef FITD_DEBUGGER
void line(int x1, int y1, int x2, int y2, char c);

void drawProjectedLine(s32 x1s, s32 y1s, s32 z1s, s32 x2s, s32 y2s, s32 z2s,int c)
{
	float x1 = (float)x1s;
	float x2 = (float)x2s;
	float y1 = (float)y1s;
	float y2 = (float)y2s;
	float z1 = (float)z1s;
	float z2 = (float)z2s;

	float transformedX1;
	float transformedX2;

	float transformedY1;
	float transformedY2;

	x1 -= translateX;
	x2 -= translateX;

#if !defined(AITD_UE4)
	y1 -= translateY;
	y2 -= translateY;
#endif

	z1 -= translateZ;
	z2 -= translateZ;

#if defined(AITD_UE4)
    osystem_draw3dLine(x1, y1, z1, x2, y2, z2, c);
#else
	transformPoint(&x1,&y1,&z1);
	transformPoint(&x2,&y2,&z2);

	z1 += cameraPerspective;
	z2 += cameraPerspective;

	transformedX1 = ((x1 * cameraFovX) / (float)z1) + cameraCenterX;
	transformedX2 = ((x2 * cameraFovX) / (float)z2) + cameraCenterX;

	transformedY1 = ((y1 * cameraFovY) / (float)z1) + cameraCenterY;
	transformedY2 = ((y2 * cameraFovY) / (float)z2) + cameraCenterY;

	if(z1>0 && z2>0)
		osystem_draw3dLine(transformedX1,transformedY1,z1,transformedX2,transformedY2,z2,c);
#endif
}

void drawZv(const ZVStruct& localZv) {
    // bottom
    drawProjectedLine(localZv.ZVX1, localZv.ZVY2, localZv.ZVZ1, localZv.ZVX1, localZv.ZVY2, localZv.ZVZ2, 10);
    drawProjectedLine(localZv.ZVX1, localZv.ZVY2, localZv.ZVZ2, localZv.ZVX2, localZv.ZVY2, localZv.ZVZ2, 10);
    drawProjectedLine(localZv.ZVX2, localZv.ZVY2, localZv.ZVZ2, localZv.ZVX2, localZv.ZVY2, localZv.ZVZ1, 10);
    drawProjectedLine(localZv.ZVX2, localZv.ZVY2, localZv.ZVZ1, localZv.ZVX1, localZv.ZVY2, localZv.ZVZ1, 10);

    // top
    drawProjectedLine(localZv.ZVX1, localZv.ZVY1, localZv.ZVZ1, localZv.ZVX1, localZv.ZVY1, localZv.ZVZ2, 10);
    drawProjectedLine(localZv.ZVX1, localZv.ZVY1, localZv.ZVZ2, localZv.ZVX2, localZv.ZVY1, localZv.ZVZ2, 10);
    drawProjectedLine(localZv.ZVX2, localZv.ZVY1, localZv.ZVZ2, localZv.ZVX2, localZv.ZVY1, localZv.ZVZ1, 10);
    drawProjectedLine(localZv.ZVX2, localZv.ZVY1, localZv.ZVZ1, localZv.ZVX1, localZv.ZVY1, localZv.ZVZ1, 10);

    drawProjectedLine(localZv.ZVX1, localZv.ZVY2, localZv.ZVZ1, localZv.ZVX1, localZv.ZVY1, localZv.ZVZ1, 10);
    drawProjectedLine(localZv.ZVX1, localZv.ZVY2, localZv.ZVZ2, localZv.ZVX1, localZv.ZVY1, localZv.ZVZ2, 10);
    drawProjectedLine(localZv.ZVX2, localZv.ZVY2, localZv.ZVZ2, localZv.ZVX2, localZv.ZVY1, localZv.ZVZ2, 10);
    drawProjectedLine(localZv.ZVX2, localZv.ZVY2, localZv.ZVZ1, localZv.ZVX2, localZv.ZVY1, localZv.ZVZ1, 10);
}

void drawZv(tObject* actorPtr)
{
	ZVStruct localZv;

	if( actorPtr->room != ListObjets[currentCameraTargetActor].room )
	{
        if (ListObjets[currentCameraTargetActor].room == -1) {
            return;
        }
		AdjustZV(&localZv, actorPtr->room, ListObjets[currentCameraTargetActor].room);
	}
	else
	{
		CopyZV(&actorPtr->zv,&localZv);
	}

    drawZv(localZv);
}

void drawConverZone(cameraZoneEntryStruct* zonePtr)
{
	int i;

	for(i=0;i<zonePtr->numPoints-1;i++)
	{
		drawProjectedLine(zonePtr->pointTable[i].x*10,0,zonePtr->pointTable[i].y*10,zonePtr->pointTable[i+1].x*10,0,zonePtr->pointTable[i+1].y*10,20);
	}

	// loop first and last

	i = zonePtr->numPoints-1;
	drawProjectedLine(zonePtr->pointTable[0].x*10,0,zonePtr->pointTable[0].y*10,zonePtr->pointTable[i].x*10,0,zonePtr->pointTable[i].y*10,20);
}

void drawConverZones()
{
	int i;
	for(i=0;i<numCameraInRoom;i++)
	{
		int j;
		for(j=0;j<cameraDataTable[i]->numViewedRooms;j++)
		{
			int k;

			if(cameraDataTable[i]->viewedRoomTable[j].viewedRoomIdx == currentRoom)
			{
				for(k=0;k<cameraDataTable[i]->viewedRoomTable[j].coverZones.size();k++)
				{
					drawConverZone(&cameraDataTable[i]->viewedRoomTable[j].coverZones[k]);
				}
			}
		}
	}
}

void drawAAQuad(s32 X1, s32 X2, s32 Z1, s32 Z2)
{
	drawProjectedLine(X1, 0, Z1, X1, 0, Z2, 20);
	drawProjectedLine(X1, 0, Z2, X2, 0, Z2, 20);
	drawProjectedLine(X2, 0, Z2, X2, 0, Z1, 20);
	drawProjectedLine(X2, 0, Z1, X1, 0, Z1, 20);
}

void drawMaskZone(cameraMaskStruct* maskZonePtr)
{
	for(int i=0; i<maskZonePtr->numTestRect; i++)
	{
		drawAAQuad(maskZonePtr->rectTests[i].zoneX1*10, maskZonePtr->rectTests[i].zoneX2*10, maskZonePtr->rectTests[i].zoneZ1*10, maskZonePtr->rectTests[i].zoneZ2*10);
	}
}

void drawMaskZones()
{
	{
		cameraDataStruct* pCamera = cameraDataTable[NumCamera];

		for(int j=0;j<pCamera->numViewedRooms;j++)
		{
			int k;

			//if(cameraDataTable[i]->viewedRoomTable[j].viewedRoomIdx == currentDisplayedRoom)
			{
				for(k=0;k<pCamera->viewedRoomTable[j].masks.size();k++)
				{
					drawMaskZone(&pCamera->viewedRoomTable[j].masks[k]);
				}
			}
		}
	}
}

#define DEPTH_THRESHOLD 1000

void drawProjectedQuad(float x1,float x2, float x3, float x4, float y1,float y2, float y3, float y4, float z1,float z2, float z3, float z4, int color, int transprency)
{
	float transformedX1;
	float transformedX2;
	float transformedX3;
	float transformedX4;

	float transformedY1;
	float transformedY2;
	float transformedY3;
	float transformedY4;

	x1 -= translateX;
	x2 -= translateX;
	x3 -= translateX;
	x4 -= translateX;

	y1 -= translateY;
	y2 -= translateY;
	y3 -= translateY;
	y4 -= translateY;

	z1 -= translateZ;
	z2 -= translateZ;
	z3 -= translateZ;
	z4 -= translateZ;

	transformPoint(&x1,&y1,&z1);
	transformPoint(&x2,&y2,&z2);
	transformPoint(&x3,&y3,&z3);
	transformPoint(&x4,&y4,&z4); 

	z1 += cameraPerspective;
	z2 += cameraPerspective;
	z3 += cameraPerspective;
	z4 += cameraPerspective;

	transformedX1 = ((x1 * cameraFovX) / (float)z1) + cameraCenterX;
	transformedX2 = ((x2 * cameraFovX) / (float)z2) + cameraCenterX;
	transformedX3 = ((x3 * cameraFovX) / (float)z3) + cameraCenterX;
	transformedX4 = ((x4 * cameraFovX) / (float)z4) + cameraCenterX;

	transformedY1 = ((y1 * cameraFovY) / (float)z1) + cameraCenterY;
	transformedY2 = ((y2 * cameraFovY) / (float)z2) + cameraCenterY;
	transformedY3 = ((y3 * cameraFovY) / (float)z3) + cameraCenterY;
	transformedY4 = ((y4 * cameraFovY) / (float)z4) + cameraCenterY;

	if(z1>DEPTH_THRESHOLD && z2>DEPTH_THRESHOLD && z3>DEPTH_THRESHOLD && z4>DEPTH_THRESHOLD)
	{
		osystem_draw3dQuad(transformedX1,transformedY1,z1, transformedX2,transformedY2,z2, transformedX3,transformedY3,z3, transformedX4,transformedY4,z4, color, transprency);
	}

	//osystem_draw3dQuad(x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4, color);
}

void drawProjectedBox(int x1,int x2,int y1,int y2,int z1,int z2, int color, int transprency)
{
	//bottom
	drawProjectedQuad((float)x1,(float)x1,(float)x2,(float)x2,(float)y1,(float)y1,(float)y1,(float)y1,(float)z1,(float)z2,(float)z2,(float)z1,color,transprency);
	//top
	drawProjectedQuad((float)x1,(float)x1,(float)x2,(float)x2,(float)y2,(float)y2,(float)y2,(float)y2,(float)z1,(float)z2,(float)z2,(float)z1,color,transprency);
	//left
	drawProjectedQuad((float)x1,(float)x1,(float)x1,(float)x1,(float)y1,(float)y2,(float)y2,(float)y1,(float)z1,(float)z1,(float)z2,(float)z2,color,transprency);
	//right
	drawProjectedQuad((float)x2,(float)x2,(float)x2,(float)x2,(float)y1,(float)y2,(float)y2,(float)y1,(float)z1,(float)z1,(float)z2,(float)z2,color,transprency);
	//front
	drawProjectedQuad((float)x1,(float)x2,(float)x2,(float)x1,(float)y1,(float)y1,(float)y2,(float)y2,(float)z1,(float)z1,(float)z1,(float)z1,color,transprency);
	//back
	drawProjectedQuad((float)x1,(float)x2,(float)x2,(float)x1,(float)y1,(float)y1,(float)y2,(float)y2,(float)z2,(float)z2,(float)z2,(float)z2,color,transprency);
}

void drawRoomZv(ZVStruct* zoneData, int color, int transparency)
{
	ZVStruct cameraZv = {-100,100,-100,100,-100,100};

	cameraZv.ZVX1 += translateX;
	cameraZv.ZVX2 += translateX;

	cameraZv.ZVY1 += translateY;
	cameraZv.ZVY2 += translateY;

	cameraZv.ZVZ1 += translateZ;
	cameraZv.ZVZ2 += translateZ;

	if(CubeIntersect(&cameraZv,zoneData))
	{
		return;
	}

	drawProjectedBox(zoneData->ZVX1,zoneData->ZVX2,zoneData->ZVY1,zoneData->ZVY2,zoneData->ZVZ1,zoneData->ZVZ2,color,transparency);
}

void drawRoomZvLine(ZVStruct* zoneData, int color)
{
	ZVStruct cameraZv = {-100,100,-100,100,-100,100};

	cameraZv.ZVX1 += translateX;
	cameraZv.ZVX2 += translateX;

	cameraZv.ZVY1 += translateY;
	cameraZv.ZVY2 += translateY;

	cameraZv.ZVZ1 += translateZ;
	cameraZv.ZVZ2 += translateZ;

	if(CubeIntersect(&cameraZv,zoneData))
	{
		return;
	}

	drawProjectedLine(zoneData->ZVX1,zoneData->ZVY1,zoneData->ZVZ1,zoneData->ZVX1,zoneData->ZVY1,zoneData->ZVZ2,color);
	drawProjectedLine(zoneData->ZVX1,zoneData->ZVY1,zoneData->ZVZ2,zoneData->ZVX2,zoneData->ZVY1,zoneData->ZVZ2,color);
	drawProjectedLine(zoneData->ZVX2,zoneData->ZVY1,zoneData->ZVZ2,zoneData->ZVX2,zoneData->ZVY1,zoneData->ZVZ1,color);
	drawProjectedLine(zoneData->ZVX2,zoneData->ZVY1,zoneData->ZVZ1,zoneData->ZVX1,zoneData->ZVY1,zoneData->ZVZ1,color);

	drawProjectedLine(zoneData->ZVX1,zoneData->ZVY2,zoneData->ZVZ1,zoneData->ZVX1,zoneData->ZVY2,zoneData->ZVZ2,color);
	drawProjectedLine(zoneData->ZVX1,zoneData->ZVY2,zoneData->ZVZ2,zoneData->ZVX2,zoneData->ZVY2,zoneData->ZVZ2,color);
	drawProjectedLine(zoneData->ZVX2,zoneData->ZVY2,zoneData->ZVZ2,zoneData->ZVX2,zoneData->ZVY2,zoneData->ZVZ1,color);
	drawProjectedLine(zoneData->ZVX2,zoneData->ZVY2,zoneData->ZVZ1,zoneData->ZVX1,zoneData->ZVY2,zoneData->ZVZ1,color);

	drawProjectedLine(zoneData->ZVX1,zoneData->ZVY1,zoneData->ZVZ1,zoneData->ZVX1,zoneData->ZVY2,zoneData->ZVZ1,color);
	drawProjectedLine(zoneData->ZVX1,zoneData->ZVY1,zoneData->ZVZ2,zoneData->ZVX1,zoneData->ZVY2,zoneData->ZVZ2,color);
	drawProjectedLine(zoneData->ZVX2,zoneData->ZVY1,zoneData->ZVZ2,zoneData->ZVX2,zoneData->ZVY2,zoneData->ZVZ2,color);
	drawProjectedLine(zoneData->ZVX2,zoneData->ZVY1,zoneData->ZVZ1,zoneData->ZVX2,zoneData->ZVY2,zoneData->ZVZ1,color);
}

void drawZone(char* zoneData,int color)
{
	int x1;
	int x2;

	int y1;
	int y2;

	int z1;
	int z2;

	int type;

	ZVStruct tempZv;

	ZVStruct cameraZv = {-100,100,-100,100,-100,100};

	type = *(s16*)(zoneData+0xE);

	x1 = *( s16*)(zoneData+0x0);
	x2 = *( s16*)(zoneData+0x2);
	y1 = *( s16*)(zoneData+0x4);
	y2 = *( s16*)(zoneData+0x6);
	z1 = *( s16*)(zoneData+0x8);
	z2 = *( s16*)(zoneData+0xA);

	cameraZv.ZVX1 += translateX;
	cameraZv.ZVX2 += translateX;

	cameraZv.ZVY1 += translateY;
	cameraZv.ZVY2 += translateY;

	cameraZv.ZVZ1 += translateZ;
	cameraZv.ZVZ2 += translateZ;

	tempZv.ZVX1 = READ_LE_S16(zoneData+0x00);
	tempZv.ZVX2 = READ_LE_S16(zoneData+0x02);
	tempZv.ZVY1 = READ_LE_S16(zoneData+0x04);
	tempZv.ZVY2 = READ_LE_S16(zoneData+0x06);
	tempZv.ZVZ1 = READ_LE_S16(zoneData+0x08);
	tempZv.ZVZ2 = READ_LE_S16(zoneData+0x0A);

	if(CubeIntersect(&cameraZv,&tempZv))
	{
		return;
	}

	drawProjectedBox(x1,x2,y1,y2,z1,z2,type,255);
}

void drawOverlayZone(char* zoneData,int color)
{
	int x1;
	int x2;

	int y1;
	int y2;

	int z1;
	int z2;

	x1 = *( s16*)(zoneData+0x0) * 10;
	z1 = *( s16*)(zoneData+0x2) * 10;
	x2 = *( s16*)(zoneData+0x4) * 10;
	z2 = *( s16*)(zoneData+0x6) * 10;

	y1=0;
	y2=0;

	drawProjectedBox(x1,x2,y1,y2,z1,z2,color,255);
}

void drawSceZone(int roomNumber)
{
	u32 i;
	ZVStruct dataLocal;

	for(i=0;i<roomDataTable[roomNumber].numSceZone;i++)
	{
		memcpy(&dataLocal,&roomDataTable[roomNumber].sceZoneTable[i].zv,sizeof(ZVStruct));
		if(roomNumber!=currentRoom)
		{
			AdjustZV(&dataLocal,roomNumber,currentRoom);
		}

//		if(roomDataTable[roomNumber].sceZoneTable[i].parameter == 4)
//			if(roomDataTable[roomNumber].sceZoneTable[i].type)
			{
				drawRoomZv(&dataLocal,20,40);
			}
	}
}

void drawHardCol(int roomNumber)
{
	ZVStruct dataLocal;

	for(u32 i=0;i<roomDataTable[roomNumber].numHardCol;i++)
	{
		/*if(roomDataTable[roomNumber].hardColTable[i].type != 9)
			continue;*/

		CopyZV(&roomDataTable[roomNumber].hardColTable[i].zv,&dataLocal);

		if(roomNumber!=currentRoom)
		{
			AdjustZV(&dataLocal,roomNumber,currentRoom);
		}

		switch(roomDataTable[roomNumber].hardColTable[i].type)
		{
		case 0: // objects
			drawRoomZv(&dataLocal,9,150);
			break;
		case 1: // walls
			drawRoomZv(&dataLocal,100,255);
			break;
		case 2: // dummy
			drawRoomZv(&dataLocal,180,255);
			break;
		case 3: // ground/climb
			drawRoomZv(&dataLocal,70,255);
			break;
		case 4: // over door zones
			drawRoomZv(&dataLocal,50,255);
			break;
		case 9: // scenario
			drawRoomZv(&dataLocal,60,255);
			break;
		case 10: // monsters
			drawRoomZv(&dataLocal,80,255);
			break;
		default:
			drawRoomZv(&dataLocal,40,40);
			break;
		}

	}
}
#endif


int isBgOverlayRequired( int X1, int X2, int Z1, int Z2, char* data, int param )
{
	int i;
	for(i=0;i<param;i++)
	{
		////////////////////////////////////// DEBUG
		//  drawOverlayZone(data, 80);
		/////////////////////////////////////

		int zoneX1 = *(s16*)(data);
		int zoneZ1 = *(s16*)(data+2);
		int zoneX2 = *(s16*)(data+4);
		int zoneZ2 = *(s16*)(data+6);

		if(X1 >= zoneX1 && Z1 >= zoneZ1 && X2 <= zoneX2 && Z2 <= zoneZ2)
		{
			return(1);
		}

		data+=0x8;
	}

	return(0);
}

void drawBgOverlay(tObject* actorPtr)
{
	char* data;
	char* data2;

	int numOverlayZone;

	actorPtr->screenXMin = BBox3D1;
	actorPtr->screenYMin = BBox3D2;
	actorPtr->screenXMax = BBox3D3;
	actorPtr->screenYMax = BBox3D4;

	//if(actorPtr->trackMode != 1)
	//	return;

	SetClip(BBox3D1, BBox3D2, BBox3D3, BBox3D4);

	cameraDataStruct* pCamera = cameraDataTable[NumCamera];

	// look for the correct room data of that camera
	cameraViewedRoomStruct* pcameraViewedRoomData = NULL;
	int relativeCameraIndex = -1;
	for(int i=0; i<pCamera->numViewedRooms; i++)
	{
		if(pCamera->viewedRoomTable[i].viewedRoomIdx == actorPtr->room)
		{
			pcameraViewedRoomData = &pCamera->viewedRoomTable[i];
			relativeCameraIndex = i;
			break;
		}
	}
	if(pcameraViewedRoomData == NULL)
		return;

	if(g_gameId == AITD1)
	{
		data2 = room_PtrCamera[NumCamera] + pcameraViewedRoomData->offsetToMask;
		data = data2;
		data+=2;

		numOverlayZone = *(s16*)(data2);

		for(int i=0;i<numOverlayZone;i++)
		{
			int numOverlay;
			char* src = data2 + *(u16*)(data+2);

			if(isBgOverlayRequired( actorPtr->zv.ZVX1 / 10, actorPtr->zv.ZVX2 / 10,
				actorPtr->zv.ZVZ1 / 10, actorPtr->zv.ZVZ2 / 10,
				data+4,
				*(s16*)(data) ))
			{
				osystem_setClip(clipLeft, clipTop, clipRight, clipBottom);
				osystem_drawMask(relativeCameraIndex, i);
				osystem_clearClip();

				/*
				int j;
				numOverlay = *(s16*)src;
				src += 2;

				for(j=0;j<numOverlay;j++)
				{
				int param = *(s16*)(src);
				src+=2;

				memcpy(cameraBuffer, src, param*4);

				src+=param*4;

				drawBgOverlaySub2(param);
				}
				*/

				//      blitOverlay(src);
			}

			numOverlay = *(s16*)(data);
			data+=2;
			data+=((numOverlay*4)+1)*2;
		}
	}
	else
	{
		for(int i=0;i<pcameraViewedRoomData->masks.size();i++)
		{
			cameraMaskStruct* pMaskZones = &pcameraViewedRoomData->masks[i];

			for(int j=0; j<pMaskZones->numTestRect; j++)
			{
				rectTestStruct* pRect = &pMaskZones->rectTests[j];

				int actorX1 = actorPtr->zv.ZVX1 / 10;
				int actorX2 = actorPtr->zv.ZVX2 / 10;
				int actorZ1 = actorPtr->zv.ZVZ1 / 10;
				int actorZ2 = actorPtr->zv.ZVZ2 / 10;

				if(actorX1 >= pRect->zoneX1 && actorZ1 >= pRect->zoneZ1 && actorX2 <= pRect->zoneX2 && actorZ2 <= pRect->zoneZ2)
				{
					osystem_setClip(clipLeft, clipTop, clipRight, clipBottom);
					osystem_drawMask(relativeCameraIndex, i);
					osystem_clearClip();
					break;
				}
			}
		}
	}

	SetClip(0,0,319,199);
}

void AffSpecialObjet(int actorIdx) // draw flow
{
	//actorStruct* actorPtr = &actorTable[actorIdx];

	//char* data = printTextSub2(hqrUnk, actorPtr->FRAME);

	// TODO: finish
}

void getHotPoint(int hotPointIdx, sBody* bodyPtr, point3dStruct* hotPoint)
{
    hotPoint->x = 0;
    hotPoint->y = 0;
    hotPoint->z = 0;

	if(bodyPtr->m_flags &2)
	{
		ASSERT(hotPointIdx < bodyPtr->m_groups.size());

		if(hotPointIdx < bodyPtr->m_groups.size())
		{
            auto& group = bodyPtr->m_groups[hotPointIdx];

			int pointIdx = group.m_baseVertices; // first point
            *hotPoint = pointBuffer[pointIdx];
		}
	}
}

void AllRedraw(int flagFlip)
{
    uiLayer.fill(0);

	//if(flagFlip == 2)
	{
		if(cameraBackgroundChanged)
		{
			osystem_CopyBlockPhys((unsigned char*)aux,0,0,320,200);
			cameraBackgroundChanged = false;
		}
	}

	if(flagFlip== 0)
	{
		//ClsAuxLog();
	}
	else
	{
		NbPhysBoxs = 0;
		FastCopyScreen(aux2,logicalScreen);
	}

	SetClip(0,0,319,199);
	NbLogBoxs = 0;

	for(int i=0;i<NbAffObjets + NbAnim2D;i++)
	{
		int currentDrawActor = Index[i];
        if (currentDrawActor & 0x8000) {
            // Anim2d
            SetClip(0, 0, 319, 199);
            int num = currentDrawActor & 0x7FFF;
            AffHyb(TabAnim2d[num].pAnim->id, 0, 0, PtrAnim2D);

            // TODO: Clipping stuff
            osystem_CopyBlockPhys((unsigned char*)logicalScreen, 0, 0, 320, 200);
        }
        else {
            tObject* actorPtr = &ListObjets[currentDrawActor];

            // this is commented out to draw actors backed into the background
            //if(actorPtr->_flags & (AF_ANIMATED + AF_DRAWABLE + AF_SPECIAL))
            {
                actorPtr->objectType &= ~AF_DRAWABLE;

                if (actorPtr->objectType & AF_SPECIAL)
                {
                    AffSpecialObjet(currentDrawActor);
                }
                else
                {
                    if (actorPtr->objectType & AF_OBJ_2D) {
                        sHybrid* pHybrid = HQR_Get(HQ_Hybrides, actorPtr->ANIM);
                        s16 numHybrid = pHybrid->animations[actorPtr->bodyNum].anims[actorPtr->frame].id;
                        AffHyb(numHybrid, 0, 0, pHybrid);

                        // TODO: bounding volume
                    }
                    else
                    {
                        sBody* bodyPtr = HQR_Get(HQ_Bodys, actorPtr->bodyNum);

                        if (HQ_Load)
                        {
                            //          initAnimInBody(actorPtr->FRAME, HQR_Get(listAnim, actorPtr->ANIM), bodyPtr);
                        }

                        AffObjet(actorPtr->worldX + actorPtr->stepX, actorPtr->worldY + actorPtr->stepY, actorPtr->worldZ + actorPtr->stepZ, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, bodyPtr);


                        if (actorPtr->animActionType != 0)
                        {
                            if (actorPtr->hotPointID != -1)
                            {
                                getHotPoint(actorPtr->hotPointID, bodyPtr, &actorPtr->hotPoint);
                            }
                        }
                    }
                }

                if (BBox3D1 < 0)
                    BBox3D1 = 0;
                if (BBox3D3 > 319)
                    BBox3D3 = 319;
                if (BBox3D2 < 0)
                    BBox3D2 = 0;
                if (BBox3D4 > 199)
                    BBox3D4 = 199;

                if (BBox3D1 <= 319 && BBox3D2 <= 199 && BBox3D3 >= 0 && BBox3D4 >= 0) // is the character on screen ?
                {
                    if (g_gameId == AITD1)
                    {
                        if (actorPtr->indexInWorld == CVars[getCVarsIdx(LIGHT_OBJECT)])
                        {
                            lightX = (BBox3D3 + BBox3D1) / 2;
                            lightY = (BBox3D4 + BBox3D2) / 2;
                        }
                    }

#ifdef FITD_DEBUGGER
                    if (backgroundMode == backgroundModeEnum_2D)
#endif
                    {
                        //if(g_gameId == AITD1)
                        drawBgOverlay(actorPtr);
                    }
                    //addToRedrawBox();
                }
                else
                {
                    actorPtr->screenYMax = -1;
                    actorPtr->screenXMax = -1;
                    actorPtr->screenYMin = -1;
                    actorPtr->screenXMin = -1;
                }
            }
        }
	}

#ifdef FITD_DEBUGGER
    {
        if (backgroundMode == backgroundModeEnum_3D)
        {
            for (int i = 0; i < NUM_MAX_OBJECT; i++)
            {
                tObject* actorPtr = &ListObjets[i];

                if (actorPtr->indexInWorld != -1)
                {
                    drawZv(actorPtr);
                }
            }
        }

        /////////////////////////////////////
        for (int i = 0; i < getNumberOfRoom(); i++)
        {
            if (hardColDisplayMode != displayNone) {
                drawHardCol(i);
            }

            if (sceColDisplayMode != displayNone) {
                drawSceZone(i);
            }
        }

        //drawConverZones();
        //drawMaskZones();
    }
#endif

	if(drawTextOverlay())
	{
		//addToRedrawBox();
	}

	if(!lightOff)
	{
		if(flagFlip)
		{
			if(flagFlip==2 || lightVar2)
			{
				//makeBlackPalette();
				osystem_flip(NULL);
				FadeInPhys(0x10,0);
				lightVar2 = 0;
			}
			else
			{
				//osystem_flip(NULL);
			}
		}
		else
		{
			//mainDrawSub1();
		}
	}
	else
	{
	}

	//    osystem_stopFrame();

	//	osystem_flip(NULL);

	flagRedraw = 0;
}

void walkStep(int angle1, int angle2, int angle3)
{
	Rotate(angle3,angle1,angle2,&animMoveZ,&animMoveX);
}

void addActorToBgInscrust(int actorIdx)
{
	ListObjets[actorIdx].objectType |= AF_BOXIFY + AF_DRAWABLE;
	ListObjets[actorIdx].objectType &= ~AF_ANIMATED;

	//FlagRefreshAux2 = 1;
}

int CubeIntersect(ZVStruct* zvPtr1,ZVStruct* zvPtr2)
{
	if(zvPtr1->ZVX1 >= zvPtr2->ZVX2)
		return 0;

	if(zvPtr2->ZVX1 >= zvPtr1->ZVX2)
		return 0;

	if(zvPtr1->ZVY1 >= zvPtr2->ZVY2)
		return 0;

	if(zvPtr2->ZVY1 >= zvPtr1->ZVY2)
		return 0;

	if(zvPtr1->ZVZ1 >= zvPtr2->ZVZ2)
		return 0;

	if(zvPtr2->ZVZ1 >= zvPtr1->ZVZ2)
		return 0;

	return 1;
}

void AdjustZV(ZVStruct* zvPtr, int startRoom, int destRoom)
{
	unsigned int Xdif = 10*(roomDataTable[destRoom].worldX - roomDataTable[startRoom].worldX);
	unsigned int Ydif = 10*(roomDataTable[destRoom].worldY - roomDataTable[startRoom].worldY);
	unsigned int Zdif = 10*(roomDataTable[destRoom].worldZ - roomDataTable[startRoom].worldZ);

	zvPtr->ZVX1 -= Xdif;
	zvPtr->ZVX2 -= Xdif;
	zvPtr->ZVY1 += Ydif;
	zvPtr->ZVY2 += Ydif;
	zvPtr->ZVZ1 += Zdif;
	zvPtr->ZVZ2 += Zdif;
}

int CheckObjectCol(int actorIdx, ZVStruct* zvPtr)
{
	int currentCollisionSlot = 0;
    
	int actorRoom = ListObjets[actorIdx].room;

	for(int i=0;i<3;i++)
	{
		currentProcessedActorPtr->COL[i] = -1;
	}

	for(int i=0;i<NUM_MAX_OBJECT;i++)
	{
        tObject* currentActor = &ListObjets.at(i);

        if (currentActor->indexInWorld == -1)
            continue;
        if (i == actorIdx)
            continue;

        ZVStruct* currentActorZv = &currentActor->zv;

        if (currentActor->room != actorRoom)
        {
            ZVStruct localZv;

            CopyZV(zvPtr, &localZv);

            AdjustZV(&localZv, actorRoom, currentActor->room);

            if (CubeIntersect(&localZv, currentActorZv))
            {
                currentProcessedActorPtr->COL[currentCollisionSlot++] = i;

                if (currentCollisionSlot == 3)
                    return(3);
            }
        }
        else
        {
            if (CubeIntersect(zvPtr, currentActorZv))
            {
                currentProcessedActorPtr->COL[currentCollisionSlot++] = i;

                if (currentCollisionSlot == 3)
                    return(3);
            }
        }
	}

	return(currentCollisionSlot);
}

void take(int objIdx)
{
	tWorldObject* objPtr = &ListWorldObjets[objIdx];

	if(numObjInInventoryTable[currentInventory] == 0)
	{
		inventoryTable[currentInventory][0] = objIdx;
	}
	else
	{
		int i;

		for(i=numObjInInventoryTable[currentInventory];i>0;i--)
		{
			inventoryTable[currentInventory][i+1] = inventoryTable[currentInventory][i];
		}

		inventoryTable[currentInventory][1] = objIdx;
	}

	numObjInInventoryTable[currentInventory]++;

	action = 0x800;

	executeFoundLife(objIdx);

	if(objPtr->objIndex != -1)
	{
		DeleteObjet(objPtr->objIndex);
	}

	objPtr->foundFlag &= 0xBFFF;
	objPtr->foundFlag |= 0x8000;

	objPtr->room = -1;
	objPtr->stage = -1;
}

void Glisser(int flag)
{
	switch(flag)
	{
	case 1:
	case 2:
		{
			hardColStepZ = 0;
			break;
		}
	case 4:
	case 8:
		{
			hardColStepX = 0;
			break;
		}
	default:
		{
			break;
		}
	}
}

void GereCollision(ZVStruct* oldZv, ZVStruct* animatedZv, ZVStruct* fixZv)
{
	s32 oldpos = 0;
	s32 oldtype;
	s32 halfX;
	s32 halfZ;
	s32 pos;
	s32 type;

	if(oldZv->ZVX2 > fixZv->ZVX1)
	{
		if(fixZv->ZVX2 <= oldZv->ZVX1)
		{
			oldpos = 8; // right
		}
        else {
            oldpos = 0; // center
        }
	}
	else
	{
		oldpos = 4; // left
	}

	if(oldZv->ZVZ2 > fixZv->ZVZ1)
	{
		if(oldZv->ZVZ1 >= fixZv->ZVZ2)
		{
			oldpos |= 2;
		}
        else {
            oldpos |= 0; // center
        }
	}
	else
	{
		oldpos |= 1;
	}

	if( (oldpos == 5) || (oldpos == 9) || (oldpos == 6) || (oldpos == 10) )
	{
		oldtype = 2; // corner
	}
	else
	{
		if(oldpos == 0)
		{
			oldtype = 0;

			hardColStepZ = 0;
			hardColStepX = 0;

			return;
		}
		else
		{
			oldtype = 1; // corner
		}
	}

	halfX = (animatedZv->ZVX1 + animatedZv->ZVX2) / 2;
	halfZ = (animatedZv->ZVZ1 + animatedZv->ZVZ2) / 2;

	if(fixZv->ZVX1 > halfX)
	{
		pos = 4; // left
	}
	else
	{
		if(fixZv->ZVX2 < halfX)
		{
			pos = 0; // center
		}
		else
		{
			pos = 8; // right
		}
	}

	if(fixZv->ZVZ1 > halfZ)
	{
		pos |= 1; // up
	}
	else
	{
		if(fixZv->ZVZ2 < halfZ)
		{
			pos |= 0; // center
		}
		else
		{
			pos |= 2; // bellow
		}
	}

	if( (pos == 5) || (pos == 9) || (pos == 6) || (pos == 10) )
	{
		type = 2; // corner
	}
	else
	{
		if(pos == 0)
		{
			type = 0; // center
		}
		else
		{
			type = 1; // border
		}
	}

	if(oldtype == 1) // border
	{
		Glisser(oldpos);
	}
    else
    {
        if ((type == 1) && (pos & oldpos))
        {
            Glisser(pos);
        }
        else {
            if ((pos == oldpos) || (pos+oldpos == 15))
            {
                int Xmod = abs(animatedZv->ZVX1 - oldZv->ZVX1); // recheck
                int Zmod = abs(animatedZv->ZVZ1 - oldZv->ZVZ1);

                if (Xmod > Zmod)
                {
                    hardColStepZ = 0;
                }
                else
                {
                    hardColStepX = 0;
                }
            }
            else
            {
                if ((type == 0) || ((type == 1) && ((pos & oldpos) == 0)))
                {
                    hardColStepZ = 0;
                    hardColStepX = 0;
                }
                else
                {
                    Glisser(oldpos & pos);
                }
            }
        }
    }
}

int AsmCheckListCol(ZVStruct* zvPtr, roomDataStruct* pRoomData)
{
	u16 i;
	int hardColVar = 0;
	hardColStruct* pCurrentEntry = pRoomData->hardColTable.data();

#ifdef FITD_DEBUGGER
	if(debuggerVar_noHardClip)
		return 0;
#endif

	for(i=0;i<pRoomData->numHardCol;i++)
	{
		if(((pCurrentEntry->zv.ZVX1) < (zvPtr->ZVX2)) && ((zvPtr->ZVX1) < (pCurrentEntry->zv.ZVX2)))
		{
			if(((pCurrentEntry->zv.ZVY1) < (zvPtr->ZVY2)) && ((zvPtr->ZVY1) < (pCurrentEntry->zv.ZVY2)))
			{
				if(((pCurrentEntry->zv.ZVZ1) < (zvPtr->ZVZ2)) && ((zvPtr->ZVZ1) < (pCurrentEntry->zv.ZVZ2)))
				{
					ASSERT(hardColVar < 10);
					hardColTable[hardColVar++] = pCurrentEntry;
				}
			}
		}

		pCurrentEntry++;
	}

	return hardColVar;
}

void menuWaitVSync()
{
}

int testCrossProduct(int x1, int z1, int x2, int z2, int x3, int z3, int x4, int z4)
{
	int returnFlag = 0;

	int xAB = x1 - x2;
	int yCD = z3 - z4;
	int xCD = x3 - x4;
	int yAB = z1 - z2;

	int xAC = x1 - x3;
	int yAC = z1 - z3;

	int DotProduct = (xAB * yCD) - (xCD * yAC);

	if(DotProduct)
	{
		int Dda = xAC*yCD - xCD*yAC;
		int Dmu = -xAB*yAC + xAC*yAB;

		if(DotProduct<0)
		{
			DotProduct = -DotProduct;
			Dda = -Dda;
			Dmu = -Dmu;
		}

		if(Dda >= 0 && Dmu >= 0 && DotProduct >= Dda && DotProduct >= Dmu)
			returnFlag = 1;

	}

	return(returnFlag);
}

int isInPoly(int x1, int x2, int z1, int z2, cameraViewedRoomStruct* pCameraZoneDef)
{
	int xMid = (x1+x2)/2;
	int zMid = (z1+z2)/2;

	int i;

	for(i=0;i<pCameraZoneDef->coverZones.size();i++)
	{
		int j;
		int flag = 0;

		for(j=0;j<pCameraZoneDef->coverZones[i].numPoints;j++)
		{
			int zoneX1;
			int zoneZ1;
			int zoneX2;
			int zoneZ2;

			zoneX1= pCameraZoneDef->coverZones[i].pointTable[j].x;
			zoneZ1= pCameraZoneDef->coverZones[i].pointTable[j].y;
			zoneX2= pCameraZoneDef->coverZones[i].pointTable[j+1].x;
			zoneZ2= pCameraZoneDef->coverZones[i].pointTable[j+1].y;

			if(testCrossProduct(xMid,zMid,xMid-10000,zMid,zoneX1,zoneZ1,zoneX2,zoneZ2))
			{
				flag |= 1;
			}

			if(testCrossProduct(xMid,zMid,xMid+10000,zMid,zoneX1,zoneZ1,zoneX2,zoneZ2))
			{
				flag |= 2;
			}
		}

		if(flag == 3)
		{
			return(1);
		}
	}

	return(0);
}

int findBestCamera(void)
{
	int foundAngle = 32000;
	int foundCamera = -1;

	tObject* actorPtr = &ListObjets[currentCameraTargetActor];

	int x1 = actorPtr->zv.ZVX1/10;
	int x2 = actorPtr->zv.ZVX2/10;
	int z1 = actorPtr->zv.ZVZ1/10;
	int z2 = actorPtr->zv.ZVZ2/10;

	int i;

	for(i=0;i<numCameraInRoom;i++)
	{
		ASSERT(i<NUM_MAX_CAMERA_IN_ROOM);
        if(currentCameraZoneList[i])
		if(isInPoly(x1,x2,z1,z2,currentCameraZoneList[i])) // if in camera zone ?
		{
			// we try to select the best camera that looks behind the player
			int newAngle = actorPtr->beta + (((cameraDataTable[i]->beta)+0x200)&0x3FF);

			if(newAngle <0 )
			{
				newAngle = -newAngle;
			}

			if(newAngle < foundAngle)
			{
				foundAngle = newAngle;
				foundCamera = i;
			}
		}
	}

	return(foundCamera);
}

void GereSwitchCamera(void)
{
	int localCurrentCam = NumCamera;
	int newCamera;

	if(NumCamera!=-1)
	{
        if (currentCameraTargetActor == -1)
        {
            // TODO: Happens at the start of AITD3, likely a bug
            return;
        }

		tObject* actorPtr;
		int zvx1;
		int zvx2;
		int zvz1;
		int zvz2;

		actorPtr = &ListObjets[currentCameraTargetActor];

		zvx1 = actorPtr->zv.ZVX1/10;
		zvx2 = actorPtr->zv.ZVX2/10;

		zvz1 = actorPtr->zv.ZVZ1/10;
		zvz2 = actorPtr->zv.ZVZ2/10;

		if(isInPoly(zvx1,zvx2,zvz1,zvz2,currentCameraZoneList[NumCamera])) // is still in current camera zone ?
		{
			return;
		}
	}

#ifdef FITD_DEBUGGER
	//printf("Exited current camera cover zone...\n");
#endif

	newCamera = findBestCamera(); // find new camera

	if(newCamera!=-1)
	{
		localCurrentCam = newCamera;
	}

	if(NumCamera != localCurrentCam)
	{
		NewNumCamera = localCurrentCam;
		FlagInitView = 1;
	}

#ifdef FITD_DEBUGGER
	/* if(newCamera == -1)
	{
	printf("No new camera found...\n");
	}*/
#endif
}

bool isPointInZV(int x, int y, int z, ZVStruct* pZV)
{
	if(pZV->ZVX1 <= x && pZV->ZVX2 >= x)
	{
		if(pZV->ZVY1 <= y && pZV->ZVY2 >= y)
		{
			if(pZV->ZVZ1 <= z && pZV->ZVZ2 >= z)
			{
				return(true);
			}
		}
	}

	return false;
}

sceZoneStruct* processActor2Sub(int x, int y, int z, roomDataStruct* pRoomData)
{
	u32 i;
	sceZoneStruct* pCurrentZone;  

	pCurrentZone = pRoomData->sceZoneTable.data();

	for(i=0;i<pRoomData->numSceZone;i++)
	{
		if(pCurrentZone->zv.ZVX1 <= x && pCurrentZone->zv.ZVX2 >= x)
		{
			if(pCurrentZone->zv.ZVY1 <= y && pCurrentZone->zv.ZVY2 >= y)
			{
				if(pCurrentZone->zv.ZVZ1 <= z && pCurrentZone->zv.ZVZ2 >= z)
				{
					return(pCurrentZone);
				}
			}
		}

		pCurrentZone++;
	}

	return(NULL);

}

void GereDec()
{
	bool onceMore = false;
	bool flagFloorChange = false;
	int zoneIdx = 0;

	do
	{
		onceMore = false;
		roomDataStruct* pRoomData = &roomDataTable[currentProcessedActorPtr->room];
		for(u32 i=0;i<pRoomData->numSceZone;i++)
		{
			sceZoneStruct* pCurrentZone = &pRoomData->sceZoneTable[i];

			if(isPointInZV(	currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
				currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY,
				currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
				&pCurrentZone->zv))
			{
				switch(pCurrentZone->type)
				{
				case 0:
					{
						int oldRoom = currentProcessedActorPtr->room;

						currentProcessedActorPtr->room = (short)pCurrentZone->parameter;

                        int x = (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[oldRoom].worldX) * 10;
                        int y = (roomDataTable[currentProcessedActorPtr->room].worldY - roomDataTable[oldRoom].worldY) * 10;
                        int z = (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[oldRoom].worldZ) * 10;

						currentProcessedActorPtr->roomX -= x;
						currentProcessedActorPtr->roomY += y;
						currentProcessedActorPtr->roomZ += z;

						currentProcessedActorPtr->zv.ZVX1 -= x;
						currentProcessedActorPtr->zv.ZVX2 -= x;

						currentProcessedActorPtr->zv.ZVY1 += y;
						currentProcessedActorPtr->zv.ZVY2 += y;

						currentProcessedActorPtr->zv.ZVZ1 += z;
						currentProcessedActorPtr->zv.ZVZ2 += z;

						onceMore = true;
						if(currentProcessedActorIdx == currentCameraTargetActor)
						{
							FlagChangeSalle = 1;
							NewNumSalle = (short)pCurrentZone->parameter;
                            if (g_gameId > AITD1)
                            {
                                ChangeSalle(NewNumSalle);

                                // Hack: this wasn't in the original game, but is required here, or else we would use the current camera index in the new room and potentially go out of bound
                                InitView();
                            }
						}
						else
						{
							FlagGenereAffList = 1;
						}

						startChrono(&currentProcessedActorPtr->ROOM_CHRONO);


						break;
					}
				case 8:
					{
						assert(g_gameId != AITD1);
						if(g_gameId != AITD1)
						{
							currentProcessedActorPtr->hardMat = (short)pCurrentZone->parameter;
						}
						break;
					}
				case 9: // Scenar
					{
						if((g_gameId == AITD1) || !flagFloorChange)
						{
							currentProcessedActorPtr->HARD_DEC = (short)pCurrentZone->parameter;
						}
						break;
					}
				case 10: // stage
					{
                        int life = ListWorldObjets[currentProcessedActorPtr->indexInWorld].floorLife;

						if(life==-1)
							return;

						currentProcessedActorPtr->life = life;

						currentProcessedActorPtr->HARD_DEC = (short)pCurrentZone->parameter;
						flagFloorChange = true;
						break;
					}
				}

				if(g_gameId == AITD1) // AITD1 stops at the first zone
					return;
			}
			if(onceMore)
				break;
		}
	}while(onceMore);
}

int checkLineProjectionWithActors( int actorIdx, int X, int Y, int Z, int beta, int room, int param )
{
	ZVStruct localZv;
	int foundFlag = -2;
	int tempX;
	int tempZ;

	localZv.ZVX1 = X - param;
	localZv.ZVX2 = X + param;
	localZv.ZVY1 = Y - param;
	localZv.ZVY2 = Y + param;
	localZv.ZVZ1 = Z - param;
	localZv.ZVZ2 = Z + param;

	walkStep(param * 2, 0, beta);

	while(foundFlag == -2)
	{
		localZv.ZVX1 += animMoveX;
		localZv.ZVX2 += animMoveX;

		localZv.ZVZ1 += animMoveZ;
		localZv.ZVZ2 += animMoveZ;

		tempX = X;
		tempZ = Z;

		X += animMoveX;
		Z += animMoveZ;

		if(X > 20000 || X < -20000 || Z > 20000 || Z < -20000)
		{
			foundFlag = -1;
			break;
		}

		if(AsmCheckListCol(&localZv, &roomDataTable[room]) <= 0)
		{
			foundFlag = -1;
		}
		else
		{
			for(int i=0;i<ListObjets.size();i++)
			{
                tObject* currentActorPtr = &ListObjets[i];
				if(currentActorPtr->indexInWorld != -1 && i != actorIdx && !(currentActorPtr->objectType & AF_SPECIAL))
				{
					ZVStruct* zvPtr = &currentActorPtr->zv;

					if(room != currentActorPtr->room)
					{
						ZVStruct localZv2;

						CopyZV(&localZv, &localZv2);
						AdjustZV(&localZv2, room, currentActorPtr->room);

						if(!CubeIntersect(&localZv2,zvPtr))
						{
							currentActorPtr++;
							continue;
						}
					}
					else
					{
						if(!CubeIntersect(&localZv,zvPtr))
						{
							currentActorPtr++;
							continue;
						}
					}

					foundFlag = i;
					break;
				}
			}
		}
	}

	animMoveX = tempX;
	animMoveY = Y;
	animMoveZ = tempZ;

	return(foundFlag);
}

void PutAtObjet(int objIdx, int objIdxToPutAt)
{
	tWorldObject* objPtr = &ListWorldObjets[objIdx];
	tWorldObject* objPtrToPutAt = &ListWorldObjets[objIdxToPutAt];

	if(objPtrToPutAt->objIndex != -1)
	{
		tObject* actorToPutAtPtr = &ListObjets[objPtrToPutAt->objIndex];

		DeleteInventoryObjet(objIdx);

		if(objPtr->objIndex == -1)
		{
			objPtr->x = actorToPutAtPtr->roomX;
			objPtr->y = actorToPutAtPtr->roomY;
			objPtr->z = actorToPutAtPtr->roomZ;
			objPtr->room = actorToPutAtPtr->room;
			objPtr->stage = actorToPutAtPtr->stage;
			objPtr->alpha = actorToPutAtPtr->alpha;
			objPtr->beta = actorToPutAtPtr->beta;
			objPtr->gamma = actorToPutAtPtr->gamma;

			objPtr->foundFlag |= 0x4000;
			objPtr->flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		}
		else
		{
			currentProcessedActorPtr->roomX = actorToPutAtPtr->roomX;
			currentProcessedActorPtr->roomY = actorToPutAtPtr->roomY;
			currentProcessedActorPtr->roomZ = actorToPutAtPtr->roomZ;
			currentProcessedActorPtr->room = actorToPutAtPtr->room;
			currentProcessedActorPtr->stage = actorToPutAtPtr->stage;
			currentProcessedActorPtr->alpha = actorToPutAtPtr->alpha;
			currentProcessedActorPtr->beta = actorToPutAtPtr->beta;
			currentProcessedActorPtr->gamma = actorToPutAtPtr->gamma;

			ListWorldObjets[currentProcessedActorPtr->indexInWorld].foundFlag |= 0x4000;
			ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		}

	}
	else
	{
		DeleteInventoryObjet(objIdx);

		if(objPtr->objIndex == -1)
		{
			objPtr->x = objPtrToPutAt->x;
			objPtr->y = objPtrToPutAt->y;
			objPtr->z = objPtrToPutAt->z;
			objPtr->room = objPtrToPutAt->room;
			objPtr->stage = objPtrToPutAt->stage;
			objPtr->alpha = objPtrToPutAt->alpha;
			objPtr->beta = objPtrToPutAt->beta;
			objPtr->gamma = objPtrToPutAt->gamma;

			objPtr->foundFlag |= 0x4000;
			objPtr->flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		}
		else
		{
			currentProcessedActorPtr->roomX = objPtrToPutAt->x;
			currentProcessedActorPtr->roomY = objPtrToPutAt->y;
			currentProcessedActorPtr->roomZ = objPtrToPutAt->z;
			currentProcessedActorPtr->room = objPtrToPutAt->room;
			currentProcessedActorPtr->stage = objPtrToPutAt->stage;
			currentProcessedActorPtr->alpha = objPtrToPutAt->alpha;
			currentProcessedActorPtr->beta = objPtrToPutAt->beta;
			currentProcessedActorPtr->gamma = objPtrToPutAt->gamma;

			ListWorldObjets[currentProcessedActorPtr->indexInWorld].foundFlag |= 0x4000;
			ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		}
	}
}

void throwStoppedAt(int x, int z)
{
	int x2;
	int y2;
	int z2;
	int foundPosition;
	int step;

	ZVStruct zvCopy;
	ZVStruct zvLocal;

	sBody* bodyPtr = HQR_Get(HQ_Bodys,currentProcessedActorPtr->bodyNum);

	GiveZVObjet(bodyPtr,&zvLocal);

	x2 = x;
	y2 = (currentProcessedActorPtr->roomY/2000)*2000;
	z2 = z;

	foundPosition = 0;
	step = 0;

	while(!foundPosition)
	{
		walkStep(0,-step,currentProcessedActorPtr->beta+0x200);
		CopyZV(&zvLocal,&zvCopy);

		x2 = x + animMoveX;
		z2 = z + animMoveZ;

		zvCopy.ZVX1 += x2;
		zvCopy.ZVX2 += x2;

		zvCopy.ZVY1 += y2;
		zvCopy.ZVY2 += y2;

		zvCopy.ZVZ1 += z2;
		zvCopy.ZVZ2 += z2;

		if(!AsmCheckListCol(&zvCopy,&roomDataTable[currentProcessedActorPtr->room]))
		{
			foundPosition = 1;
		}

		if(foundPosition)
		{
			if(y2<-500)
			{
				zvCopy.ZVY1 += 100; // is the object reachable ? (100 is Carnby height. If hard col at Y + 100, carnby can't reach that spot)
				zvCopy.ZVY2 += 100;

				if(!AsmCheckListCol(&zvCopy,&roomDataTable[currentProcessedActorPtr->room]))
				{
					y2 += 2000;
					foundPosition = 0;
				}
				else
				{
					zvCopy.ZVY1 -= 100;
					zvCopy.ZVY2 -= 100;
				}
			}
		}
		else
		{
			step+=100;
		}
	}

	currentProcessedActorPtr->worldX = x2;
	currentProcessedActorPtr->roomX = x2;
	currentProcessedActorPtr->worldY = y2;
	currentProcessedActorPtr->roomY = y2;
	currentProcessedActorPtr->worldZ = z2;
	currentProcessedActorPtr->roomZ = z2;

	currentProcessedActorPtr->stepX = 0;
	currentProcessedActorPtr->stepZ = 0;

	currentProcessedActorPtr->animActionType = 0;
	currentProcessedActorPtr->speed = 0;
	currentProcessedActorPtr->gamma = 0;

	GiveZVObjet(bodyPtr,&currentProcessedActorPtr->zv);

	currentProcessedActorPtr->zv.ZVX1 += x2;
	currentProcessedActorPtr->zv.ZVX2 += x2;
	currentProcessedActorPtr->zv.ZVY1 += y2;
	currentProcessedActorPtr->zv.ZVY2 += y2;
	currentProcessedActorPtr->zv.ZVZ1 += z2;
	currentProcessedActorPtr->zv.ZVZ2 += z2;

	ListWorldObjets[currentProcessedActorPtr->indexInWorld].foundFlag |= 0x4000;
	ListWorldObjets[currentProcessedActorPtr->indexInWorld].foundFlag &= 0xEFFF;

	addActorToBgInscrust(currentProcessedActorIdx);
}

void startGame(int startupFloor, int startupRoom, int allowSystemMenu)
{
	LoadWorld();
	initVars();

	LoadEtage(startupFloor);

	NumCamera = -1;

	ChangeSalle(startupRoom);

	NewNumCamera = 0;
	FlagInitView = 2;

	InitView();

	PlayWorld(allowSystemMenu, 1);

	/*freeScene();

	fadeOut(8,0);*/
}

int parseAllSaves(int arg)
{
	return(0);
	// TODO : make real implementation
}

void detectGame(void)
{
	if(fileExists("LISTBOD2.PAK"))
	{
		g_gameId = AITD1;
        CVars.resize(45);
		currentCVarTable = AITD1KnownCVars;

		printf("Detected Alone in the Dark\n");
#ifndef AITD_UE4
        SDL_SetWindowTitle(gWindowBGFX, "Alone in the Dark");
#endif
		return;
	}
	if(fileExists("PERE.PAK"))
	{
		g_gameId = JACK;
        CVars.resize(70);
		currentCVarTable = AITD2KnownCVars;

		printf("Detected Jack in the Dark\n");
#ifndef AITD_UE4
        SDL_SetWindowTitle(gWindowBGFX, "Jack in the Dark");
#endif
		return;
	}
	if(fileExists("MER.PAK"))
	{
		g_gameId = AITD2;
        CVars.resize(70);
		currentCVarTable = AITD2KnownCVars;

		printf("Detected Alone in the Dark 2\n");
#ifndef AITD_UE4
        SDL_SetWindowTitle(gWindowBGFX, "Alone in the Dark 2");
#endif
		return;
	}
	if(fileExists("AN1.PAK"))
	{
		g_gameId = AITD3;
        CVars.resize(70);
		currentCVarTable = AITD2KnownCVars;

		printf("Detected Alone in the Dark 3\n");
#ifndef AITD_UE4
        SDL_SetWindowTitle(gWindowBGFX, "Alone in the Dark 3");
#endif
		return;
	}
	if(fileExists("PURSUIT.PAK"))
	{
		g_gameId = TIMEGATE;
        CVars.resize(100); // TODO: figure this
		currentCVarTable = AITD2KnownCVars; // TODO: figure this

		printf("Detected Time Gate\n");
#ifndef AITD_UE4
        SDL_SetWindowTitle(gWindowBGFX, "Time Gate");
#endif
		return;
	}

	printf("FATAL: Game detection failed...\n");
	assert(0);
}

extern "C" {
	int FitdMain(int argc, char* argv[]);
	void setCurrentContext(void);
}

int FitdMain(int argc, char* argv[])
{
#ifndef AITD_UE4
    initBgfxGlue(argc, argv);
#endif

	osystem_startOfFrame();

	//  int protectionToBeDone = 1;

	OpenProgram();

	paletteFill(currentGamePalette,0,0,0);

	loadPalette();
	
	switch(g_gameId)
	{
	case AITD1:
		startAITD1();
		break;
	case JACK:
		startJACK();
		break;
	case AITD2:
		startAITD2();
		break;
	case AITD3:
		startAITD3();
		break;
    case TIMEGATE:
        startGame(0, 5, 1);
        break;
	default:
		assert(0);
		break;
	}

	return(0);
}

int drawTextOverlay(void)
{
	int var_14 = 0;
	int var_10 = 183;
	messageStruct* currentMessage;

	BBox3D4 = 199;
	BBox3D1 = 319;
	BBox3D3 = 0;

	currentMessage = messageTable;

	if(lightOff==0)
	{
		int i;

		for(i=0;i<5;i++)
		{
			if(currentMessage->string)
			{
				int width = currentMessage->string->width;
				int X = 160 - width/2;
				int Y = X + width;

				if(X<BBox3D1)
				{
					BBox3D1 = X;
				}

				if(Y>BBox3D3)
				{
					BBox3D3 = Y;
				}

				if((currentMessage->time++)>55)
				{
					currentMessage->string = NULL;
				}
				else
				{
					if(currentMessage->time<26)
					{
						SetFont(PtrFont,16);
					}
					else
					{
						SetFont(PtrFont,16+(currentMessage->time-26)/2);
					}

					PrintFont(X,var_10+1,logicalScreen,currentMessage->string->textPtr);
				}

				var_10 -= 16;
				var_14 = 1;

			}

			currentMessage++;
		}
	}
	else
	{
	}

	BBox3D2 = var_10;
	return(var_14);
}

void makeMessage(int messageIdx)
{
	textEntryStruct* messagePtr;

	messagePtr = getTextFromIdx(messageIdx);

	if(messagePtr)
	{
		int i;

		for(i=0;i<5;i++)
		{
			if(messageTable[i].string == messagePtr)
			{
				messageTable[i].time = 0;
				return;
			}
		}

		for(i=0;i<5;i++)
		{
			if(messageTable[i].string == NULL)
			{
				messageTable[i].string = messagePtr;
				messageTable[i].time = 0;
				return;
			}
		}
	}
}

void hit(int animNumber,int arg_2,int arg_4,int arg_6,int hitForce,int arg_A)
{
	if(InitAnim(animNumber, 0, arg_A))
	{
		currentProcessedActorPtr->animActionANIM = animNumber;
		currentProcessedActorPtr->animActionFRAME = arg_2;
		currentProcessedActorPtr->animActionType = 1;
		currentProcessedActorPtr->animActionParam = arg_6;
		currentProcessedActorPtr->hotPointID = arg_4;
		currentProcessedActorPtr->hitForce = hitForce;
	}
}

void SetClip(int left, int top, int right, int bottom)
{
	clipLeft = left;
	clipTop = top;
	clipRight = right;
	clipBottom = bottom;
}

extern "C" {
	void Sound_Quit(void);
}

void cleanupAndExit(void)
{
	Sound_Quit();

	HQR_Free(listMus);
	HQR_Free(listSamp);
	HQR_Free(HQ_Memory);
	HQR_Free(listLife);
	HQR_Free(listTrack);
	HQR_Free(HQ_Bodys);
	HQR_Free(HQ_Anims);

	/* free(tabTextes);
	free(aux);
	free(aux2);
	free(bufferAnim);

	free(screen); */

	destroyMusicDriver();

	exit(0);
}
