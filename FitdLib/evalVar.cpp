#include "common.h"

int getPosRelTable[] = {4,1,8,2,4,1,8,0};

int getMatrix(int param1, int actorIdx, int param2)
{
    unsigned char* matrixPtr = (unsigned char*)HQR_Get(HQ_Matrices,param1);

    int matrixWidth = *matrixPtr++;
    int matrixHeigh = *matrixPtr++;

    matrixPtr+=(objectTable[actorIdx].hardMat-1)*matrixWidth;
    matrixPtr+=(objectTable[param2].hardMat-1);

	if(g_gameId == AITD3)
	{
		return *(unsigned char*)matrixPtr;
	}
    return *(char*)matrixPtr;
}

int getPosRel(tObject* actor1, tObject* actor2)
{
    int beta1 = actor1->beta;
    int counter = 3;
    ZVStruct localZv;
    int centerX;
    int centerZ;

    if(beta1 >= 0x80 && beta1 < 0x180)
    {
        counter = 2;
    }

    if(beta1 >= 0x180 && beta1 < 0x280)
    {
        counter = 1;
    }

    if(beta1 >= 0x280 && beta1 < 0x380)
    {
        counter = 0;
    }


    copyZv(&actor2->zv, &localZv);

    if(actor1->room != actor2->room)
    {
        getZvRelativePosition(&localZv, actor2->room, actor1->room);
    }

    centerX = (localZv.ZVX1 + localZv.ZVX2) / 2;
    centerZ = (localZv.ZVZ1 + localZv.ZVZ2) / 2;

    if(actor1->zv.ZVZ2 >= centerZ && actor1->zv.ZVZ1 <= centerZ)
    {
        if(actor1->zv.ZVX2 < centerX)
        {
            counter++;
        }
        else
        {
            if(actor1->zv.ZVX1 <= centerX)
            {
                return(0);
            }
            else
            {
                counter+=3;
            }
        }
    }
    else
        if(actor1->zv.ZVX2 >= centerX || actor1->zv.ZVX1 <= centerX)
        {
            if(actor1->zv.ZVZ2 < centerZ )
            {
                counter+=2;
            }
            else
            {
                if(actor1->zv.ZVZ1 <= centerZ)
                {
                    return(0);
                }
            }
        }
        else
        {
            return(0);
        }

        return(getPosRelTable[counter]);
}

int calcDist(int X1, int Y1, int Z1, int X2, int Y2, int Z2)
{
    int Xdist = abs(X1 - X2);
    int Ydist = abs(Y1 - Y2);
    int Zdist = abs(Z1 - Z2);

    return(Xdist + Ydist + Zdist); // recheck overflow
}

int testZvEndAnim(tObject* actorPtr,char* animPtr, int param)
{
    s16 var_16;
    s16 var_14;
    s16 var_E = 0;
    s16 var_12 = 0;
    s16 var_10 = param;
    s16 var_18;
    ZVStruct localZv;

    ASSERT(actorPtr);
    ASSERT(animPtr);

    var_16 = *(s16*)(animPtr);
    animPtr += 2;
    var_14 = *(s16*)(animPtr);
    animPtr += 2;

    for(var_18 = 0; var_18 < var_16; var_18 ++)
    {
        animPtr += 2;
        var_12 += *(s16*)animPtr;
        animPtr += 2;
        animPtr += 2;
        var_E += *(s16*)animPtr; // step depth
        animPtr += 2;

        animPtr+= var_14*8;
    }

    copyZv(&actorPtr->zv, &localZv);

    walkStep(0,var_E,actorPtr->beta);

    localZv.ZVX1 += animMoveX;
    localZv.ZVX2 += animMoveX;
    localZv.ZVY1 += var_10;
    localZv.ZVY2 += var_10;
    localZv.ZVZ1 += animMoveZ;
    localZv.ZVZ2 += animMoveZ;

    if(AsmCheckListCol(&localZv, &roomDataTable[actorPtr->room]))
    {
        return(0);
    }

    localZv.ZVY1 += 100;
    localZv.ZVY2 += 100;

    if(AsmCheckListCol(&localZv, &roomDataTable[actorPtr->room]))
    {
        return(1);
    }

    return(0);
}

