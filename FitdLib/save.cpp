#include "common.h"

unsigned int currentSaveEntrySize;

void* getSaveEntry(int index)
{
    currentSaveEntrySize = saveTable[index].size;

    return(saveTable[index].ptr);
}

void loadInterpolatedValue(RealValue* pRotateStruct, FILE* fHandle)
{
    ASSERT(sizeof(pRotateStruct->startValue) == 2);
    fread(&pRotateStruct->startValue,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->endValue) == 2);
    fread(&pRotateStruct->endValue,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->numSteps) == 2);
    fread(&pRotateStruct->numSteps,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->memoTicks) == 4);
    fread(&pRotateStruct->memoTicks,2,1,fHandle);
}

void saveInterpolatedValue(RealValue* pRotateStruct, FILE* fHandle)
{
    ASSERT(sizeof(pRotateStruct->startValue) == 2);
    fwrite(&pRotateStruct->startValue,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->endValue) == 2);
    fwrite(&pRotateStruct->endValue,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->numSteps) == 2);
    fwrite(&pRotateStruct->numSteps,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->memoTicks) == 4);
    fwrite(&pRotateStruct->memoTicks,2,1,fHandle);
}

int loadSave(int saveNumber)
{
    char buffer[256];
    FILE* fHandle;
    unsigned int var28;
    int var_E;
    int var_16;
    unsigned int offsetToVars;
    u16 tempVarSize;
    unsigned int offsetToActors;
    int i;
    int oldNumMaxObj;

    sprintf(buffer,"SAVE%d.ITD",saveNumber);

    fHandle = fopen(buffer,"rb");

    if(!fHandle)
    {
        return(0);
    }

    LoadWorld();
    initVars();

    fseek(fHandle,8,SEEK_SET);

    fread(&var28,4,1,fHandle);

    var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | (( var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);

    fseek(fHandle,var28,SEEK_SET);

    ASSERT(sizeof(currentRoom) == 2);
    fread(&currentRoom,2,1,fHandle);

    ASSERT(sizeof(g_currentFloor) == 2);
    fread(&g_currentFloor,2,1,fHandle);

    ASSERT(sizeof(NumCamera) == 2);
    fread(&NumCamera,2,1,fHandle);

    ASSERT(sizeof(currentWorldTarget) == 2);
    fread(&currentWorldTarget,2,1,fHandle);

    ASSERT(sizeof(currentCameraTargetActor) == 2);
    fread(&currentCameraTargetActor,2,1,fHandle);

    ASSERT(sizeof(maxObjects) == 2);
    fread(&maxObjects,2,1,fHandle);

    if(g_gameId == AITD1)
    {
        oldNumMaxObj = maxObjects;
        maxObjects = 300; // fix for save engine..
    }

    for(i=0;i<maxObjects;i++)
    {
        ASSERT(sizeof(ListWorldObjets[i].objIndex) == 2);
        fread(&ListWorldObjets[i].objIndex,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].body) == 2);
        fread(&ListWorldObjets[i].body,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].flags) == 2);
        fread(&ListWorldObjets[i].flags,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].typeZV) == 2);
        fread(&ListWorldObjets[i].typeZV,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].foundBody) == 2);
        fread(&ListWorldObjets[i].foundBody,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].foundName) == 2);
        fread(&ListWorldObjets[i].foundName,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].foundFlag) == 2);
        fread(&ListWorldObjets[i].foundFlag,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].foundLife) == 2);
        fread(&ListWorldObjets[i].foundLife,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].x) == 2);
        fread(&ListWorldObjets[i].x,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].y) == 2);
        fread(&ListWorldObjets[i].y,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].z) == 2);
        fread(&ListWorldObjets[i].z,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].alpha) == 2);
        fread(&ListWorldObjets[i].alpha,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].beta) == 2);
        fread(&ListWorldObjets[i].beta,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].gamma) == 2);
        fread(&ListWorldObjets[i].gamma,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].stage) == 2);
        fread(&ListWorldObjets[i].stage,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].room) == 2);
        fread(&ListWorldObjets[i].room,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].lifeMode) == 2);
        fread(&ListWorldObjets[i].lifeMode,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].life) == 2);
        fread(&ListWorldObjets[i].life,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].floorLife) == 2);
        fread(&ListWorldObjets[i].floorLife,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].anim) == 2);
        fread(&ListWorldObjets[i].anim,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].frame) == 2);
        fread(&ListWorldObjets[i].frame,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].animType) == 2);
        fread(&ListWorldObjets[i].animType,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].animInfo) == 2);
        fread(&ListWorldObjets[i].animInfo,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].trackMode) == 2);
        fread(&ListWorldObjets[i].trackMode,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].trackNumber) == 2);
        fread(&ListWorldObjets[i].trackNumber,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].positionInTrack) == 2);
        fread(&ListWorldObjets[i].positionInTrack,2,1,fHandle);
    }

    if(g_gameId == AITD1)
    {
        maxObjects = oldNumMaxObj;
    }

    if(g_gameId == AITD1)
    {
        ASSERT(CVars.size() == 45);
    }

    for(i=0;i< CVars.size();i++)
    {
        ASSERT(sizeof(CVars[i]) == 2);
        fread(&CVars[i], 2, 1, fHandle);
    }
	
	for(int inventoryId=0; inventoryId<NUM_MAX_INVENTORY; inventoryId++)
	{
		ASSERT(sizeof(inHandTable[inventoryId]) == 2);
		fread(&inHandTable[inventoryId],2,1,fHandle);

		ASSERT(sizeof(numObjInInventoryTable[inventoryId]) == 2);
		fread(&numObjInInventoryTable[inventoryId],2,1,fHandle);

		if(g_gameId == AITD1)
		{
			ASSERT(INVENTORY_SIZE == 30);
		}

		for(i=0;i<INVENTORY_SIZE;i++)
		{
			ASSERT(sizeof(inventoryTable[inventoryId][i]) == 2);
			fread(&inventoryTable[inventoryId][i],2,1,fHandle);
		}
	}

    ASSERT(sizeof(statusScreenAllowed) == 2);
    fread(&statusScreenAllowed,2,1,fHandle);

    ASSERT(sizeof(FlagGameOver) == 2);
    fread(&FlagGameOver,2,1,fHandle);

    ASSERT(sizeof(lightOff) == 2);
    fread(&lightOff,2,1,fHandle);

    ASSERT(sizeof(shakingAmplitude) == 2);
    fread(&shakingAmplitude,2,1,fHandle);

    ASSERT(sizeof(shakeVar1) == 2);
    fread(&shakeVar1,2,1,fHandle);

    ASSERT(sizeof(timer) == 4);
    fread(&timer,4,1,fHandle);

    ASSERT(sizeof(timerFreeze1) == 4);
    fread(&timerFreeze1,4,1,fHandle);

    ASSERT(sizeof(currentMusic) == 2);
    fread(&currentMusic,2,1,fHandle);

    //timerFreeze = 1;

    var_E = NumCamera;

    LoadEtage(g_currentFloor);
    NumCamera = -1;
    ChangeSalle(currentRoom);
    var_16 = currentMusic;
    currentMusic = -1;
    playMusic(var_16);

    fseek(fHandle,12,SEEK_SET);
    fread(&offsetToVars,4,1,fHandle);
    offsetToVars = ((offsetToVars & 0xFF) << 24) | ((offsetToVars & 0xFF00) << 8) | (( offsetToVars & 0xFF0000) >> 8) | ((offsetToVars & 0xFF000000) >> 24);
    fseek(fHandle,offsetToVars,SEEK_SET);

    fread(&tempVarSize,2,1,fHandle);
    varSize = tempVarSize;

    fread(vars,varSize,1,fHandle);

    if(g_gameId == AITD1)
    {
        configureHqrHero(HQ_Bodys,listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
        configureHqrHero(HQ_Anims,listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
    }
    else
    {
		/*
        configureHqrHero(listBody,0);
        configureHqrHero(listAnim,0);
		*/
    }

    fseek(fHandle,16,SEEK_SET);
    fread(&offsetToActors,4,1,fHandle);
    offsetToVars = ((offsetToActors & 0xFF) << 24) | ((offsetToActors & 0xFF00) << 8) | (( offsetToActors & 0xFF0000) >> 8) | ((offsetToActors & 0xFF000000) >> 24);
    fseek(fHandle,offsetToVars,SEEK_SET);

    for(i=0;i<NUM_MAX_OBJECT;i++)
    {
        ASSERT(sizeof(ListObjets[i].indexInWorld) == 2);
        fread(&ListObjets[i].indexInWorld,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].bodyNum) == 2);
        fread(&ListObjets[i].bodyNum,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].objectType) == 2);
        fread(&ListObjets[i].objectType,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].dynFlags) == 2);
        fread(&ListObjets[i].dynFlags,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVX1) == 2);
        fread(&ListObjets[i].zv.ZVX1,2,1,fHandle);
        ListObjets[i].zv.ZVX1 = (s16)ListObjets[i].zv.ZVX1;

        //    ASSERT(sizeof(actorTable[i].zv.ZVX2) == 2);
        fread(&ListObjets[i].zv.ZVX2,2,1,fHandle);
        ListObjets[i].zv.ZVX2 = (s16)ListObjets[i].zv.ZVX2;

        //    ASSERT(sizeof(actorTable[i].zv.ZVY1) == 2);
        fread(&ListObjets[i].zv.ZVY1,2,1,fHandle);
        ListObjets[i].zv.ZVY1 = (s16)ListObjets[i].zv.ZVY1;

        //    ASSERT(sizeof(actorTable[i].zv.ZVY2) == 2);
        fread(&ListObjets[i].zv.ZVY2,2,1,fHandle);
        ListObjets[i].zv.ZVY2 = (s16)ListObjets[i].zv.ZVY2;

        //    ASSERT(sizeof(actorTable[i].zv.ZVZ1) == 2);
        fread(&ListObjets[i].zv.ZVZ1,2,1,fHandle);
        ListObjets[i].zv.ZVZ1 = (s16)ListObjets[i].zv.ZVZ1;

        //    ASSERT(sizeof(actorTable[i].zv.ZVZ2) == 2);
        fread(&ListObjets[i].zv.ZVZ2,2,1,fHandle);
        ListObjets[i].zv.ZVZ2 = (s16)ListObjets[i].zv.ZVZ2;

        ASSERT(sizeof(ListObjets[i].screenXMin) == 2);
        fread(&ListObjets[i].screenXMin,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].screenYMin) == 2);
        fread(&ListObjets[i].screenYMin,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].screenXMax) == 2);
        fread(&ListObjets[i].screenXMax,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].screenYMax) == 2);
        fread(&ListObjets[i].screenYMax,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].roomX) == 2);
        fread(&ListObjets[i].roomX,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].roomY) == 2);
        fread(&ListObjets[i].roomY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].roomZ) == 2);
        fread(&ListObjets[i].roomZ,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].worldX) == 2);
        fread(&ListObjets[i].worldX,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].worldY) == 2);
        fread(&ListObjets[i].worldY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].worldZ) == 2);
        fread(&ListObjets[i].worldZ,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].alpha) == 2);
        fread(&ListObjets[i].alpha,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].beta) == 2);
        fread(&ListObjets[i].beta,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].gamma) == 2);
        fread(&ListObjets[i].gamma,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].room) == 2);
        fread(&ListObjets[i].room,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].stage) == 2);
        fread(&ListObjets[i].stage,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].lifeMode) == 2);
        fread(&ListObjets[i].lifeMode,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].life) == 2);
        fread(&ListObjets[i].life,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].CHRONO) == 4);
        fread(&ListObjets[i].CHRONO,4,1,fHandle);

        ASSERT(sizeof(ListObjets[i].ROOM_CHRONO) == 4);
        fread(&ListObjets[i].ROOM_CHRONO,4,1,fHandle);

        ASSERT(sizeof(ListObjets[i].ANIM) == 2);
        fread(&ListObjets[i].ANIM,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animType) == 2);
        fread(&ListObjets[i].animType,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animInfo) == 2);
        fread(&ListObjets[i].animInfo,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].newAnim) == 2);
        fread(&ListObjets[i].newAnim,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].newAnimType) == 2);
        fread(&ListObjets[i].newAnimType,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].newAnimInfo) == 2);
        fread(&ListObjets[i].newAnimInfo,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].frame) == 2);
        fread(&ListObjets[i].frame,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].numOfFrames) == 2);
        fread(&ListObjets[i].numOfFrames,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].END_FRAME) == 2);
        fread(&ListObjets[i].END_FRAME,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].flagEndAnim) == 2);
        fread(&ListObjets[i].flagEndAnim,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].trackMode) == 2);
        fread(&ListObjets[i].trackMode,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].trackNumber) == 2);
        fread(&ListObjets[i].trackNumber,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].MARK) == 2);
        fread(&ListObjets[i].MARK,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].positionInTrack) == 2);
        fread(&ListObjets[i].positionInTrack,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].stepX) == 2);
        fread(&ListObjets[i].stepX,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].stepY) == 2);
        fread(&ListObjets[i].stepY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].stepZ) == 2); // 45
        fread(&ListObjets[i].stepZ,2,1,fHandle);

        loadInterpolatedValue(&ListObjets[i].YHandler,fHandle);

        ASSERT(sizeof(ListObjets[i].falling) == 2);
        fread(&ListObjets[i].falling,2,1,fHandle);

        loadInterpolatedValue(&ListObjets[i].rotate,fHandle);

        ASSERT(sizeof(ListObjets[i].direction) == 2); 
        fread(&ListObjets[i].direction,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].speed) == 2);
        fread(&ListObjets[i].speed,2,1,fHandle);

        loadInterpolatedValue(&ListObjets[i].speedChange,fHandle);

        ASSERT(sizeof(ListObjets[i].COL[0]) == 2);
        fread(&ListObjets[i].COL[0],2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].COL[1]) == 2);
        fread(&ListObjets[i].COL[1],2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].COL[2]) == 2);
        fread(&ListObjets[i].COL[2],2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].COL_BY) == 2);
        fread(&ListObjets[i].COL_BY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].HARD_DEC) == 2);
        fread(&ListObjets[i].HARD_DEC,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].HARD_COL) == 2);
        fread(&ListObjets[i].HARD_COL,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].HIT) == 2);
        fread(&ListObjets[i].HIT,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].HIT_BY) == 2);
        fread(&ListObjets[i].HIT_BY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animActionType) == 2);
        fread(&ListObjets[i].animActionType,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animActionANIM) == 2);
        fread(&ListObjets[i].animActionANIM,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animActionFRAME) == 2);
        fread(&ListObjets[i].animActionFRAME,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animActionParam) == 2);
        fread(&ListObjets[i].animActionParam,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hitForce) == 2);
        fread(&ListObjets[i].hitForce,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hotPointID) == 2);
        fread(&ListObjets[i].hotPointID,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hotPoint.x) == 2);
        fread(&ListObjets[i].hotPoint.x,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hotPoint.y) == 2);
        fread(&ListObjets[i].hotPoint.x,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hotPoint.z) == 2);
        fread(&ListObjets[i].hotPoint.x,2,1,fHandle);
    }
    fclose(fHandle);

    for(i=0;i<NUM_MAX_OBJECT;i++)
    {
        if(ListObjets[i].indexInWorld != -1 && ListObjets[i].bodyNum != -1)
        {
            sBody* bodyPtr = HQR_Get(HQ_Bodys,ListObjets[i].bodyNum);

            if(ListObjets[i].ANIM != -1)
            {
                sAnimation* animPtr = HQR_Get(HQ_Anims,ListObjets[i].ANIM);
                SetAnimObjet(ListObjets[i].frame,animPtr,bodyPtr);
            }
        }
    }

    NewNumCamera = var_E;

    return(1);
}

