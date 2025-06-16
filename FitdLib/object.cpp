#include "common.h"

int copyObjectToActor(int body, int typeZv, int hardZvIdx, s16 objectType, int x, int y, int z, int stage, int room, int alpha, int beta, int gamma, int anim, int frame, int animtype, int animInfo)
{
    int i;
    int j;
    tObject* actorPtr = objectTable.data();
    char* bodyPtr;
    ZVStruct* zvPtr;

    for(i=0;i<NUM_MAX_OBJECT;i++)
    {
        if(actorPtr->indexInWorld == -1)
            break;

        actorPtr++;
    }

    if(i==NUM_MAX_OBJECT)
        return -1;

    currentProcessedActorPtr = actorPtr;
    currentProcessedActorIdx = i;

    actorPtr->bodyNum = body;
    actorPtr->objectType = objectType;
    actorPtr->stage = stage;
    actorPtr->room = room;
    actorPtr->worldX = actorPtr->roomX = x;
    actorPtr->worldY = actorPtr->roomY = y;
    actorPtr->worldZ = actorPtr->roomZ = z;

    if(room != currentRoom)
    {
        actorPtr->worldX -= (s16)((roomDataTable[currentRoom].worldX - roomDataTable[actorPtr->room].worldX) * 10);
        actorPtr->worldY += (s16)((roomDataTable[currentRoom].worldY - roomDataTable[actorPtr->room].worldY) * 10);
        actorPtr->worldZ += (s16)((roomDataTable[currentRoom].worldZ - roomDataTable[actorPtr->room].worldZ) * 10);
    }

    actorPtr->alpha = alpha;
    actorPtr->beta = beta;
    actorPtr->gamma = gamma;

    actorPtr->dynFlags = 1;

    actorPtr->ANIM = anim;
    actorPtr->frame = frame;

    actorPtr->animType = animtype;
    actorPtr->animInfo = animInfo;

    actorPtr->END_FRAME = 1;
    actorPtr->flagEndAnim = 1;
    actorPtr->newAnim = -1;
    actorPtr->newAnimType = 0;
    actorPtr->newAnimInfo = -1;

    actorPtr->stepX = 0;
    actorPtr->stepY = 0;
    actorPtr->stepZ = 0;

    for(j=0;j<3;j++)
    {
        actorPtr->COL[j] = -1;
    }

    actorPtr->COL_BY = -1;
    actorPtr->HARD_DEC = -1;
    actorPtr->HARD_COL = -1;

    if(g_gameId != AITD1)
    {
        actorPtr->hardMat = -1;
    }

    actorPtr->rotate.startValue = 0;
    actorPtr->rotate.endValue = 0;
    actorPtr->rotate.numSteps = 0;

    actorPtr->YHandler.startValue = 0;
    actorPtr->YHandler.endValue = 0;
    actorPtr->YHandler.numSteps = 0;

    actorPtr->falling = 0;

    actorPtr->direction = 0;

    actorPtr->speed = 0;

    actorPtr->trackMode = 0;
    actorPtr->trackNumber = -1;

    actorPtr->animActionType = 0;
    actorPtr->HIT = -1;
    actorPtr->HIT_BY = -1;

    if(body != -1)
    {
        bodyPtr = HQR_Get(HQ_Bodys,actorPtr->bodyNum);

        if(anim != -1)
        {
            char* animPtr = HQR_Get(listAnim,anim);

            SetAnimObjet(frame,animPtr,bodyPtr);

            actorPtr->numOfFrames = GetNbFramesAnim(animPtr);
            actorPtr->flagEndAnim = 0;
            actorPtr->objectType |= AF_ANIMATED;

            //			computeScreenBox(actorPtr->field_22 + actorPtr->field_5A, actorPtr->field_24 + actorPtr->field_5C, actorPtr->anim + actorPtr->field_5E, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, bodyPtr);

            if(BBox3D1<0)
                BBox3D1 = 0;

            if(BBox3D3>319)
                BBox3D3 = 319;

            if(BBox3D2<0)
                BBox3D2 = 0;

            if(BBox3D4>199)
                BBox3D4 = 199;

            actorPtr->screenXMin = BBox3D1;
            actorPtr->screenYMin = BBox3D2;
            actorPtr->screenXMax = BBox3D3;
            actorPtr->screenYMax = BBox3D4;
        }
        else
        {
            if(!(actorPtr->objectType & AF_DRAWABLE))
            {
                actorPtr->objectType &= ~AF_ANIMATED; // do not animate an object that is invisible
            }
        }
    }

    startChrono(&actorPtr->ROOM_CHRONO);
    startChrono(&actorPtr->CHRONO);

    zvPtr = &actorPtr->zv;

    switch(typeZv)
    {
    case 0:
        {
            if(body!=-1)
            {
                getZvMax(bodyPtr,zvPtr);
            }
            else
            {
                makeDefaultZV(zvPtr);
            }
            break;
        }
    case 1:
        {
            if(body!=-1)
            {
                GiveZVObjet(bodyPtr,zvPtr);
            }
            else
            {
                makeDefaultZV(zvPtr);
            }
            break;
        }
    case 2:
        {
            if(body!=-1)
            {
                getZvCube(bodyPtr,zvPtr);
            }
            else
            {
                makeDefaultZV(zvPtr);
            }
            break;
        }
    case 3:
        {
            if(body!=-1)
            {
                getZvRot(bodyPtr,zvPtr,alpha,beta,gamma);
            }
            else
            {
                makeDefaultZV(zvPtr);
            }
            break;
        }
    case 4: // HARD_ZV
        {
			bool bFound = false;

            for(int hardColIdx=0;hardColIdx<roomDataTable[room].numHardCol;hardColIdx++)
            {
				if((roomDataTable[room].hardColTable[hardColIdx].type == 9) && (roomDataTable[room].hardColTable[hardColIdx].parameter == hardZvIdx))
				{
					copyZv(&roomDataTable[room].hardColTable[hardColIdx].zv, zvPtr);

                    x = 0;
                    y = 0;
                    z = 0;

                    actorPtr->worldX = actorPtr->roomX = zvPtr->ZVX1/2 + zvPtr->ZVX2/2;
					actorPtr->worldY = actorPtr->roomY = zvPtr->ZVY1/2 + zvPtr->ZVY2/2;
					actorPtr->worldZ = actorPtr->roomZ = zvPtr->ZVZ1/2 + zvPtr->ZVZ2/2;

                    if(room != currentRoom)
                    {
                        actorPtr->worldX -= (roomDataTable[currentRoom].worldX - roomDataTable[room].worldX) * 10;
                        actorPtr->worldY += (roomDataTable[currentRoom].worldY - roomDataTable[room].worldY) * 10;
                        actorPtr->worldZ += (roomDataTable[currentRoom].worldZ - roomDataTable[room].worldZ) * 10;
                    }

					bFound = true;
                    break;
                }
            }

			if(!bFound)
			{
				makeDefaultZV(zvPtr);
			}

            break;
        }
    default:
        {
            printf("Unsupported ZV type in copyObjectToActor\n");
            printf("var1: %d\n", typeZv);
            ASSERT(0);
            assert(0);
            break;
        }
    }

    zvPtr->ZVX1 += x;
    zvPtr->ZVX2 += x;

    zvPtr->ZVY1 += y;
    zvPtr->ZVY2 += y;

    zvPtr->ZVZ1 += z;
    zvPtr->ZVZ2 += z;

    return(i);
}