int evalVar(const char* name)
{
    int var1;

    if(g_gameId >= JACK)
    {
        return evalVar2(name);
    }

    var1 = *(s16*)(currentLifePtr);
    currentLifePtr+=2;

    if(var1 == -1)
    {
        int temp = *(s16*)(currentLifePtr);
        currentLifePtr+=2;

        return(temp);
    }
    else
        if(var1 == 0)
        {
            int temp = *(s16*)(currentLifePtr);
            currentLifePtr+=2;

            return(vars[temp]);
        }
        else
        {
            tObject* actorPtr = currentLifeActorPtr;
            int actorIdx = currentLifeActorIdx;

            if(var1 & 0x8000)
            {
                int objectNumber;

                objectNumber = *(s16*)currentLifePtr;

                actorIdx = ListWorldObjets[objectNumber].objIndex;

                currentLifePtr+=2;
                
                if(actorIdx==-1)
                {
                    actorPtr = nullptr;
                    switch(var1 & 0x7FFF)
                    {
                    case 0x1F:
                        {
                            return(ListWorldObjets[objectNumber].room);
                            break;
                        }
                    case 0x26:
                        {
                            return(ListWorldObjets[objectNumber].stage);
                            break;
                        }
                    default:
                        {
                            printf("Unsupported evalVar %X when actor not in room !\n", var1 & 0x7FFF);
                            assert(0);
                        }
                    }
                }
                else {
                    actorPtr = &objectTable[actorIdx];
                }
                
            }
            {

                var1&=0x7FFF;     

                var1--;

                switch(var1)
                {
                case 0x0:
                    {
                        int temp1 = actorPtr->COL[0];

                        if(temp1 != -1)
                        {
                            return(objectTable[temp1].indexInWorld);
                        }
                        else
                        {
                            return(-1);
                        }
                        break;
                    }
                case 0x1:
                    {
                        return(actorPtr->HARD_DEC);
                        break;
                    }
                case 0x2:
                    {
                        return(actorPtr->HARD_COL);
                        break;
                    }
                case 0x3:
                    {
                        int temp = actorPtr->HIT;

                        if(temp == -1)
                        {
                            return(-1);
                        }
                        else
                        {
                            return(objectTable[temp].indexInWorld);
                        }

                        break;
                    }
                case 0x4:
                    {
                        int temp = actorPtr->HIT_BY;

                        if(temp == -1)
                        {
                            return(-1);
                        }
                        else
                        {
                            return(objectTable[temp].indexInWorld);
                        }

                        break;
                    }
                case 0x5:
                    {
                        return(actorPtr->ANIM);
                        break;
                    }
                case 0x6:
                    {
                        return(actorPtr->flagEndAnim);
                        break;
                    }
                case 0x7:
                    {
                        return(actorPtr->frame);
                        break;
                    }
                case 0x8:
                    {
                        return(actorPtr->END_FRAME);
                        break;
                    }
                case 0x9:
                    {
                        return(actorPtr->bodyNum);
                        break;
                    }
                case 0xA: // MARK
                    {
                        return(actorPtr->MARK);
                        break;
                    }
                case 0xB: // NUM_TRACK
                    {
                        return(actorPtr->trackNumber);
                        break;
                    }
                case 0xC: // CHRONO
                    {
                        return(evalChrono(&actorPtr->CHRONO) /60); // recheck
                        break;
                    }
                case 0xD:
                    {
                        return(evalChrono(&actorPtr->ROOM_CHRONO) / 60); // recheck
                        break;
                    }
                case 0xE: // DIST
                    {
                        int actorNumber = ListWorldObjets[*(s16*)currentLifePtr].objIndex;
                        currentLifePtr+=2;

                        if(actorNumber == -1)
                        {
                            return(32000);
                        }
                        else
                        {
                            int tempX = objectTable[actorNumber].worldX;
                            int tempY = objectTable[actorNumber].worldY;
                            int tempZ = objectTable[actorNumber].worldZ;

                            return(calcDist(actorPtr->worldX, actorPtr->worldY, actorPtr->worldZ, tempX, tempY, tempZ));
                        }

                        break;
                    }
                case 0xF: // COL_BY
                    {
                        if(actorPtr->COL_BY == -1)
                            return(-1);
                        else
                            return(objectTable[actorPtr->COL_BY].indexInWorld);
                        break;
                    }
                case 0x10: // found
                    {
                        if(ListWorldObjets[evalVar()].foundFlag & 0x8000)
                        {
                            return(1);
                        }
                        else
                        {
                            return(0);
                        }

                        break;
                    }
                case 0x11:
                    {
                        return action;
                        break;
                    }
                case 0x12: // POSREL
                    {
                        int objNum;

                        objNum = *(s16*)currentLifePtr;
                        currentLifePtr+=2;

                        if(ListWorldObjets[objNum].objIndex == -1)
                        {
                            return 0;
                        }

                        return (getPosRel(actorPtr, &objectTable[ListWorldObjets[objNum].objIndex]));

                        break;
                    }
                case 0x13:
                    {
                        if(localJoyD & 4)
                            return 4;
                        if(localJoyD & 8)
                            return 8;
                        if(localJoyD & 1)
                            return 1;
                        if(localJoyD & 2)
                            return 2;

                        return 0;
                        break;
                    }
                case 0x14:
                    {
                        return(localClick);
                        break;
                    }
				case 0x15:
					{
						int temp1 = actorPtr->COL[0];
						if(temp1 == -1)
						{
							temp1 = actorPtr->COL_BY;
							if(temp1 == -1)
								return -1;
						}

						return objectTable[temp1].indexInWorld;
						break;
					}
                case 0x16:
                    {
                        return(actorPtr->alpha);
                        break;
                    }
                case 0x17:
                    {
                        return(actorPtr->beta);
                        break;
                    }
                case 0x18:
                    {
                        return(actorPtr->gamma);
                        break;
                    }
                case 0x19:
                    {
                        return(inHandTable[currentInventory]);
                        break;
                    }
                case 0x1A:
                    {
                        return(actorPtr->hitForce);
                        break;
                    }
                case 0x1B:
                    {
                        return(*(u16*)(((NumCamera+6)*2)+cameraPtr));
                        break;
                    }
                case 0x1C:
                    {
                        int temp = *(s16*)currentLifePtr;
                        currentLifePtr+=2;
                        return(rand()%temp);
                        break;
                    }
                case 0x1D:
                    {
                        return(actorPtr->falling);
                        break;
                    }
                case 0x1E:
                    {
                        return(actorPtr->room);
                        break;
                    }
                case 0x1F:
                    {
                        return(actorPtr->life);
                        break;
                    }
                case 0x20:
                    {
                        int objNum;

                        objNum = *(s16*)currentLifePtr;
                        currentLifePtr+=2;

                        if(ListWorldObjets[objNum].foundFlag & 0xC000)
                        {
                            return(1);
                        }
                        else
                        {
                            return(0);
                        }

                        break;
                    }
                case 0x21:
                    {
                        return(actorPtr->roomY);
                        break;
                    }
                case 0x22: // TEST_ZV_END_ANIM
                    {
                        int temp1;
                        int temp2;

                        temp1 = *(s16*)currentLifePtr;
                        currentLifePtr +=2;
                        temp2 = *(s16*)currentLifePtr;
                        currentLifePtr +=2;

                        return(testZvEndAnim(actorPtr,HQR_Get(HQ_Anims,temp1),temp2));

                        break;
                    }
                case 0x23: // TODO: music
                    {
                        return(currentMusic);
                        break;
                    }
                case 0x24:
                    {
                        int temp = CVars[*(s16*)currentLifePtr];
                        currentLifePtr+=2;
                        return(temp);
                        break;
                    }
                case 0x25:
                    {
                        return(actorPtr->stage);
                        break;
                    }
                case 0x26: // THROW
                    {
                        int objNum;

                        objNum = *(s16*)currentLifePtr;
                        currentLifePtr+=2;

                        if(ListWorldObjets[objNum].foundFlag & 0x1000)
                        {
                            return 1;
                        }
                        else
                        {
                            return 0;
                        }
                        break;
                    }
                default:
                    {
                        printf("Unhandled test type %X in evalVar\n",var1);
                        assert(0);
                        break;
                    }
                }
            }
        }
}

