#include "common.h"
#include "math.h"

#define	TL_INIT_COOR		0
#define	TL_GOTO			1
#define	TL_END			2
#define	TL_REPEAT		3
#define	TL_MARK			4
#define	TL_WALK			5 // removed in AITD2+
#define	TL_RUN			6 // removed in AITD2+
#define	TL_STOP			7
#define	TL_BACK			8 // removed in AITD2+
#define	TL_SET_ANGLE		9
#define	TL_COL_OFF		10
#define	TL_COL_ON		11
#define	TL_SET_DIST		12
#define	TL_DEC_OFF		13
#define	TL_DEC_ON		14
#define	TL_GOTO_3D		15
#define	TL_MEMO_COOR		16
#define	TL_GOTO_3DX		17
#define	TL_GOTO_3DZ		18
#define	TL_ANGLE		19
#define	TL_CLOSE		20

// AITD2+ has removed a few entries, so covert the enum to match
s16 convertTrackMacro(s16 inTrackMacro) {
    if (g_gameId == AITD1) {
        return inTrackMacro;
    }

    if (inTrackMacro <= 4)
        return inTrackMacro;

    if (inTrackMacro >= 6)
        return inTrackMacro + 3;

    assert(inTrackMacro == 5);
    return TL_STOP;
}

int makeProportional(int x1, int x2, int y1, int y2)
{
    return x1 + ((x2 - x1) * y2) / y1;
}

int computeAngleModificatorToPositionSub1(int ax)
{
    int xOut;
    int yOut;

    Rotate(ax,0,1000,&xOut,&yOut);

    yOut *= angleCompZ;
    xOut *= angleCompX;

    yOut -= xOut;

    if(yOut==0)
        return(0);

    if(yOut>0)
        return(1);
    else
        return(-1);
}

int CapObjet(int x1,int z1, int beta, int x2, int z2)
{
    int resultMin;
    int resultMax;

    angleCompX = x2 - x1;
    angleCompZ = z2 - z1;
    angleCompBeta = beta;

    resultMin = computeAngleModificatorToPositionSub1(beta - 4);
    resultMax = computeAngleModificatorToPositionSub1(beta + 4);

    if(resultMax == -1 && resultMin == 1) // in the middle
    {
        return(computeAngleModificatorToPositionSub1(beta));
    }
    else
    {
        return(((resultMax+resultMin)+1)>>1);
    }
}

void GereManualRot(int param)
{
    if(localJoyD&4)
    {
        if(currentProcessedActorPtr->direction != 1)
        {
            currentProcessedActorPtr->rotate.numSteps = 0;
        }

        currentProcessedActorPtr->direction = 1;

        if(currentProcessedActorPtr->rotate.numSteps == 0)
        {
            int oldBeta = currentProcessedActorPtr->beta;

            if(currentProcessedActorPtr->speed == 0)
            {
                InitRealValue(oldBeta,oldBeta+0x100,param/2,&currentProcessedActorPtr->rotate);
            }
            else
            {
                InitRealValue(oldBeta,oldBeta+0x100,param,&currentProcessedActorPtr->rotate);
            }
        }

        currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
    }
    if(localJoyD&8)
    {
        if(currentProcessedActorPtr->direction != -1)
        {
            currentProcessedActorPtr->rotate.numSteps = 0;
        }

        currentProcessedActorPtr->direction = -1;

        if(currentProcessedActorPtr->rotate.numSteps == 0)
        {
            int oldBeta = currentProcessedActorPtr->beta;

            if(currentProcessedActorPtr->speed == 0)
            {
                InitRealValue(oldBeta,oldBeta-0x100,param/2,&currentProcessedActorPtr->rotate);
            }
            else
            {
                InitRealValue(oldBeta,oldBeta-0x100,param,&currentProcessedActorPtr->rotate);
            }
        }

        currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
    }
    if(!(localJoyD&0xC))
    {
        currentProcessedActorPtr->direction = 0;
        currentProcessedActorPtr->rotate.numSteps = 0;
    }
}

