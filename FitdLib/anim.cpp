#include <unordered_map>

#include "common.h"

int SetAnimObjet(int frame, char* anim, sBody* body)
{
    sAnimation* pAnimation = getAnimationFromPtr(anim);

    s16 bx;

    anim+=2;

    if(frame >= pAnimation->m_numFrames)
    {
        return(0);
    }

    int numGroupsInAnimation = pAnimation->m_numGroups;
    anim+=2;

    int ax;
    if(body->m_flags & INFO_OPTIMISE)
    {
        ax = ((numGroupsInAnimation <<4)+8)*frame;
    }
    else
    {
        ax = ((numGroupsInAnimation +1)<<3)*frame;
    }

    anim+=ax;

    animCurrentTime = *(s16*)anim;
    animKeyframeLength = animCurrentTime;

    if(!(body->m_flags & INFO_ANIM))
    {
        return(0);
    }

    body->startAnim = anim;
    *(u16*)(body->m_scratchBuffer.data() + 4) = timer;

    if(numGroupsInAnimation > body->m_groupOrder.size())
        numGroupsInAnimation = body->m_groupOrder.size();

    anim += 2; // skip key length
    animStepX = *(s16*)(anim); anim += 2;
    animStepY = *(s16*)(anim); anim += 2;
    animStepZ = *(s16*)(anim); anim += 2;

    for(int i=0;i< numGroupsInAnimation;i++)
    {
        body->m_groups[i].m_state.m_type = *(s16*)(anim); anim += 2;
        body->m_groups[i].m_state.m_delta[0] = *(s16*)(anim); anim += 2;
        body->m_groups[i].m_state.m_delta[1] = *(s16*)(anim); anim += 2;
        body->m_groups[i].m_state.m_delta[2] = *(s16*)(anim); anim += 2;

        if(body->m_flags & INFO_OPTIMISE)
        {
            body->m_groups[i].m_state.m_rotateDelta[0] = *(s16*)(anim); anim += 2;
            body->m_groups[i].m_state.m_rotateDelta[1] = *(s16*)(anim); anim += 2;
            body->m_groups[i].m_state.m_rotateDelta[2] = *(s16*)(anim); anim += 2;
            body->m_groups[i].m_state.m_padding = *(s16*)(anim); anim += 2;
        }
    }

    return(1);

}

int InitAnim(int animNum,int animType, int animInfo)
{
    if(animNum == currentProcessedActorPtr->ANIM)
    {
        if(!(currentProcessedActorPtr->objectType & AF_ANIMATED))
        {
            if(currentProcessedActorPtr->objectType & AF_BOXIFY)
            {
                removeFromBGIncrust(currentProcessedActorIdx);
            }

            currentProcessedActorPtr->objectType |= AF_ANIMATED;

            SetAnimObjet(currentProcessedActorPtr->frame, HQR_Get(listAnim,animNum), getBodyFromPtr(HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum)));

            currentProcessedActorPtr->animType = animType;
            currentProcessedActorPtr->animInfo = animInfo;

			if(g_gameId > AITD1)
			{
				currentProcessedActorPtr->frame = 0;
			}

			return 1;
        }
        else
        {
            currentProcessedActorPtr->animType = animType;
            currentProcessedActorPtr->animInfo = animInfo;
            return 0;
        }
    }

    if(animNum == -1)
    {
        currentProcessedActorPtr->newAnim = -2;
        return(1);
    }

    if(!(currentProcessedActorPtr->objectType & AF_ANIMATED))
    {
		currentProcessedActorPtr->objectType |= AF_ANIMATED;

        if(currentProcessedActorPtr->objectType & AF_BOXIFY)
        {
            removeFromBGIncrust(currentProcessedActorIdx);
        }

        SetAnimObjet(0, HQR_Get(listAnim,animNum), getBodyFromPtr(HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum)));

		currentProcessedActorPtr->newAnim = animNum;
		currentProcessedActorPtr->newAnimType = animType;
		currentProcessedActorPtr->newAnimInfo = animInfo;
		if(g_gameId > AITD1)
		{
			currentProcessedActorPtr->frame = 0;
		}
		return 1;
    }

	if(g_gameId == AITD1)
	{
		if(currentProcessedActorPtr->animType & ANIM_UNINTERRUPTABLE)
			return(0);

		if(currentProcessedActorPtr->newAnimType & ANIM_UNINTERRUPTABLE)
			return(0);
	}
	else
	{
		if(currentProcessedActorPtr->animType & ANIM_UNINTERRUPTABLE)
		{
			if(currentProcessedActorPtr->newAnimType & ANIM_UNINTERRUPTABLE)
			{
				return 0;
			}
			else
			{
				currentProcessedActorPtr->animInfo = animNum;
				return 1;
			}
		}
	}

    currentProcessedActorPtr->newAnim = animNum;
    currentProcessedActorPtr->newAnimType = animType;
    currentProcessedActorPtr->newAnimInfo = animInfo;

	if(g_gameId != AITD1)
	{
		currentProcessedActorPtr->frame = 0;
	}

    return(1);
}