int evalVar2(const char* name)
{
    int var1;

    var1 = *(s16*)(currentLifePtr);
    currentLifePtr+=2;

    if(var1 == -1)
    {
        int temp = *(s16*)(currentLifePtr);
        currentLifePtr+=2;

        if (name)
            appendFormated("%s:", name);
        appendFormated("%d, ", temp);

        return(temp);
    }
    else
        if(var1 == 0)
        {
            int temp = *(s16*)(currentLifePtr);
            currentLifePtr+=2;

            if (name)
                appendFormated("%s:", name);
            appendFormated("vars[%d], ", temp);

            return(vars[temp]);
        }
        else
        {
            tObject* actorPtr = currentLifeActorPtr;
            int actorIdx = currentLifeActorIdx;

            if(var1 & 0x8000)
            {
                int objectNumber;

                objectNumber = *(s16*)currentLifePtr;

                actorIdx = ListWorldObjets[objectNumber].objIndex;

                currentLifePtr+=2;
                actorPtr = nullptr;

                if(actorIdx==-1)
                {
                    switch(var1 & 0x7FFF)
                    {
                    case 0x1F:
                        {
                            if (name)
                                appendFormated("%s:", name);
                            appendFormated("worldObjects[%d].room, ", objectNumber);

                            return(ListWorldObjets[objectNumber].room);
                            break;
                        }
                    case 0x24:
                        {
                            if (name)
                                appendFormated("%s:", name);
                            appendFormated("worldObjects[%d].stage, ", objectNumber);

                            return(ListWorldObjets[objectNumber].stage);
                            break;
                        }
                    default:
                        {
                            printf("Unsupported evalVar2 %X when actor not in room !\n", var1 & 0x7FFF);
                            //assert(0);
							return false;
                        }
                    }
                }
                else {
                    actorPtr = &objectTable[actorIdx];
                }
            }
            {

                var1&=0x7FFF;     

                var1--;

                switch(var1)
                {
                case 0x0:
                    {
                        int temp1 = actorPtr->COL[0];

                        if (name)
                            appendFormated("%s:", name);
                        appendFormated("objectTable[%d].COL, ", temp1);

                        if(temp1 != -1)
                        {
                            return(objectTable[temp1].indexInWorld);
                        }
                        else
                        {
                            return(-1);
                        }
                        break;
                    }
                case 0x1:
                    {
                        if (name)
                            appendFormated("%s:", name);
                        appendFormated("HARD_DEC, ");
                        return(actorPtr->HARD_DEC);
                        break;
                    }
                case 0x2:
                    {
                        if (name)
                            appendFormated("%s:", name);
                        appendFormated("HARD_COL, ");

                        return(actorPtr->HARD_COL);
                        break;
                    }
                case 0x3:
                    {
                        if (name)
                            appendFormated("%s:", name);
                        appendFormated("HIT, ");
                        int temp = actorPtr->HIT;

                        if(temp == -1)
                        {
                            return(-1);
                        }
                        else
                        {
                            return(objectTable[temp].indexInWorld);
                        }

                        break;
                    }
                case 0x4:
                    {
                        int temp = actorPtr->HIT_BY;

                        if(temp == -1)
                        {
                            return(-1);
                        }
                        else
                        {
                            return(objectTable[temp].indexInWorld);
                        }

                        break;
                    }
                case 0x5:
                    {
                        return(actorPtr->ANIM);
                        break;
                    }
                case 0x6:
                    {
                        return(actorPtr->flagEndAnim);
                        break;
                    }
                case 0x7:
                    {
                        return(actorPtr->frame);
                        break;
                    }
                case 0x8:
                    {
                        return(actorPtr->END_FRAME);
                        break;
                    }
                case 0x9:
                    {
                        return(actorPtr->bodyNum);
                        break;
                    }
                case 0xA: // MARK
                    {
                        return(actorPtr->MARK);
                        break;
                    }
                case 0xB: // NUM_TRACK
                    {
                        return(actorPtr->trackNumber);
                        break;
                    }
                case 0xC: // CHRONO
                    {
                        return(evalChrono(&actorPtr->CHRONO) /60); // recheck
                        break;
                    }
                case 0xD:
                    {
                        return(evalChrono(&actorPtr->ROOM_CHRONO) / 60); // recheck
                        break;
                    }
                case 0xE: // DIST
                    {
						int worldObjectIdx = *(s16*)currentLifePtr;
						currentLifePtr+=2;

                        int objectIdx = ListWorldObjets[worldObjectIdx].objIndex;
                        
						int tempX;
						int tempY;
						int tempZ;

                        if(objectIdx == -1)
                        {
							if(ListWorldObjets[worldObjectIdx].room == currentRoom)
							{
								tempX = ListWorldObjets[worldObjectIdx].x;
								tempY = ListWorldObjets[worldObjectIdx].y;
								tempZ = ListWorldObjets[worldObjectIdx].z;
							}
							else
							{
								return(32000);
							}
                        }
                        else
                        {
                            tempX = objectTable[objectIdx].worldX;
                            tempY = objectTable[objectIdx].worldY;
                            tempZ = objectTable[objectIdx].worldZ;
                        }

						return(calcDist(actorPtr->worldX, actorPtr->worldY, actorPtr->worldZ, tempX, tempY, tempZ));

                        break;
                    }
                case 0xF: // COL_BY
                    {
                        if(actorPtr->COL_BY == -1)
                            return(-1);
                        else
                            return(objectTable[actorPtr->COL_BY].indexInWorld);
                        break;
                    }
                case 0x10: // found
                    {
                        if(ListWorldObjets[evalVar()].foundFlag & 0x8000)
                        {
                            return(1);
                        }
                        else
                        {
                            return(0);
                        }

                        break;
                    }
                case 0x11:
                    {
                        return action;
                        break;
                    }
                case 0x12: // POSREL
                    {
                        int objNum;

                        objNum = *(s16*)currentLifePtr;
                        currentLifePtr+=2;

                        if(ListWorldObjets[objNum].objIndex == -1)
                        {
                            return 0;
                        }

                        return (getPosRel(actorPtr, &objectTable[ListWorldObjets[objNum].objIndex]));

                        break;
                    }
                case 0x13:
                    {
                        if(localJoyD & 4)
                            return 4;
                        if(localJoyD & 8)
                            return 8;
                        if(localJoyD & 1)
                            return 1;
                        if(localJoyD & 2)
                            return 2;

                        return 0;
                        break;
                    }
                case 0x14:
                    {
                        return(localClick);
                        break;
                    }
                case 0x15:
                    {
                        int temp1 = actorPtr->COL[0];
                        if(temp1 == -1)
						{
							temp1 = actorPtr->COL_BY;
							if(temp1 == -1)
								return -1;
						}

                        return objectTable[temp1].indexInWorld;
                        break;
                    }
                case 0x16:
                    {
                        return(actorPtr->alpha);
                        break;
                    }
                case 0x17:
                    {
                        return(actorPtr->beta);
                        break;
                    }
                case 0x18:
                    {
                        return(actorPtr->gamma);
                        break;
                    }
                case 0x19:
                    {
                        return(inHandTable[currentInventory]);
                        break;
                    }
                case 0x1A:
                    {
                        return(actorPtr->hitForce);
                        break;
                    }
                case 0x1B:
                    {
                        return(cameraDataTable[NumCamera]-&g_currentFloorCameraData[0]);
                        break;
                    }
                case 0x1C:
                    {
                        int temp = *(s16*)currentLifePtr;
                        currentLifePtr+=2;
                        return(rand()%temp);
                        break;
                    }
                case 0x1D:
                    {
                        return(actorPtr->falling);
                        break;
                    }
                case 0x1E:
                    {
                        return(actorPtr->room);
                        break;
                    }
                case 0x1F:
                    {
                        return(actorPtr->life);
                        break;
                    }
                case 0x20:
                    {
                        int objNum;

                        objNum = *(s16*)currentLifePtr;
                        currentLifePtr+=2;

                        if(ListWorldObjets[objNum].foundFlag & 0xC000)
                        {
                            return(1);
                        }
                        else
                        {
                            return(0);
                        }

                        break;
                    }
                case 0x21: // TODO: music
                    {
                        return(currentMusic);
                        break;
                    }
                case 0x22: // c_var
                    {
                        int temp = CVars[*(s16*)currentLifePtr];
                        currentLifePtr+=2;
                        return(temp);
                        break;
                    }
                case 0x23:
                    {
                        return(actorPtr->stage);
                        break;
                    }
                case 0x24: // THROW
                    {
                        int objNum;

                        objNum = *(s16*)currentLifePtr;
                        currentLifePtr+=2;

                        if(ListWorldObjets[objNum].foundFlag & 0x1000)
                        {
                            return 1;
                        }
                        else
                        {
                            return 0;
                        }
                        break;
                    }
                case 0x25: // get_matrix
                    {
                        int param1;
                        int param2;

                        param1 = *(s16*)currentLifePtr;
                        currentLifePtr+=2;

                        param2 = *(s16*)currentLifePtr;
                        currentLifePtr+=2;

                        return getMatrix(param1,actorIdx,ListWorldObjets[param2].objIndex);
                        break;
                    }
                case 0x26: // hard_mat
                    {
                        return actorPtr->hardMat;
                        break;
                    }
                case 0x27: // TEST_PROTECT
                    {
                        return 1;
                        break;
                    }
                case 0x2A: // related to samples
                    {
                    return 1;
                    break;
                    }
                default:
                    {
                        printf("Unhandled test type %X in evalVar\n",var1);
                        assert(0);
                        return 0;
                        break;
                    }
                }
            }
        }
}