int restoreSave(int arg0, int arg1)
{
    int selectedSave;
    //  restoreSaveVar1 = arg0;

    if(arg1==0)
    {
        flushScreen();
        osystem_flip(NULL);
        FadeInPhys(0x40,0);
    }

    selectedSave = parseAllSaves(0);

    if(arg1==0)
    {
        //  fadeOut(8,0);
    }

    if(selectedSave == -1)
    {
        return(0);
    }

    if(arg1==0)
    {
        //freeScene();
    }

    return(loadSave(selectedSave));

}

int makeSaveFile(int entry)
{
    FILE* fHandle;
    char buffer[100];
    int i;
    unsigned long int var28 = 0;
    int temp;
    int oldNumMaxObj;

	if(g_gameId == AITD1)
	{
        // For safety, destroy special objects before mallocs
		for(i=0;i<NUM_MAX_OBJECT;i++)
		{
            if (ListObjets[i].indexInWorld == -2) // Special objects
            {
                ListObjets[i].indexInWorld = -1;
                if (ListObjets[i].ANIM == 4)
                {
                    CVars[getCVarsIdx(FOG_FLAG)] = 0;
                    HQ_Free_Malloc(HQ_Memory, ListObjets[i].frame);
                }
            }
		}
	}

    sprintf(buffer,"SAVE%d.ITD",entry);

    fHandle = fopen(buffer,"wb+");

    if(!fHandle)
        return 0;

    fwrite(&var28,4,1,fHandle);
    fwrite(&var28,4,1,fHandle);

    var28 = ftell(fHandle)+12;
    var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | (( var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);

    fwrite(&var28,4,1,fHandle);

    fwrite(&var28,4,1,fHandle);
    fwrite(&var28,4,1,fHandle);

    ASSERT(sizeof(currentRoom) == 2);
    fwrite(&currentRoom,2,1,fHandle);

    ASSERT(sizeof(g_currentFloor) == 2);
    fwrite(&g_currentFloor,2,1,fHandle);

    ASSERT(sizeof(NumCamera) == 2);
    fwrite(&NumCamera,2,1,fHandle);

    ASSERT(sizeof(currentWorldTarget) == 2);
    fwrite(&currentWorldTarget,2,1,fHandle);

    ASSERT(sizeof(currentCameraTargetActor) == 2);
    fwrite(&currentCameraTargetActor,2,1,fHandle);

    ASSERT(sizeof(maxObjects) == 2);
    fwrite(&maxObjects,2,1,fHandle);

    if(g_gameId == AITD1)
    {
        oldNumMaxObj = maxObjects;
        maxObjects = 300; // fix for save engine..
    }

    for(i=0;i<maxObjects;i++)
    {
        ASSERT(sizeof(ListWorldObjets[i].objIndex) == 2);
        fwrite(&ListWorldObjets[i].objIndex,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].body) == 2);
        fwrite(&ListWorldObjets[i].body,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].flags) == 2);
        fwrite(&ListWorldObjets[i].flags,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].typeZV) == 2);
        fwrite(&ListWorldObjets[i].typeZV,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].foundBody) == 2);
        fwrite(&ListWorldObjets[i].foundBody,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].foundName) == 2);
        fwrite(&ListWorldObjets[i].foundName,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].foundFlag) == 2);
        fwrite(&ListWorldObjets[i].foundFlag,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].foundLife) == 2);
        fwrite(&ListWorldObjets[i].foundLife,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].x) == 2);
        fwrite(&ListWorldObjets[i].x,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].y) == 2);
        fwrite(&ListWorldObjets[i].y,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].z) == 2);
        fwrite(&ListWorldObjets[i].z,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].alpha) == 2);
        fwrite(&ListWorldObjets[i].alpha,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].beta) == 2);
        fwrite(&ListWorldObjets[i].beta,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].gamma) == 2);
        fwrite(&ListWorldObjets[i].gamma,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].stage) == 2);
        fwrite(&ListWorldObjets[i].stage,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].room) == 2);
        fwrite(&ListWorldObjets[i].room,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].lifeMode) == 2);
        fwrite(&ListWorldObjets[i].lifeMode,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].life) == 2);
        fwrite(&ListWorldObjets[i].life,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].floorLife) == 2);
        fwrite(&ListWorldObjets[i].floorLife,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].anim) == 2);
        fwrite(&ListWorldObjets[i].anim,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].frame) == 2);
        fwrite(&ListWorldObjets[i].frame,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].animType) == 2);
        fwrite(&ListWorldObjets[i].animType,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].animInfo) == 2);
        fwrite(&ListWorldObjets[i].animInfo,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].trackMode) == 2);
        fwrite(&ListWorldObjets[i].trackMode,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].trackNumber) == 2);
        fwrite(&ListWorldObjets[i].trackNumber,2,1,fHandle);

        ASSERT(sizeof(ListWorldObjets[i].positionInTrack) == 2);
        fwrite(&ListWorldObjets[i].positionInTrack,2,1,fHandle);
    }

    if(g_gameId == AITD1)
    {
        maxObjects = oldNumMaxObj;
    }

    if(g_gameId == AITD1)
    {
        ASSERT(CVars.size() == 45);
    }

    for(i=0;i< CVars.size();i++)
    {
        ASSERT(sizeof(CVars[i]) == 2);
        fwrite(&CVars[i], 2, 1, fHandle);
    }

	for(int inventoryId = 0; inventoryId<NUM_MAX_INVENTORY; inventoryId++)
	{
		ASSERT(sizeof(inHandTable[inventoryId]) == 2);
		fwrite(&inHandTable[inventoryId],2,1,fHandle);

		ASSERT(sizeof(numObjInInventoryTable[inventoryId]) == 2);
		fwrite(&numObjInInventoryTable[inventoryId],2,1,fHandle);

		if(g_gameId == AITD1)
		{
			ASSERT(INVENTORY_SIZE == 30);
		}

		for(i=0;i<INVENTORY_SIZE;i++)
		{
			ASSERT(sizeof(inventoryTable[inventoryId][i]) == 2);
			fwrite(&inventoryTable[inventoryId][i],2,1,fHandle);
		}
	}

    ASSERT(sizeof(statusScreenAllowed) == 2);
    fwrite(&statusScreenAllowed,2,1,fHandle);

    ASSERT(sizeof(FlagGameOver) == 2);
    fwrite(&FlagGameOver,2,1,fHandle);

    ASSERT(sizeof(lightOff) == 2);
    fwrite(&lightOff,2,1,fHandle);

    ASSERT(sizeof(shakingAmplitude) == 2);
    fwrite(&shakingAmplitude,2,1,fHandle);

    ASSERT(sizeof(shakeVar1) == 2);
    fwrite(&shakeVar1,2,1,fHandle);

    ASSERT(sizeof(timer) == 4);
    fwrite(&timer,4,1,fHandle);

    ASSERT(sizeof(timerFreeze1) == 4);
    fwrite(&timerFreeze1,4,1,fHandle);

    ASSERT(sizeof(currentMusic) == 2);
    fwrite(&currentMusic,2,1,fHandle);

    //timerFreeze = 1;

    var28 = ftell(fHandle);
    temp = var28;
    fseek(fHandle,12,SEEK_SET);
    var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | (( var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);
    fwrite(&var28,4,1,fHandle);
    fseek(fHandle,temp,SEEK_SET);

    fwrite(&varSize,2,1,fHandle);
    fwrite(vars,varSize,1,fHandle);

    var28 = ftell(fHandle);
    temp = var28;
    fseek(fHandle,16,SEEK_SET);
    var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | (( var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);
    fwrite(&var28,4,1,fHandle);
    fseek(fHandle,temp,SEEK_SET);

    for(i=0;i<NUM_MAX_OBJECT;i++)
    {
        ASSERT(sizeof(ListObjets[i].indexInWorld) == 2);
        fwrite(&ListObjets[i].indexInWorld,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].bodyNum) == 2);
        fwrite(&ListObjets[i].bodyNum,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].objectType) == 2);
        fwrite(&ListObjets[i].objectType,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].dynFlags) == 2);
        fwrite(&ListObjets[i].dynFlags,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVX1) == 2);
        fwrite(&ListObjets[i].zv.ZVX1,2,1,fHandle);
        ListObjets[i].zv.ZVX1 = (s16)ListObjets[i].zv.ZVX1;

        //    ASSERT(sizeof(actorTable[i].zv.ZVX2) == 2);
        fwrite(&ListObjets[i].zv.ZVX2,2,1,fHandle);
        ListObjets[i].zv.ZVX2 = (s16)ListObjets[i].zv.ZVX2;

        //    ASSERT(sizeof(actorTable[i].zv.ZVY1) == 2);
        fwrite(&ListObjets[i].zv.ZVY1,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVY2) == 2);
        fwrite(&ListObjets[i].zv.ZVY2,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVZ1) == 2);
        fwrite(&ListObjets[i].zv.ZVZ1,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVZ2) == 2);
        fwrite(&ListObjets[i].zv.ZVZ2,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].screenXMin) == 2);
        fwrite(&ListObjets[i].screenXMin,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].screenYMin) == 2);
        fwrite(&ListObjets[i].screenYMin,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].screenXMax) == 2);
        fwrite(&ListObjets[i].screenXMax,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].screenYMax) == 2);
        fwrite(&ListObjets[i].screenYMax,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].roomX) == 2);
        fwrite(&ListObjets[i].roomX,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].roomY) == 2);
        fwrite(&ListObjets[i].roomY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].roomZ) == 2);
        fwrite(&ListObjets[i].roomZ,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].worldX) == 2);
        fwrite(&ListObjets[i].worldX,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].worldY) == 2);
        fwrite(&ListObjets[i].worldY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].worldZ) == 2);
        fwrite(&ListObjets[i].worldZ,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].alpha) == 2);
        fwrite(&ListObjets[i].alpha,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].beta) == 2);
        fwrite(&ListObjets[i].beta,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].gamma) == 2);
        fwrite(&ListObjets[i].gamma,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].room) == 2);
        fwrite(&ListObjets[i].room,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].stage) == 2);
        fwrite(&ListObjets[i].stage,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].lifeMode) == 2);
        fwrite(&ListObjets[i].lifeMode,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].life) == 2);
        fwrite(&ListObjets[i].life,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].CHRONO) == 4);
        fwrite(&ListObjets[i].CHRONO,4,1,fHandle);

        ASSERT(sizeof(ListObjets[i].ROOM_CHRONO) == 4);
        fwrite(&ListObjets[i].ROOM_CHRONO,4,1,fHandle);

        ASSERT(sizeof(ListObjets[i].ANIM) == 2);
        fwrite(&ListObjets[i].ANIM,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animType) == 2);
        fwrite(&ListObjets[i].animType,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animInfo) == 2);
        fwrite(&ListObjets[i].animInfo,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].newAnim) == 2);
        fwrite(&ListObjets[i].newAnim,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].newAnimType) == 2);
        fwrite(&ListObjets[i].newAnimType,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].newAnimInfo) == 2);
        fwrite(&ListObjets[i].newAnimInfo,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].frame) == 2);
        fwrite(&ListObjets[i].frame,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].numOfFrames) == 2);
        fwrite(&ListObjets[i].numOfFrames,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].END_FRAME) == 2);
        fwrite(&ListObjets[i].END_FRAME,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].flagEndAnim) == 2);
        fwrite(&ListObjets[i].flagEndAnim,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].trackMode) == 2);
        fwrite(&ListObjets[i].trackMode,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].trackNumber) == 2);
        fwrite(&ListObjets[i].trackNumber,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].MARK) == 2);
        fwrite(&ListObjets[i].MARK,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].positionInTrack) == 2);
        fwrite(&ListObjets[i].positionInTrack,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].stepX) == 2);
        fwrite(&ListObjets[i].stepX,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].stepY) == 2);
        fwrite(&ListObjets[i].stepY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].stepZ) == 2); // 45
        fwrite(&ListObjets[i].stepZ,2,1,fHandle);

        saveInterpolatedValue(&ListObjets[i].YHandler,fHandle);

        ASSERT(sizeof(ListObjets[i].falling) == 2);
        fwrite(&ListObjets[i].falling,2,1,fHandle);

        saveInterpolatedValue(&ListObjets[i].rotate,fHandle);

        ASSERT(sizeof(ListObjets[i].direction) == 2); 
        fwrite(&ListObjets[i].direction,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].speed) == 2);
        fwrite(&ListObjets[i].speed,2,1,fHandle);

        saveInterpolatedValue(&ListObjets[i].speedChange,fHandle);

        ASSERT(sizeof(ListObjets[i].COL[0]) == 2);
        fwrite(&ListObjets[i].COL[0],2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].COL[1]) == 2);
        fwrite(&ListObjets[i].COL[1],2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].COL[2]) == 2);
        fwrite(&ListObjets[i].COL[2],2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].COL_BY) == 2);
        fwrite(&ListObjets[i].COL_BY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].HARD_DEC) == 2);
        fwrite(&ListObjets[i].HARD_DEC,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].HARD_COL) == 2);
        fwrite(&ListObjets[i].HARD_COL,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].HIT) == 2);
        fwrite(&ListObjets[i].HIT,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].HIT_BY) == 2);
        fwrite(&ListObjets[i].HIT_BY,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animActionType) == 2);
        fwrite(&ListObjets[i].animActionType,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animActionANIM) == 2);
        fwrite(&ListObjets[i].animActionANIM,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animActionFRAME) == 2);
        fwrite(&ListObjets[i].animActionFRAME,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].animActionParam) == 2);
        fwrite(&ListObjets[i].animActionParam,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hitForce) == 2);
        fwrite(&ListObjets[i].hitForce,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hotPointID) == 2);
        fwrite(&ListObjets[i].hotPointID,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hotPoint.x) == 2);
        fwrite(&ListObjets[i].hotPoint.x,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hotPoint.y) == 2);
        fwrite(&ListObjets[i].hotPoint.x,2,1,fHandle);

        ASSERT(sizeof(ListObjets[i].hotPoint.z) == 2);
        fwrite(&ListObjets[i].hotPoint.x,2,1,fHandle);
    }

    fclose(fHandle);

    return 1;
}

int makeSave(int arg0)
{
    return(makeSaveFile(0));
}