int evaluateReal(RealValue* data)
{
	if(!data->numSteps)
		return data->endValue;

	if(timer - data->memoTicks> (unsigned int)data->numSteps)
	{
		data->numSteps = 0;
		return data->endValue;
	}

    s32 valueDiff = data->endValue - data->startValue;
    s32 currentTime = timer - data->memoTicks;
    s32 interpolatedValue = data->startValue + ((valueDiff * currentTime) / data->numSteps);

	return interpolatedValue;
}

int manageFall(int actorIdx, ZVStruct* zvPtr)
{
	int fallResult = 0;
	int i;
	int room = objectTable[actorIdx].room;

	for(i=0;i<NUM_MAX_OBJECT;i++)
	{
		tObject* currentTestedActorPtr = &objectTable[i];

		if(currentTestedActorPtr->indexInWorld != -1 && i != actorIdx)
		{
			ZVStruct* testedZv = &currentTestedActorPtr->zv;

			if(currentTestedActorPtr->room != room)
			{
				ZVStruct localZv;
				copyZv(zvPtr, &localZv);
				getZvRelativePosition(&localZv, room, currentTestedActorPtr->room);

				if(checkZvCollision(&localZv, testedZv))
				{
					objectTable[i].COL_BY = actorIdx;
					fallResult++;
				}
			}
			else
			{
				if(checkZvCollision(zvPtr, testedZv))
				{
					objectTable[i].COL_BY = actorIdx;
					fallResult++;
				}
			}
		}
	}

	return(fallResult);
}

