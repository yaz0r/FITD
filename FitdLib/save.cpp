#include "common.h"

unsigned int currentSaveEntrySize;

void* getSaveEntry(int index)
{
    currentSaveEntrySize = saveTable[index].size;

    return(saveTable[index].ptr);
}

void loadInterpolatedValue(interpolatedValue* pRotateStruct, FILE* fHandle)
{
    ASSERT(sizeof(pRotateStruct->oldAngle) == 2);
    fread(&pRotateStruct->oldAngle,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->newAngle) == 2);
    fread(&pRotateStruct->newAngle,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->param) == 2);
    fread(&pRotateStruct->param,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->timeOfRotate) == 4);
    fread(&pRotateStruct->timeOfRotate,2,1,fHandle);
}

void saveInterpolatedValue(interpolatedValue* pRotateStruct, FILE* fHandle)
{
    ASSERT(sizeof(pRotateStruct->oldAngle) == 2);
    fwrite(&pRotateStruct->oldAngle,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->newAngle) == 2);
    fwrite(&pRotateStruct->newAngle,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->param) == 2);
    fwrite(&pRotateStruct->param,2,1,fHandle);

    ASSERT(sizeof(pRotateStruct->timeOfRotate) == 4);
    fwrite(&pRotateStruct->timeOfRotate,2,1,fHandle);
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

    initEngine();
    initVars();

    fseek(fHandle,8,SEEK_SET);

    fread(&var28,4,1,fHandle);

    var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | (( var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);

    fseek(fHandle,var28,SEEK_SET);

    ASSERT(sizeof(currentRoom) == 2);
    fread(&currentRoom,2,1,fHandle);

    ASSERT(sizeof(g_currentFloor) == 2);
    fread(&g_currentFloor,2,1,fHandle);

    ASSERT(sizeof(currentCamera) == 2);
    fread(&currentCamera,2,1,fHandle);

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
        ASSERT(sizeof(worldObjects[i].objIndex) == 2);
        fread(&worldObjects[i].objIndex,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].body) == 2);
        fread(&worldObjects[i].body,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].flags) == 2);
        fread(&worldObjects[i].flags,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].typeZV) == 2);
        fread(&worldObjects[i].typeZV,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].foundBody) == 2);
        fread(&worldObjects[i].foundBody,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].foundName) == 2);
        fread(&worldObjects[i].foundName,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].flags2) == 2);
        fread(&worldObjects[i].flags2,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].foundLife) == 2);
        fread(&worldObjects[i].foundLife,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].x) == 2);
        fread(&worldObjects[i].x,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].y) == 2);
        fread(&worldObjects[i].y,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].z) == 2);
        fread(&worldObjects[i].z,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].alpha) == 2);
        fread(&worldObjects[i].alpha,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].beta) == 2);
        fread(&worldObjects[i].beta,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].gamma) == 2);
        fread(&worldObjects[i].gamma,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].stage) == 2);
        fread(&worldObjects[i].stage,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].room) == 2);
        fread(&worldObjects[i].room,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].lifeMode) == 2);
        fread(&worldObjects[i].lifeMode,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].life) == 2);
        fread(&worldObjects[i].life,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].floorLife) == 2);
        fread(&worldObjects[i].floorLife,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].anim) == 2);
        fread(&worldObjects[i].anim,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].frame) == 2);
        fread(&worldObjects[i].frame,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].animType) == 2);
        fread(&worldObjects[i].animType,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].animInfo) == 2);
        fread(&worldObjects[i].animInfo,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].trackMode) == 2);
        fread(&worldObjects[i].trackMode,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].trackNumber) == 2);
        fread(&worldObjects[i].trackNumber,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].positionInTrack) == 2);
        fread(&worldObjects[i].positionInTrack,2,1,fHandle);
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

    ASSERT(sizeof(giveUp) == 2);
    fread(&giveUp,2,1,fHandle);

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

    var_E = currentCamera;

    loadFloor(g_currentFloor);
    currentCamera = -1;
    loadRoom(currentRoom);
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
        configureHqrHero(listBody,listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
        configureHqrHero(listAnim,listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
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
        ASSERT(sizeof(objectTable[i].indexInWorld) == 2);
        fread(&objectTable[i].indexInWorld,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].bodyNum) == 2);
        fread(&objectTable[i].bodyNum,2,1,fHandle);

        ASSERT(sizeof(objectTable[i]._flags) == 2);
        fread(&objectTable[i]._flags,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].dynFlags) == 2);
        fread(&objectTable[i].dynFlags,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVX1) == 2);
        fread(&objectTable[i].zv.ZVX1,2,1,fHandle);
        objectTable[i].zv.ZVX1 = (s16)objectTable[i].zv.ZVX1;

        //    ASSERT(sizeof(actorTable[i].zv.ZVX2) == 2);
        fread(&objectTable[i].zv.ZVX2,2,1,fHandle);
        objectTable[i].zv.ZVX2 = (s16)objectTable[i].zv.ZVX2;

        //    ASSERT(sizeof(actorTable[i].zv.ZVY1) == 2);
        fread(&objectTable[i].zv.ZVY1,2,1,fHandle);
        objectTable[i].zv.ZVY1 = (s16)objectTable[i].zv.ZVY1;

        //    ASSERT(sizeof(actorTable[i].zv.ZVY2) == 2);
        fread(&objectTable[i].zv.ZVY2,2,1,fHandle);
        objectTable[i].zv.ZVY2 = (s16)objectTable[i].zv.ZVY2;

        //    ASSERT(sizeof(actorTable[i].zv.ZVZ1) == 2);
        fread(&objectTable[i].zv.ZVZ1,2,1,fHandle);
        objectTable[i].zv.ZVZ1 = (s16)objectTable[i].zv.ZVZ1;

        //    ASSERT(sizeof(actorTable[i].zv.ZVZ2) == 2);
        fread(&objectTable[i].zv.ZVZ2,2,1,fHandle);
        objectTable[i].zv.ZVZ2 = (s16)objectTable[i].zv.ZVZ2;

        ASSERT(sizeof(objectTable[i].screenXMin) == 2);
        fread(&objectTable[i].screenXMin,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].screenYMin) == 2);
        fread(&objectTable[i].screenYMin,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].screenXMax) == 2);
        fread(&objectTable[i].screenXMax,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].screenYMax) == 2);
        fread(&objectTable[i].screenYMax,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].roomX) == 2);
        fread(&objectTable[i].roomX,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].roomY) == 2);
        fread(&objectTable[i].roomY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].roomZ) == 2);
        fread(&objectTable[i].roomZ,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].worldX) == 2);
        fread(&objectTable[i].worldX,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].worldY) == 2);
        fread(&objectTable[i].worldY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].worldZ) == 2);
        fread(&objectTable[i].worldZ,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].alpha) == 2);
        fread(&objectTable[i].alpha,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].beta) == 2);
        fread(&objectTable[i].beta,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].gamma) == 2);
        fread(&objectTable[i].gamma,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].room) == 2);
        fread(&objectTable[i].room,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].stage) == 2);
        fread(&objectTable[i].stage,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].lifeMode) == 2);
        fread(&objectTable[i].lifeMode,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].life) == 2);
        fread(&objectTable[i].life,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].CHRONO) == 4);
        fread(&objectTable[i].CHRONO,4,1,fHandle);

        ASSERT(sizeof(objectTable[i].ROOM_CHRONO) == 4);
        fread(&objectTable[i].ROOM_CHRONO,4,1,fHandle);

        ASSERT(sizeof(objectTable[i].ANIM) == 2);
        fread(&objectTable[i].ANIM,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animType) == 2);
        fread(&objectTable[i].animType,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animInfo) == 2);
        fread(&objectTable[i].animInfo,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].newAnim) == 2);
        fread(&objectTable[i].newAnim,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].newAnimType) == 2);
        fread(&objectTable[i].newAnimType,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].newAnimInfo) == 2);
        fread(&objectTable[i].newAnimInfo,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].FRAME) == 2);
        fread(&objectTable[i].FRAME,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].numOfFrames) == 2);
        fread(&objectTable[i].numOfFrames,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].END_FRAME) == 2);
        fread(&objectTable[i].END_FRAME,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].END_ANIM) == 2);
        fread(&objectTable[i].END_ANIM,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].trackMode) == 2);
        fread(&objectTable[i].trackMode,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].trackNumber) == 2);
        fread(&objectTable[i].trackNumber,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].MARK) == 2);
        fread(&objectTable[i].MARK,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].positionInTrack) == 2);
        fread(&objectTable[i].positionInTrack,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].stepX) == 2);
        fread(&objectTable[i].stepX,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].stepY) == 2);
        fread(&objectTable[i].stepY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].stepZ) == 2); // 45
        fread(&objectTable[i].stepZ,2,1,fHandle);

        loadInterpolatedValue(&objectTable[i].YHandler,fHandle);

        ASSERT(sizeof(objectTable[i].falling) == 2);
        fread(&objectTable[i].falling,2,1,fHandle);

        loadInterpolatedValue(&objectTable[i].rotate,fHandle);

        ASSERT(sizeof(objectTable[i].direction) == 2); 
        fread(&objectTable[i].direction,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].speed) == 2);
        fread(&objectTable[i].speed,2,1,fHandle);

        loadInterpolatedValue(&objectTable[i].speedChange,fHandle);

        ASSERT(sizeof(objectTable[i].COL[0]) == 2);
        fread(&objectTable[i].COL[0],2,1,fHandle);

        ASSERT(sizeof(objectTable[i].COL[1]) == 2);
        fread(&objectTable[i].COL[1],2,1,fHandle);

        ASSERT(sizeof(objectTable[i].COL[2]) == 2);
        fread(&objectTable[i].COL[2],2,1,fHandle);

        ASSERT(sizeof(objectTable[i].COL_BY) == 2);
        fread(&objectTable[i].COL_BY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].HARD_DEC) == 2);
        fread(&objectTable[i].HARD_DEC,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].HARD_COL) == 2);
        fread(&objectTable[i].HARD_COL,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].HIT) == 2);
        fread(&objectTable[i].HIT,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].HIT_BY) == 2);
        fread(&objectTable[i].HIT_BY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animActionType) == 2);
        fread(&objectTable[i].animActionType,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animActionANIM) == 2);
        fread(&objectTable[i].animActionANIM,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animActionFRAME) == 2);
        fread(&objectTable[i].animActionFRAME,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animActionParam) == 2);
        fread(&objectTable[i].animActionParam,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hitForce) == 2);
        fread(&objectTable[i].hitForce,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hotPointID) == 2);
        fread(&objectTable[i].hotPointID,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hotPoint.x) == 2);
        fread(&objectTable[i].hotPoint.x,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hotPoint.y) == 2);
        fread(&objectTable[i].hotPoint.x,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hotPoint.z) == 2);
        fread(&objectTable[i].hotPoint.x,2,1,fHandle);
    }
    fclose(fHandle);

    for(i=0;i<NUM_MAX_OBJECT;i++)
    {
        if(objectTable[i].indexInWorld != -1 && objectTable[i].bodyNum != -1)
        {
            char* bodyPtr = HQR_Get(listBody,objectTable[i].bodyNum);

            if(objectTable[i].ANIM != -1)
            {
                char* animPtr = HQR_Get(listAnim,objectTable[i].ANIM);
                SetAnimObjet(objectTable[i].FRAME,animPtr,bodyPtr);
            }
        }
    }

    startGameVar1 = var_E;

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
		for(i=0;i<NUM_MAX_OBJECT;i++)
		{
			if(objectTable[i].indexInWorld == -1);

			if(objectTable[i].ANIM == 4 )
			{
				CVars[getCVarsIdx(FOG_FLAG)] = 0;
				HQ_Free_Malloc(HQ_Memory,objectTable[i].FRAME);
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

    ASSERT(sizeof(currentCamera) == 2);
    fwrite(&currentCamera,2,1,fHandle);

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
        ASSERT(sizeof(worldObjects[i].objIndex) == 2);
        fwrite(&worldObjects[i].objIndex,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].body) == 2);
        fwrite(&worldObjects[i].body,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].flags) == 2);
        fwrite(&worldObjects[i].flags,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].typeZV) == 2);
        fwrite(&worldObjects[i].typeZV,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].foundBody) == 2);
        fwrite(&worldObjects[i].foundBody,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].foundName) == 2);
        fwrite(&worldObjects[i].foundName,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].flags2) == 2);
        fwrite(&worldObjects[i].flags2,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].foundLife) == 2);
        fwrite(&worldObjects[i].foundLife,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].x) == 2);
        fwrite(&worldObjects[i].x,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].y) == 2);
        fwrite(&worldObjects[i].y,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].z) == 2);
        fwrite(&worldObjects[i].z,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].alpha) == 2);
        fwrite(&worldObjects[i].alpha,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].beta) == 2);
        fwrite(&worldObjects[i].beta,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].gamma) == 2);
        fwrite(&worldObjects[i].gamma,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].stage) == 2);
        fwrite(&worldObjects[i].stage,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].room) == 2);
        fwrite(&worldObjects[i].room,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].lifeMode) == 2);
        fwrite(&worldObjects[i].lifeMode,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].life) == 2);
        fwrite(&worldObjects[i].life,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].floorLife) == 2);
        fwrite(&worldObjects[i].floorLife,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].anim) == 2);
        fwrite(&worldObjects[i].anim,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].frame) == 2);
        fwrite(&worldObjects[i].frame,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].animType) == 2);
        fwrite(&worldObjects[i].animType,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].animInfo) == 2);
        fwrite(&worldObjects[i].animInfo,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].trackMode) == 2);
        fwrite(&worldObjects[i].trackMode,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].trackNumber) == 2);
        fwrite(&worldObjects[i].trackNumber,2,1,fHandle);

        ASSERT(sizeof(worldObjects[i].positionInTrack) == 2);
        fwrite(&worldObjects[i].positionInTrack,2,1,fHandle);
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

    ASSERT(sizeof(giveUp) == 2);
    fwrite(&giveUp,2,1,fHandle);

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
        ASSERT(sizeof(objectTable[i].indexInWorld) == 2);
        fwrite(&objectTable[i].indexInWorld,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].bodyNum) == 2);
        fwrite(&objectTable[i].bodyNum,2,1,fHandle);

        ASSERT(sizeof(objectTable[i]._flags) == 2);
        fwrite(&objectTable[i]._flags,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].dynFlags) == 2);
        fwrite(&objectTable[i].dynFlags,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVX1) == 2);
        fwrite(&objectTable[i].zv.ZVX1,2,1,fHandle);
        objectTable[i].zv.ZVX1 = (s16)objectTable[i].zv.ZVX1;

        //    ASSERT(sizeof(actorTable[i].zv.ZVX2) == 2);
        fwrite(&objectTable[i].zv.ZVX2,2,1,fHandle);
        objectTable[i].zv.ZVX2 = (s16)objectTable[i].zv.ZVX2;

        //    ASSERT(sizeof(actorTable[i].zv.ZVY1) == 2);
        fwrite(&objectTable[i].zv.ZVY1,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVY2) == 2);
        fwrite(&objectTable[i].zv.ZVY2,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVZ1) == 2);
        fwrite(&objectTable[i].zv.ZVZ1,2,1,fHandle);

        //    ASSERT(sizeof(actorTable[i].zv.ZVZ2) == 2);
        fwrite(&objectTable[i].zv.ZVZ2,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].screenXMin) == 2);
        fwrite(&objectTable[i].screenXMin,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].screenYMin) == 2);
        fwrite(&objectTable[i].screenYMin,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].screenXMax) == 2);
        fwrite(&objectTable[i].screenXMax,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].screenYMax) == 2);
        fwrite(&objectTable[i].screenYMax,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].roomX) == 2);
        fwrite(&objectTable[i].roomX,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].roomY) == 2);
        fwrite(&objectTable[i].roomY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].roomZ) == 2);
        fwrite(&objectTable[i].roomZ,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].worldX) == 2);
        fwrite(&objectTable[i].worldX,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].worldY) == 2);
        fwrite(&objectTable[i].worldY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].worldZ) == 2);
        fwrite(&objectTable[i].worldZ,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].alpha) == 2);
        fwrite(&objectTable[i].alpha,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].beta) == 2);
        fwrite(&objectTable[i].beta,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].gamma) == 2);
        fwrite(&objectTable[i].gamma,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].room) == 2);
        fwrite(&objectTable[i].room,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].stage) == 2);
        fwrite(&objectTable[i].stage,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].lifeMode) == 2);
        fwrite(&objectTable[i].lifeMode,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].life) == 2);
        fwrite(&objectTable[i].life,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].CHRONO) == 4);
        fwrite(&objectTable[i].CHRONO,4,1,fHandle);

        ASSERT(sizeof(objectTable[i].ROOM_CHRONO) == 4);
        fwrite(&objectTable[i].ROOM_CHRONO,4,1,fHandle);

        ASSERT(sizeof(objectTable[i].ANIM) == 2);
        fwrite(&objectTable[i].ANIM,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animType) == 2);
        fwrite(&objectTable[i].animType,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animInfo) == 2);
        fwrite(&objectTable[i].animInfo,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].newAnim) == 2);
        fwrite(&objectTable[i].newAnim,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].newAnimType) == 2);
        fwrite(&objectTable[i].newAnimType,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].newAnimInfo) == 2);
        fwrite(&objectTable[i].newAnimInfo,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].FRAME) == 2);
        fwrite(&objectTable[i].FRAME,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].numOfFrames) == 2);
        fwrite(&objectTable[i].numOfFrames,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].END_FRAME) == 2);
        fwrite(&objectTable[i].END_FRAME,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].END_ANIM) == 2);
        fwrite(&objectTable[i].END_ANIM,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].trackMode) == 2);
        fwrite(&objectTable[i].trackMode,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].trackNumber) == 2);
        fwrite(&objectTable[i].trackNumber,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].MARK) == 2);
        fwrite(&objectTable[i].MARK,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].positionInTrack) == 2);
        fwrite(&objectTable[i].positionInTrack,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].stepX) == 2);
        fwrite(&objectTable[i].stepX,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].stepY) == 2);
        fwrite(&objectTable[i].stepY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].stepZ) == 2); // 45
        fwrite(&objectTable[i].stepZ,2,1,fHandle);

        saveInterpolatedValue(&objectTable[i].YHandler,fHandle);

        ASSERT(sizeof(objectTable[i].falling) == 2);
        fwrite(&objectTable[i].falling,2,1,fHandle);

        saveInterpolatedValue(&objectTable[i].rotate,fHandle);

        ASSERT(sizeof(objectTable[i].direction) == 2); 
        fwrite(&objectTable[i].direction,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].speed) == 2);
        fwrite(&objectTable[i].speed,2,1,fHandle);

        saveInterpolatedValue(&objectTable[i].speedChange,fHandle);

        ASSERT(sizeof(objectTable[i].COL[0]) == 2);
        fwrite(&objectTable[i].COL[0],2,1,fHandle);

        ASSERT(sizeof(objectTable[i].COL[1]) == 2);
        fwrite(&objectTable[i].COL[1],2,1,fHandle);

        ASSERT(sizeof(objectTable[i].COL[2]) == 2);
        fwrite(&objectTable[i].COL[2],2,1,fHandle);

        ASSERT(sizeof(objectTable[i].COL_BY) == 2);
        fwrite(&objectTable[i].COL_BY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].HARD_DEC) == 2);
        fwrite(&objectTable[i].HARD_DEC,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].HARD_COL) == 2);
        fwrite(&objectTable[i].HARD_COL,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].HIT) == 2);
        fwrite(&objectTable[i].HIT,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].HIT_BY) == 2);
        fwrite(&objectTable[i].HIT_BY,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animActionType) == 2);
        fwrite(&objectTable[i].animActionType,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animActionANIM) == 2);
        fwrite(&objectTable[i].animActionANIM,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animActionFRAME) == 2);
        fwrite(&objectTable[i].animActionFRAME,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].animActionParam) == 2);
        fwrite(&objectTable[i].animActionParam,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hitForce) == 2);
        fwrite(&objectTable[i].hitForce,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hotPointID) == 2);
        fwrite(&objectTable[i].hotPointID,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hotPoint.x) == 2);
        fwrite(&objectTable[i].hotPoint.x,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hotPoint.y) == 2);
        fwrite(&objectTable[i].hotPoint.x,2,1,fHandle);

        ASSERT(sizeof(objectTable[i].hotPoint.z) == 2);
        fwrite(&objectTable[i].hotPoint.x,2,1,fHandle);
    }

    fclose(fHandle);

    return 1;
}

int makeSave(int arg0)
{
    return(makeSaveFile(0));
}