#define DISTANCE_TO_POINT_TRESSHOLD 400

unsigned int lastTimeForward = 0;

char* getRoomLink(unsigned int room1, unsigned int room2)
{
    int i;
    s16 numOfZones;
    char* zoneData = (char*)getRoomData(room1);  
    char* bestZone;

    zoneData += *(s16*)(zoneData);
    numOfZones = *(s16*)zoneData;
    zoneData+=2;

    bestZone = zoneData;

    for(i=0;i<numOfZones;i++)
    {
        if(*(s16*)(zoneData+14) == 4)
        {
            bestZone = zoneData;

            if(*(s16*)(zoneData+12) == room2)
            {
                return bestZone;
            }
        }

        zoneData += 16;
    }

    return bestZone;
}

void processTrack(void)
{
    switch(currentProcessedActorPtr->trackMode)
    {
    case 1: // manual
        {
            GereManualRot(60);
            if(localJoyD&1) // forward
            {
                if(timer - lastTimeForward < 10 && currentProcessedActorPtr->speed != 4) // start running ?
                {
                    currentProcessedActorPtr->speed = 5;
                }
                else
                {
                    if(currentProcessedActorPtr->speed == 0 || currentProcessedActorPtr->speed == -1)
                    {
                        currentProcessedActorPtr->speed = 4;
                    }
                }

                /*        if(currentProcessedActorPtr->speed>0 && currentProcessedActorPtr->speed<4)
                currentProcessedActorPtr->speed = 5; */


                lastTimeForward = timer;
            }
            else
            {
                if((currentProcessedActorPtr->speed > 0) && (currentProcessedActorPtr->speed<=4))
                {
                    currentProcessedActorPtr->speed--;
                }
                else
                {
                    currentProcessedActorPtr->speed = 0;
                }
            }

            if(localJoyD&2) // backward
            {
                if(currentProcessedActorPtr->speed == 0 || currentProcessedActorPtr->speed >= 4)
                    currentProcessedActorPtr->speed = -1;

                if(currentProcessedActorPtr->speed == 5)
                    currentProcessedActorPtr->speed = 0;
            }

            break;
        }
    case 2: // follow
        {
            int followedActorIdx = ListWorldObjets[currentProcessedActorPtr->trackNumber].objIndex;

            if(followedActorIdx == -1)
            {
                currentProcessedActorPtr->direction = 0;
                currentProcessedActorPtr->speed = 0;
            }
            else
            {
                tObject* followedActorPtr = &ListObjets[followedActorIdx];

                int targetRoomNumber = followedActorPtr->room;
                int targetX = followedActorPtr->roomX;
                int targetY = followedActorPtr->roomY;
                int targetZ = followedActorPtr->roomZ;
                int angleModif;

                if (g_gameId == AITD1) {
                    // in AITD1, if the target entity is in a different room, the entity would aim for the center of the trigger zone to get to that room
                    if (currentProcessedActorPtr->room != targetRoomNumber)
                    {
                        char* link = getRoomLink(currentProcessedActorPtr->room, targetRoomNumber);

                        targetX = *(s16*)(link + 0) + (((*(s16*)(link + 2)) - (*(s16*)(link + 0))) / 2);
                        targetY = *(s16*)(link + 4) + (((*(s16*)(link + 6)) - (*(s16*)(link + 4))) / 2);
                        targetZ = *(s16*)(link + 8) + (((*(s16*)(link + 10)) - (*(s16*)(link + 8))) / 2);
                    }
                }
                else {
                    // This was changed from AITD2+ to aim for the target in world-space
                    if((followedActorPtr->room >= 0) && (currentProcessedActorPtr->room>=0))
                    {
                        const auto& targetRoom = roomDataTable[followedActorPtr->room];
                        const auto& sourceRoom = roomDataTable[currentProcessedActorPtr->room];

                        targetX -= (sourceRoom.worldX - targetRoom.worldX) * 10;
                        targetZ += (sourceRoom.worldZ - targetRoom.worldZ) * 10;
                    }
                }

                angleModif = CapObjet( currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
                    currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
                    currentProcessedActorPtr->beta, targetX, targetZ);

                if( (currentProcessedActorPtr->rotate.numSteps == 0) || (currentProcessedActorPtr->direction != angleModif) )
                {
                    InitRealValue( currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif * 256), 60, &currentProcessedActorPtr->rotate);
                }

                currentProcessedActorPtr->direction = angleModif;

                if( currentProcessedActorPtr->direction == 0 )
                {
                    currentProcessedActorPtr->rotate.numSteps = 0;
                }
                else
                {
                    currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
                }

                currentProcessedActorPtr->speed = 4;

            }
            break;
        }
    case 3: // track
        {
            char* trackPtr = HQR_Get(listTrack,currentProcessedActorPtr->trackNumber);
            trackPtr+=currentProcessedActorPtr->positionInTrack * 2;

            s16 trackMacro = convertTrackMacro(*(s16*)trackPtr);
            trackPtr += 2;

            //printf("Track macro %X\n",trackMacro);

            switch(trackMacro)
            {
            case TL_INIT_COOR: // warp
                {
                    int roomNumber = *(s16*)(trackPtr);
                    trackPtr += 2;

                    if(currentProcessedActorPtr->room != roomNumber)
                    {
                        if(currentCameraTargetActor == currentProcessedActorIdx)
                        {
                            FlagChangeSalle = 1;
                            NewNumSalle = roomNumber;
                        }

                        currentProcessedActorPtr->room = roomNumber;
                    }

                    currentProcessedActorPtr->zv.ZVX1 -= currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
                    currentProcessedActorPtr->zv.ZVX2 -= currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
                    currentProcessedActorPtr->zv.ZVY1 -= currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
                    currentProcessedActorPtr->zv.ZVY2 -= currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
                    currentProcessedActorPtr->zv.ZVZ1 -= currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;
                    currentProcessedActorPtr->zv.ZVZ2 -= currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

                    currentProcessedActorPtr->worldX = currentProcessedActorPtr->roomX = *(s16*)(trackPtr);
                    trackPtr += 2;
                    currentProcessedActorPtr->worldY = currentProcessedActorPtr->roomY = *(s16*)(trackPtr);
                    trackPtr += 2;
                    currentProcessedActorPtr->worldZ = currentProcessedActorPtr->roomZ = *(s16*)(trackPtr);
                    trackPtr += 2;

                    currentProcessedActorPtr->worldX -= (s16)((roomDataTable[currentRoom].worldX - roomDataTable[currentProcessedActorPtr->room].worldX) * 10);
                    currentProcessedActorPtr->worldY += (s16)((roomDataTable[currentRoom].worldY - roomDataTable[currentProcessedActorPtr->room].worldY) * 10);
                    currentProcessedActorPtr->worldZ += (s16)((roomDataTable[currentRoom].worldZ - roomDataTable[currentProcessedActorPtr->room].worldZ) * 10);

                    currentProcessedActorPtr->zv.ZVX1 += currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
                    currentProcessedActorPtr->zv.ZVX2 += currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
                    currentProcessedActorPtr->zv.ZVY1 += currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
                    currentProcessedActorPtr->zv.ZVY2 += currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
                    currentProcessedActorPtr->zv.ZVZ1 += currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;
                    currentProcessedActorPtr->zv.ZVZ2 += currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

                    currentProcessedActorPtr->speed = 0;
                    currentProcessedActorPtr->direction = 0;
                    currentProcessedActorPtr->rotate.numSteps = 0;
                    currentProcessedActorPtr->positionInTrack += 5;

                    break;
                }
            case TL_GOTO: // goToPosition
                {
                    int roomNumber = *(s16*)(trackPtr);
                    int x;
                    int y;
                    int z;
                    unsigned int distanceToPoint;

                    trackPtr += 2;

                    x = *(s16*)(trackPtr);
                    trackPtr += 2;
                    y = 0;
                    z = *(s16*)(trackPtr);
                    trackPtr += 2;

                    if(roomNumber != currentProcessedActorPtr->room)
                    {
                        // TODO: fix bug here...
                        x -= (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[roomNumber].worldX) * 10;
                        z += (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[roomNumber].worldZ) * 10;
                    }

                    distanceToPoint = GiveDistance2D( currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
                        currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
                        x,z );


                    if(distanceToPoint >= DISTANCE_TO_POINT_TRESSHOLD) // not yet at position
                    {
                        int angleModif = CapObjet( currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
                            currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
                            currentProcessedActorPtr->beta,
                            x,z );

                        if((currentProcessedActorPtr->rotate.numSteps == 0) || (currentProcessedActorPtr->direction != angleModif))
                        {
                            InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif * 64), 15, &currentProcessedActorPtr->rotate);
                        }

                        currentProcessedActorPtr->direction = angleModif;

                        if(!angleModif)
                        {
                            currentProcessedActorPtr->rotate.numSteps = 0;
                        }
                        else
                        {
                            currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
                        }
                    }
                    else // reached position
                    {
                        currentProcessedActorPtr->positionInTrack += 4;
                    }

                    break;
                }
            case TL_GOTO_3D: // goToPosition
            {
                int roomNumber = *(s16*)(trackPtr);
                int x;
                int y;
                int z;
                unsigned int distanceToPoint;

                trackPtr += 2;

                x = *(s16*)(trackPtr);
                trackPtr += 2;
                y = *(s16*)(trackPtr);
                trackPtr += 2;
                z = *(s16*)(trackPtr);
                trackPtr += 2;
                int time = *(s16*)(trackPtr);
                trackPtr += 2;

                if (roomNumber != currentProcessedActorPtr->room)
                {
                    // TODO: fix bug here...
                    x -= (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[roomNumber].worldX) * 10;
                    y += (roomDataTable[currentProcessedActorPtr->room].worldY - roomDataTable[roomNumber].worldY) * 10;
                    z += (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[roomNumber].worldZ) * 10;
                }

                // reached position?
                if (y == currentProcessedActorPtr->roomY)
                {
                    int distance = GiveDistance2D(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX, currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ, x, z);
                    if (distance < DISTANCE_TO_POINT_TRESSHOLD) {
                        currentProcessedActorPtr->positionInTrack += 6;
                        break;
                    }
                }

                {
                    int direction = CapObjet(
                        currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
                        currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
                        currentProcessedActorPtr->beta,
                        x, z);

                    if (currentProcessedActorPtr->YHandler.numSteps == 0)
                    {
                        InitRealValue(0, y - (currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY), time, &currentProcessedActorPtr->YHandler);
                    }

                    if ((currentProcessedActorPtr->rotate.numSteps == 0) || (currentProcessedActorPtr->direction != direction))
                    {
                        InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (direction * 256), 60, &currentProcessedActorPtr->rotate);
                    }

                    currentProcessedActorPtr->direction = direction;

                    if (!direction)
                    {
                        currentProcessedActorPtr->rotate.numSteps = 0;
                    }
                    else
                    {
                        currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
                    }
                }

                break;
            }
            case TL_END: // stop
                {
                    currentProcessedActorPtr->speed = 0;
                    currentProcessedActorPtr->trackNumber = -1;
                    InitDeplacement(0,0);
                    break;
                }
            case TL_REPEAT:
                {
                    currentProcessedActorPtr->positionInTrack = 0;
                    break;
                }
            case TL_MARK: // MARK
                {
                    currentProcessedActorPtr->MARK = *(s16*)(trackPtr);
                    trackPtr += 2;
                    currentProcessedActorPtr->positionInTrack += 2;
                    break;
                }
            case TL_WALK:
                {
                    currentProcessedActorPtr->speed = 4;
                    currentProcessedActorPtr->positionInTrack++;
                    break;
                }
            case TL_RUN:
                {
                    currentProcessedActorPtr->speed = 5;
                    currentProcessedActorPtr->positionInTrack++;
                    break;
                }
            case TL_STOP:
                {
                    // Stop only has a meaning in AITD1 where it means stop walking
                    if (g_gameId == AITD1)
                    {
                        currentProcessedActorPtr->speed = 0;
                        currentProcessedActorPtr->positionInTrack++;
                    }
                    break;
                }
            case TL_SET_ANGLE: // TL_SET_ANGLE
                {
                    int betaDif = *(s16*)(trackPtr);
                    trackPtr += 2;

                    if(((currentProcessedActorPtr->beta - betaDif)&1023) > 512)
                    {
                        currentProcessedActorPtr->direction = 1; // left
                    }
                    else
                    {
                        currentProcessedActorPtr->direction = -1; // right
                    }

                    if(!currentProcessedActorPtr->rotate.numSteps)
                    {
                        InitRealValue(currentProcessedActorPtr->beta, betaDif, 120, &currentProcessedActorPtr->rotate);
                    }

                    currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);

                    if(currentProcessedActorPtr->beta == betaDif)
                    {
                        currentProcessedActorPtr->direction = 0;

                        currentProcessedActorPtr->positionInTrack+=2;
                    }

                    break;
                }
            case TL_COL_OFF:
                {
                    currentProcessedActorPtr->dynFlags &= ~1;
                    currentProcessedActorPtr->positionInTrack++;
                    break;
                }
            case TL_COL_ON:
                {
                    currentProcessedActorPtr->dynFlags |= 1;
                    currentProcessedActorPtr->positionInTrack++;
                    break;
                }
            case TL_DEC_OFF: // background collision off
                {
                    currentProcessedActorPtr->objectType &= ~AF_TRIGGER;
                    currentProcessedActorPtr->positionInTrack++;
                    break;
                }
            case TL_DEC_ON: // background collision on
                {
                    currentProcessedActorPtr->objectType |= AF_TRIGGER;
                    currentProcessedActorPtr->positionInTrack++;
                    break;
                }
            case TL_MEMO_COOR:
                {
                    int objNum = currentProcessedActorPtr->indexInWorld;

                    ListWorldObjets[objNum].x = currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
                    ListWorldObjets[objNum].y = currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
                    ListWorldObjets[objNum].z = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

                    currentProcessedActorPtr->positionInTrack++;

                    break;
                }
            case TL_GOTO_3DX: // walk up/down stairs on X
                {
                    int x;
                    int y;
                    int z;
                    int objX;
                    int objY;
                    int objZ;

                    x = *(s16*)(trackPtr);
                    trackPtr += 2;
                    y = *(s16*)(trackPtr);
                    trackPtr += 2;
                    z = *(s16*)(trackPtr);
                    trackPtr += 2;

                    objX = ListWorldObjets[currentProcessedActorPtr->indexInWorld].x;
                    objY = ListWorldObjets[currentProcessedActorPtr->indexInWorld].y;
                    objZ = ListWorldObjets[currentProcessedActorPtr->indexInWorld].z;

                    if(   currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY < y - 100
                        ||  currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY > y + 100)
                    {
                        int propX = makeProportional(objY, y, x - objX, (currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX) - objX);

                        int difY = propX - currentProcessedActorPtr->worldY;
                        int angleModif;

                        currentProcessedActorPtr->worldY += difY;
                        currentProcessedActorPtr->roomY += difY;
                        currentProcessedActorPtr->zv.ZVY1 += difY;
                        currentProcessedActorPtr->zv.ZVY2 += difY;

                        angleModif = CapObjet( currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
                            currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
                            currentProcessedActorPtr->beta,
                            x,z );

                        if(!currentProcessedActorPtr->rotate.numSteps || currentProcessedActorPtr->direction != angleModif)
                        {
                            InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif<<8), 60, &currentProcessedActorPtr->rotate);
                        }

                        currentProcessedActorPtr->direction = angleModif;

                        if(angleModif)
                        {
                            currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
                        }
                        else
                        {
                            currentProcessedActorPtr->rotate.numSteps = 0;
                        }

                    }
                    else
                    {
                        int difY = y - currentProcessedActorPtr->worldY;

                        currentProcessedActorPtr->stepY = 0;
                        currentProcessedActorPtr->worldY += difY;
                        currentProcessedActorPtr->roomY += difY;
                        currentProcessedActorPtr->zv.ZVY1 += difY;
                        currentProcessedActorPtr->zv.ZVY2 += difY;

                        currentProcessedActorPtr->positionInTrack +=4;
                    }

                    break;
                }
            case TL_GOTO_3DZ: // walk up/down stairs on Z
                {
                    int x;
                    int y;
                    int z;
                    int objX;
                    int objY;
                    int objZ;

                    x = *(s16*)(trackPtr);
                    trackPtr += 2;
                    y = *(s16*)(trackPtr);
                    trackPtr += 2;
                    z = *(s16*)(trackPtr);
                    trackPtr += 2;

                    objX = ListWorldObjets[currentProcessedActorPtr->indexInWorld].x;
                    objY = ListWorldObjets[currentProcessedActorPtr->indexInWorld].y;
                    objZ = ListWorldObjets[currentProcessedActorPtr->indexInWorld].z;

                    if(   currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY < y - 100
                        ||  currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY > y + 100)
                    {
                        int propZ = makeProportional(objY, y, z - objZ, (currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ) - objZ);

                        int difY = propZ - currentProcessedActorPtr->worldY;

                        int angleModif;

                        currentProcessedActorPtr->worldY += difY;
                        currentProcessedActorPtr->roomY += difY;
                        currentProcessedActorPtr->zv.ZVY1 += difY;
                        currentProcessedActorPtr->zv.ZVY2 += difY;

                        angleModif = CapObjet( currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
                            currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
                            currentProcessedActorPtr->beta,
                            x,z );

                        if(!currentProcessedActorPtr->rotate.numSteps || currentProcessedActorPtr->direction != angleModif)
                        {
                            InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif<<8), 60, &currentProcessedActorPtr->rotate);
                        }

                        currentProcessedActorPtr->direction = angleModif;

                        if(angleModif)
                        {
                            currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
                        }
                        else
                        {
                            currentProcessedActorPtr->rotate.numSteps = 0;
                        }

                    }
                    else
                    {
                        int difY = y - currentProcessedActorPtr->worldY;

                        currentProcessedActorPtr->stepY = 0;
                        currentProcessedActorPtr->worldY += difY;
                        currentProcessedActorPtr->roomY += difY;
                        currentProcessedActorPtr->zv.ZVY1 += difY;
                        currentProcessedActorPtr->zv.ZVY2 += difY;

                        currentProcessedActorPtr->positionInTrack +=4;
                    }

                    break;
                }
            case TL_ANGLE: // rotate
                {
                    currentProcessedActorPtr->alpha = *(s16*)(trackPtr);
                    trackPtr += 2;
                    currentProcessedActorPtr->beta = *(s16*)(trackPtr);
                    trackPtr += 2;
                    currentProcessedActorPtr->gamma = *(s16*)(trackPtr);
                    trackPtr += 2;

                    currentProcessedActorPtr->direction = 0;

                    currentProcessedActorPtr->positionInTrack +=4;

                    break;
                }
            default:
                {
                    printf("Unknown track macro %X\n",trackMacro);
                    assert(0);
                    break;
                }
            }

            break;
        }
    }

    currentProcessedActorPtr->beta &= 0x3FF;
}