void GereAnim(void)
{
	int oldStepZ=0;
	int oldStepY=0;
	int oldStepX=0;
	int stepZ=0;
	int stepY=0;
	int stepX=0;
	s16 localTable[3];
	ZVStruct zvLocal;
	ZVStruct* zvPtr;

	int newAnim = currentProcessedActorPtr->newAnim;

    if (newAnim != -1) // next anim ?
    {
        if (newAnim == -2) // completely stop anim and add actor to background
        {
            addActorToBgInscrust(currentProcessedActorIdx);
            currentProcessedActorPtr->newAnim = -1;
            currentProcessedActorPtr->newAnimType = 0;
            currentProcessedActorPtr->newAnimInfo = -1;
            currentProcessedActorPtr->flagEndAnim = 1;

            return;
        }
        else {
            if (!(currentProcessedActorPtr->newAnimType & ANIM_RESET)) // Reset
            {
                if (currentProcessedActorPtr->END_FRAME == 0)
                {
                    currentProcessedActorPtr->worldX += currentProcessedActorPtr->stepX;
                    currentProcessedActorPtr->roomX += currentProcessedActorPtr->stepX;

                    currentProcessedActorPtr->worldZ += currentProcessedActorPtr->stepZ;
                    currentProcessedActorPtr->roomZ += currentProcessedActorPtr->stepZ;

                    currentProcessedActorPtr->stepX = 0;
                    currentProcessedActorPtr->stepZ = 0;

                    currentProcessedActorPtr->animNegX = 0;
                    currentProcessedActorPtr->animNegY = 0;
                    currentProcessedActorPtr->animNegZ = 0;
                }

                // TODO: AITD3 has some extra code here to handle bufferAnimCounter

                StockInterAnim(BufferAnim[bufferAnimCounter], getBodyFromPtr(HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum)));

                bufferAnimCounter++;
                if (bufferAnimCounter == NB_BUFFER_ANIM)
                    bufferAnimCounter = 0;

            }
            else {
                ResetStartAnim(getBodyFromPtr(HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum)));
                currentProcessedActorPtr->newAnimType &= ~ANIM_RESET;
            }
            currentProcessedActorPtr->ANIM = newAnim;
            currentProcessedActorPtr->animType = currentProcessedActorPtr->newAnimType;
            currentProcessedActorPtr->animInfo = currentProcessedActorPtr->newAnimInfo;

            currentProcessedActorPtr->newAnim = -1;
            currentProcessedActorPtr->newAnimType = 0;
            currentProcessedActorPtr->newAnimInfo = -1;

            currentProcessedActorPtr->flagEndAnim = 0;
            currentProcessedActorPtr->frame = 0;

            currentProcessedActorPtr->numOfFrames = GetNbFramesAnim(HQR_Get(listAnim, newAnim));
        }
    }

	if(currentProcessedActorPtr->ANIM == -1) // no animation
	{
		currentProcessedActorPtr->END_FRAME = 0;
		if(currentProcessedActorPtr->speed == 0)
		{
			int numObjectCollisions = checkObjectCollisions(currentProcessedActorIdx, &currentProcessedActorPtr->zv);

			for(int i = 0; i<numObjectCollisions; i++)
			{
				objectTable[currentProcessedActorPtr->COL[i]].COL_BY = currentProcessedActorIdx; // collision with current actor
			}

			oldStepY = 0;
			oldStepZ = 0;
			stepX = 0;
			stepZ = 0;
			stepY = 0;
		}
		else
		{
			oldStepX = currentProcessedActorPtr->stepX;
			oldStepY = currentProcessedActorPtr->stepY;
			oldStepZ = currentProcessedActorPtr->stepZ;

			animStepY = 0;
			animStepX = 0;

			animStepZ = evaluateReal(&currentProcessedActorPtr->speedChange);

			walkStep(0,animStepZ,currentProcessedActorPtr->beta);

			stepX = animMoveX - oldStepX;
			stepZ = animMoveZ - oldStepZ;
			stepY = 0;
		}
	}
	else // animation
	{
		oldStepX = currentProcessedActorPtr->stepX;
		oldStepY = currentProcessedActorPtr->stepY;
		oldStepZ = currentProcessedActorPtr->stepZ;

		currentProcessedActorPtr->END_FRAME = SetInterAnimObjet(currentProcessedActorPtr->frame, HQR_Get(listAnim, currentProcessedActorPtr->ANIM), getBodyFromPtr(HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum)));

		walkStep(animStepX,animStepZ,currentProcessedActorPtr->beta);

		stepX = animMoveX+currentProcessedActorPtr->animNegX - oldStepX;
		stepZ = animMoveZ+currentProcessedActorPtr->animNegZ - oldStepZ;

	}

	if(currentProcessedActorPtr->YHandler.numSteps) // currently falling ?
	{
		if(currentProcessedActorPtr->YHandler.numSteps != -1)
		{
			stepY = evaluateReal(&currentProcessedActorPtr->YHandler) - oldStepY;
		}
		else // stop falling
		{
			stepY = currentProcessedActorPtr->YHandler.endValue - oldStepY;

			currentProcessedActorPtr->YHandler.numSteps = 0;
			currentProcessedActorPtr->YHandler.endValue = 0;
			currentProcessedActorPtr->YHandler.startValue = 0;
		}
	}
	else
	{
		stepY = 0;
	}

	memcpy(localTable,currentProcessedActorPtr->COL,6);
	
	if(stepX || stepY || stepZ) // start of movement management
	{
		zvPtr = &currentProcessedActorPtr->zv;
		copyZv(&currentProcessedActorPtr->zv,&zvLocal);

		zvLocal.ZVX1 += stepX;
		zvLocal.ZVX2 += stepX;

		zvLocal.ZVY1 += stepY;
		zvLocal.ZVY2 += stepY;

		zvLocal.ZVZ1 += stepZ;
		zvLocal.ZVZ2 += stepZ;

		if(currentProcessedActorPtr->dynFlags & 1) // hard collision enabled for actor ?
		{
			int numCol = AsmCheckListCol(&zvLocal, &roomDataTable[currentProcessedActorPtr->room]);

			for(int i=0;i<numCol;i++)
			{
				hardColStruct* pHardCol = hardColTable[i];

				if(pHardCol->type == 9)
				{
					currentProcessedActorPtr->HARD_COL = (short)pHardCol->parameter;
				}

				if(pHardCol->type == 3)
				{
					currentProcessedActorPtr->HARD_COL = 255;
				}

				if(g_gameId == AITD1 || (g_gameId>=JACK && (pHardCol->type != 10 || currentProcessedActorIdx != currentCameraTargetActor)))
				{
					if(stepX || stepZ) // move on the X or Z axis ? update to avoid entering the hard col
					{
						//ZVStruct tempZv;

						hardColStepX = stepX;
						hardColStepZ = stepZ;

						handleCollision(zvPtr, &zvLocal, &pHardCol->zv);

						currentProcessedActorPtr->animNegX += hardColStepX - stepX;
						currentProcessedActorPtr->animNegZ += hardColStepZ - stepZ;

						zvLocal.ZVX1 +=  hardColStepX - stepX;
						zvLocal.ZVX2 +=  hardColStepX - stepX;
						zvLocal.ZVZ1 +=  hardColStepZ - stepZ;
						zvLocal.ZVZ2 +=  hardColStepZ - stepZ;

						stepX = hardColStepX;
						stepZ = hardColStepZ;
					}

					if(stepY)
					{
						//assert(0); //not implemented
					}
				}
			}
		}
		else // no hard collision -> just update the flag without performing the position update
		{
			if(AsmCheckListCol(&zvLocal, &roomDataTable[currentProcessedActorPtr->room]))
			{
				currentProcessedActorPtr->HARD_COL = 1;
			}
			else
			{
				currentProcessedActorPtr->HARD_COL = 0;
			}
		}

		int numCol = checkObjectCollisions(currentProcessedActorIdx,&zvLocal); // get the number of actor/actor collision

		for(int j=0;j<numCol;j++) // process the actor/actor collision
		{
			int collisionIndex = currentProcessedActorPtr->COL[j];

			tObject* actorTouchedPtr = &objectTable[collisionIndex];

			actorTouchedPtr->COL_BY = currentProcessedActorIdx;

			ZVStruct* touchedZv = &actorTouchedPtr->zv;

			if(actorTouchedPtr->objectType & AF_FOUNDABLE) // takable
			{
				if(currentProcessedActorPtr->trackMode == 1 /*&& ((gameId == AITD1 && defines.field_1E == 0) || (gameId >= JACK && defines.field_6 == 0))*/) // TODO: check if character isn't dead...
				{
					foundObject(actorTouchedPtr->indexInWorld, 0);
				}
			}
			else
			{
				if(actorTouchedPtr->objectType & AF_MOVABLE) // can be pushed ?
				{
					ZVStruct localZv2;

					bool isPushPossible = true;

					copyZv(touchedZv, &localZv2);

					localZv2.ZVX1 += stepX;
					localZv2.ZVX2 += stepX;

					localZv2.ZVZ1 += stepZ;
					localZv2.ZVZ2 += stepZ;

					if(!AsmCheckListCol(&localZv2, &roomDataTable[currentProcessedActorPtr->room]))
					{
						if(checkObjectCollisions(collisionIndex, &localZv2))
						{
							isPushPossible = false;
						}
					}
					else
					{
						isPushPossible = false;
					}

					if(!isPushPossible)
					{
						if(stepX || stepZ) //if we're trying to move
						{
							if(actorTouchedPtr->room != currentProcessedActorPtr->room)
							{
								ZVStruct localZv3;

								copyZv(touchedZv, &localZv3);

								getZvRelativePosition(&localZv3, actorTouchedPtr->room, currentProcessedActorPtr->room);

								hardColStepX = stepX;
								hardColStepZ = stepZ;

								handleCollision(zvPtr, &zvLocal, &localZv3);

								stepX = hardColStepX;
								stepZ = hardColStepZ;
							}
							else
							{
								hardColStepX = stepX;
								hardColStepZ = stepZ;

								handleCollision(zvPtr, &zvLocal, touchedZv); // manage as hard collision

								stepX = hardColStepX;
								stepZ = hardColStepZ;
							}
						}
					}
					else // push succeed
					{
						if(actorTouchedPtr->objectType & AF_BOXIFY)
						{
							removeFromBGIncrust(collisionIndex);
						}

						actorTouchedPtr->objectType |= AF_ANIMATED;

						actorTouchedPtr->worldX += stepX; // apply push to object
						actorTouchedPtr->worldZ += stepZ;

						actorTouchedPtr->roomX += stepX;
						actorTouchedPtr->roomZ += stepZ;

						copyZv(&localZv2,touchedZv);
					}
				}
				else
				{
					// can't be pushed
					if(currentProcessedActorPtr->dynFlags & 1)
					{
						if(stepX || stepZ) // if moving
						{
							if(actorTouchedPtr->room == currentProcessedActorPtr->room) // same room -> easy case
							{
								hardColStepX = stepX;
								hardColStepZ = stepZ;

								handleCollision(zvPtr, &zvLocal, touchedZv); // manage as hard collision

								stepX = hardColStepX;
								stepZ = hardColStepZ;
							}
							else // different room
							{
								ZVStruct localZv3;

								copyZv(touchedZv, &localZv3);

								getZvRelativePosition(&localZv3, actorTouchedPtr->room, currentProcessedActorPtr->room);

								hardColStepX = stepX;
								hardColStepZ = stepZ;

								handleCollision(zvPtr, &zvLocal, &localZv3); // manage as hard collision

								stepX = hardColStepX;
								stepZ = hardColStepZ;
							}
						}
					}
				}
			}
		} // end of actor/actor collision

		currentProcessedActorPtr->stepX = stepX + oldStepX;
		currentProcessedActorPtr->stepY = stepY + oldStepY;
		currentProcessedActorPtr->stepZ = stepZ + oldStepZ;

		currentProcessedActorPtr->zv.ZVX1 += stepX;
		currentProcessedActorPtr->zv.ZVX2 += stepX;

		currentProcessedActorPtr->zv.ZVY1 += stepY;
		currentProcessedActorPtr->zv.ZVY2 += stepY;

		currentProcessedActorPtr->zv.ZVZ1 += stepZ;
		currentProcessedActorPtr->zv.ZVZ2 += stepZ;
	} // end of movement management

	if(!currentProcessedActorPtr->YHandler.numSteps)
	{
		// fall management ?
		currentProcessedActorPtr->worldY += currentProcessedActorPtr->stepY;
		currentProcessedActorPtr->roomY += currentProcessedActorPtr->stepY;

		currentProcessedActorPtr->stepY = 0;

		if(currentProcessedActorPtr->objectType & AF_FALLABLE)
		{
			zvPtr = &currentProcessedActorPtr->zv;

			copyZv(zvPtr, &zvLocal);

			zvLocal.ZVY2 += 100;

			if(currentProcessedActorPtr->roomY < -10 && !AsmCheckListCol(&zvLocal,&roomDataTable[currentProcessedActorPtr->room]) && !manageFall(currentProcessedActorIdx,&zvLocal))
			{
				InitRealValue(0, 2000, 40, &currentProcessedActorPtr->YHandler);
			}
			else
			{
				currentProcessedActorPtr->falling = 0;
			}
		}
	}
	else
	{
		if((currentProcessedActorPtr->YHandler.numSteps != -1) && (currentProcessedActorPtr->objectType & AF_FALLABLE))
		{
			currentProcessedActorPtr->falling = 1;
		}
	}

	for(int i=0; i<3; i++)
	{
		int collisionIndex = localTable[i];

		if(collisionIndex != -1)
		{
			tObject* actorTouchedPtr = &objectTable[collisionIndex];

			if(actorTouchedPtr->objectType & AF_MOVABLE)
			{
				int i;

				for(i=0;i<3;i++)
				{
					if(currentProcessedActorPtr->COL[i] == collisionIndex)
						break;
				}

				if(i == 3)
				{
					actorTouchedPtr->objectType &= ~AF_ANIMATED;
					addActorToBgInscrust(collisionIndex);
				}
			}
		}
	}

	if(currentProcessedActorPtr->END_FRAME) // key frame change
	{
		currentProcessedActorPtr->frame++;

		if(currentProcessedActorPtr->frame >= currentProcessedActorPtr->numOfFrames) // end of anim ?
		{
			currentProcessedActorPtr->flagEndAnim = 1; // end of anim
			currentProcessedActorPtr->frame = 0; // restart anim

			if(!(currentProcessedActorPtr->animType & 1) && (currentProcessedActorPtr->newAnim == -1)) // is another anim waiting ?
			{
				currentProcessedActorPtr->animType &= 0xFFFD;

				InitAnim(currentProcessedActorPtr->animInfo, 1, -1);
			}
		}
		currentProcessedActorPtr->worldX += currentProcessedActorPtr->stepX;
		currentProcessedActorPtr->roomX += currentProcessedActorPtr->stepX;

		currentProcessedActorPtr->worldZ += currentProcessedActorPtr->stepZ;
		currentProcessedActorPtr->roomZ += currentProcessedActorPtr->stepZ;

		currentProcessedActorPtr->stepX = 0;
		currentProcessedActorPtr->stepZ = 0;

		currentProcessedActorPtr->animNegX = 0;
		currentProcessedActorPtr->animNegY = 0;
		currentProcessedActorPtr->animNegZ = 0;
	}
	else // not the end of anim
	{
		if((currentProcessedActorPtr->ANIM == -1) && (currentProcessedActorPtr->speed != 0) && (currentProcessedActorPtr->speedChange.numSteps == 0))
		{
			currentProcessedActorPtr->worldX += currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->roomX += currentProcessedActorPtr->stepX;

			currentProcessedActorPtr->worldZ += currentProcessedActorPtr->stepZ;
			currentProcessedActorPtr->roomZ += currentProcessedActorPtr->stepZ;

			currentProcessedActorPtr->stepX = 0;
			currentProcessedActorPtr->stepZ = 0;

			InitRealValue(0,currentProcessedActorPtr->speed,60,&currentProcessedActorPtr->speedChange);
		}

		currentProcessedActorPtr->flagEndAnim = 0;
	}
}


void StockInterAnim(std::vector<s16>& buffer, sBody* bodyPtr)
{
    std::vector<s16>::iterator bufferIt = buffer.begin();

    if(bodyPtr->m_flags & INFO_ANIM)
    {
        *(u16*)(bodyPtr->m_scratchBuffer.data()+4) = (u16)timer;
        bodyPtr->startAnim = (char*) & buffer[0];

        bufferIt += 4;

        for(int i=0;i< bodyPtr->m_groups.size();i++)
        {
            bufferIt[0] = bodyPtr->m_groups[i].m_state.m_type;
            bufferIt[1] = bodyPtr->m_groups[i].m_state.m_delta[0];
            bufferIt[2] = bodyPtr->m_groups[i].m_state.m_delta[1];
            bufferIt[3] = bodyPtr->m_groups[i].m_state.m_delta[2];

            bufferIt +=4;

            if(bodyPtr->m_flags & INFO_OPTIMISE)
            {
                bufferIt[0] = bodyPtr->m_groups[i].m_state.m_rotateDelta[0];
                bufferIt[1] = bodyPtr->m_groups[i].m_state.m_rotateDelta[1];
                bufferIt[2] = bodyPtr->m_groups[i].m_state.m_rotateDelta[2];
                bufferIt[3] = bodyPtr->m_groups[i].m_state.m_padding;

                bufferIt +=4;
            }
        }
    }
}

void ResetStartAnim(sBody* bodyPtr) {
    *(u16*)(bodyPtr->m_scratchBuffer.data() + 4) = (u16)timer; // reset timer
    bodyPtr->startAnim = nullptr;
}

s16 GetNbFramesAnim(char* animPtr)
{
    return(*(s16*)animPtr);
}

s16 PatchType(sGroupState* bodyPtr) // local
{
    s16 temp = *(s16*)animVar1;

    animVar1+=2;

    animVar4+=2;

    bodyPtr->m_type = temp;

    return(temp);
}

void PatchInterAngle(s16* value, int bp, int bx) // local
{
    s16 oldRotation = *(s16*)animVar4;
    s16 newRotation;
    s16 diff;

    animVar4+=2;

    newRotation = *(s16*)animVar1;
    animVar1+=2;

    diff = newRotation - oldRotation;

    if(diff == 0)
    {
        *value = newRotation;
    }
    else
    {
        if(diff <= 0x200)
        {
            if(diff >= -0x200)
            {
                *value = ((diff*bp)/bx) + oldRotation;
            }
            else
            {
                newRotation += 0x400;
                newRotation -= oldRotation;

                *value = ((newRotation*bp)/bx) + oldRotation;
            }
        }
        else
        {
            oldRotation += 0x400;
            newRotation -= oldRotation;

            *value = ((newRotation*bp)/bx) + oldRotation;
        }
    }
}

void PatchInterStep(s16* value, int bp, int bx) // local
{
    s16 cx = *(s16*)animVar4;
    s16 ax;
    animVar4+=2;

    ax = *(s16*)animVar1;
    animVar1+=2;

    if(ax == cx)
    {
        *value = ax;
    }
    else
    {
        *value = (((ax - cx)*bp)/bx) + cx;
    }
}

s16 SetInterAnimObjet(int frame, char* animPtr, sBody* pBody)
{
    int numOfBonesInAnim = *(s16*)(animPtr+2);
    u16 keyframeLength;
    u16 timeOfKeyframeStart;
    int ax;
    u16 bx;
    u16 time;
    int bp;
    int flag;

    flag = pBody->m_flags;

    animPtr+=4;

    if(flag&INFO_OPTIMISE)
    {
        animPtr += ((numOfBonesInAnim<<4)+8)*frame; // seek to keyframe
    }
    else
    {
        animPtr += ((numOfBonesInAnim+1)*8)*frame; // seek to keyframe
    }

    // animVar1 = ptr to the current keyFrame
    animVar1 = animPtr;

    keyframeLength = *(u16*)animPtr; // keyframe length

    if(!(pBody->m_flags & INFO_ANIM)) // do not anim if the model can't be animated
    {
        return(0);
    }

    timeOfKeyframeStart = *(u16*)(pBody->m_scratchBuffer.data() + 4); // time of start of keyframe

    char* animBufferPtr = pBody->startAnim;

    if(!animBufferPtr)
    {
        animBufferPtr = animVar1;
    }

    // animVar4 = ptr to previous key frame
    animVar4 = animBufferPtr;

    if(numOfBonesInAnim > pBody->m_groupOrder.size())
    {
        numOfBonesInAnim = pBody->m_groupOrder.size();
    }

    time = (u16)timer - timeOfKeyframeStart;

    bx = keyframeLength;
    bp = time;

    if(time<keyframeLength) // interpolate keyframe
    {
        char* animVar1Backup = animVar1;
        // skip bone 0 anim
        animVar4 += 8; // anim buffer
        animVar1 += 8; // current keyframe ptr

        if(!(flag&INFO_OPTIMISE))
        {
            for (int i = 0; i < numOfBonesInAnim; i++)
            {
                switch(PatchType(&pBody->m_groups[i].m_state))
                {
                case 0: // rotate
                    PatchInterAngle(&pBody->m_groups[i].m_state.m_delta[0], bp, bx);
                    PatchInterAngle(&pBody->m_groups[i].m_state.m_delta[1], bp, bx);
                    PatchInterAngle(&pBody->m_groups[i].m_state.m_delta[2], bp, bx);
                    break;
                case 1: // translate
					PatchInterStep(&pBody->m_groups[i].m_state.m_delta[0], bp, bx);
					PatchInterStep(&pBody->m_groups[i].m_state.m_delta[1], bp, bx);
					PatchInterStep(&pBody->m_groups[i].m_state.m_delta[2], bp, bx);
					break;

                case 2: // zoom
                    PatchInterStep(&pBody->m_groups[i].m_state.m_delta[0], bp, bx);
                    PatchInterStep(&pBody->m_groups[i].m_state.m_delta[1], bp, bx);
                    PatchInterStep(&pBody->m_groups[i].m_state.m_delta[2], bp, bx);
                    break;
                }
            }
        }
        else
        {
            for (int i = 0; i < numOfBonesInAnim; i++)
            {
                switch (PatchType(&pBody->m_groups[i].m_state))
                {
                case 0:
                        animVar4 += 6;
                        animVar1 += 6;
                        break;
                case 1:
                case 2:
                    PatchInterStep(&pBody->m_groups[i].m_state.m_delta[0], bp, bx);
                    PatchInterStep(&pBody->m_groups[i].m_state.m_delta[1], bp, bx);
                    PatchInterStep(&pBody->m_groups[i].m_state.m_delta[2], bp, bx);
                    break;
                }

                PatchInterStep(&pBody->m_groups[i].m_state.m_rotateDelta[0], bp, bx);
                PatchInterStep(&pBody->m_groups[i].m_state.m_rotateDelta[1], bp, bx);
                PatchInterStep(&pBody->m_groups[i].m_state.m_rotateDelta[2], bp, bx);

                animVar4 += 2;
                animVar1 += 2;
            }
        }

        animVar1 = animVar1Backup;

        animVar1+=2;

        animStepX = ((*(s16*)(animVar1))*bp)/bx; // X
        animStepY = ((*(s16*)(animVar1+2))*bp)/bx; // Y
        animStepZ = ((*(s16*)(animVar1+4))*bp)/bx; // Z

        animVar1+=6;

        animCurrentTime = bx;
        animKeyframeLength = bp;
        return(0);
    }
    else // change keyframe
    {
        char* tempBx = animVar1;
        char* si = animVar1;


        si+=8;

        for (int i = 0; i < numOfBonesInAnim; i++)
        {
            pBody->m_groups[i].m_state.m_type = *(s16*)(si);
            pBody->m_groups[i].m_state.m_delta[0] = *(s16*)(si + 2);
            pBody->m_groups[i].m_state.m_delta[1] = *(s16*)(si + 4);
            pBody->m_groups[i].m_state.m_delta[2] = *(s16*)(si + 6);
            si += 8;

            if (flag & INFO_OPTIMISE)
            {
                pBody->m_groups[i].m_state.m_rotateDelta[0] = *(s16*)(si + 2);
                pBody->m_groups[i].m_state.m_rotateDelta[1] = *(s16*)(si + 4);
                pBody->m_groups[i].m_state.m_rotateDelta[2] = *(s16*)(si + 6);
                pBody->m_groups[i].m_state.m_padding = *(s16*)(si);
                si += 8;
            }
        };

        pBody->startAnim = animVar1;

        *(u16*)(pBody->m_scratchBuffer.data()+4) = (u16)timer;

        tempBx+=2;

        animCurrentTime = bx;
        animKeyframeLength = bx;

        animStepX = *(s16*)(tempBx);
        animStepY = *(s16*)(tempBx+2);
        animStepZ = *(s16*)(tempBx+4);

        tempBx += 6;
        return(1);

    }

}
