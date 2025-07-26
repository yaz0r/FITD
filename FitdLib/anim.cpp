#include "common.h"
#include "hybrid.h"

hqrEntryStruct<sAnimation>* HQ_Anims = nullptr;

std::vector<sFrame> BufferAnim;

int SetAnimObjet(int frame, sAnimation* pAnimation, sBody* body)
{
    if(frame >= pAnimation->m_numFrames)
    {
        return(0);
    }

    int numGroupsInAnimation = pAnimation->m_numGroups;
    sFrame& keyframe = pAnimation->m_frames[frame];

    animCurrentTime = keyframe.m_timestamp;
    animKeyframeLength = animCurrentTime;

    if(!(body->m_flags & INFO_ANIM))
    {
        return(0);
    }

    body->startAnim = &keyframe;
    *(u16*)(body->m_scratchBuffer.data() + 4) = timer;

    if(numGroupsInAnimation > body->m_groupOrder.size())
        numGroupsInAnimation = body->m_groupOrder.size();

    animStepX = keyframe.m_animStep.x;
    animStepY = keyframe.m_animStep.y;
    animStepZ = keyframe.m_animStep.z;

    for(int i=0;i< numGroupsInAnimation;i++)
    {
        body->m_groups[i].m_state.m_type = keyframe.m_groups[i].m_type;
        body->m_groups[i].m_state.m_delta = keyframe.m_groups[i].m_delta;

        if(body->m_flags & INFO_OPTIMISE)
        {
            body->m_groups[i].m_state.m_rotateDelta = keyframe.m_groups[i].m_rotateDelta;
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

            SetAnimObjet(currentProcessedActorPtr->frame, HQR_Get(HQ_Anims,animNum), HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum));

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

        SetAnimObjet(0, HQR_Get(HQ_Anims,animNum), HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum));

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
	int room = ListObjets[actorIdx].room;

	for(i=0;i<NUM_MAX_OBJECT;i++)
	{
		tObject* currentTestedActorPtr = &ListObjets[i];

		if(currentTestedActorPtr->indexInWorld != -1 && i != actorIdx)
		{
			ZVStruct* testedZv = &currentTestedActorPtr->zv;

			if(currentTestedActorPtr->room != room)
			{
				ZVStruct localZv;
				CopyZV(zvPtr, &localZv);
				AdjustZV(&localZv, room, currentTestedActorPtr->room);

				if(CubeIntersect(&localZv, testedZv))
				{
					ListObjets[i].COL_BY = actorIdx;
					fallResult++;
				}
			}
			else
			{
				if(CubeIntersect(zvPtr, testedZv))
				{
					ListObjets[i].COL_BY = actorIdx;
					fallResult++;
				}
			}
		}
	}

	return(fallResult);
}

void GereAnim(void)
{
    if (currentProcessedActorPtr->objectType & AF_OBJ_2D) {
        if ((currentProcessedActorPtr->ANIM != -1) && (currentProcessedActorPtr->bodyNum != -1)) {
            sHybrid* pHybrid = HQR_Get(HQ_Hybrides, currentProcessedActorPtr->ANIM);
        }
    }

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
                StockInterAnim(BufferAnim[bufferAnimCounter], HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum));

                bufferAnimCounter++;
                if (bufferAnimCounter == NB_BUFFER_ANIM)
                    bufferAnimCounter = 0;

            }
            else {
                ResetStartAnim(HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum));
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

            currentProcessedActorPtr->numOfFrames = GetNbFramesAnim(HQR_Get(HQ_Anims, newAnim));
        }
    }

	if(currentProcessedActorPtr->ANIM == -1) // no animation
	{
		currentProcessedActorPtr->END_FRAME = 0;
		if(currentProcessedActorPtr->speed == 0)
		{
			int numObjectCollisions = CheckObjectCol(currentProcessedActorIdx, &currentProcessedActorPtr->zv);

			for(int i = 0; i<numObjectCollisions; i++)
			{
				ListObjets[currentProcessedActorPtr->COL[i]].COL_BY = currentProcessedActorIdx; // collision with current actor
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

		currentProcessedActorPtr->END_FRAME = SetInterAnimObjet(currentProcessedActorPtr->frame, HQR_Get(HQ_Anims, currentProcessedActorPtr->ANIM), HQR_Get(HQ_Bodys, currentProcessedActorPtr->bodyNum));

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
		CopyZV(&currentProcessedActorPtr->zv,&zvLocal);

		zvLocal.ZVX1 += stepX;
		zvLocal.ZVX2 += stepX;

		zvLocal.ZVY1 += stepY;
		zvLocal.ZVY2 += stepY;

		zvLocal.ZVZ1 += stepZ;
		zvLocal.ZVZ2 += stepZ;

        // check for wall collisions
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

                // climbable wall
				if(pHardCol->type == 3)
				{
					currentProcessedActorPtr->HARD_COL = 255;
				}

                if (g_gameId >= JACK) {
                    // monster collision for floor changes
                    if ((pHardCol->type == 10) && (currentProcessedActorIdx == currentCameraTargetActor))
                        continue;
                }

				if(stepX || stepZ) // move on the X or Z axis ? update to avoid entering the hard col
				{
					//ZVStruct tempZv;

					hardColStepX = stepX;
					hardColStepZ = stepZ;

					GereCollision(zvPtr, &zvLocal, &pHardCol->zv);

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

        // check for object collisions
        // TODO: AITD2 checks flag collision here
		int numCol = CheckObjectCol(currentProcessedActorIdx,&zvLocal); // get the number of actor/actor collision
		for(int j=0;j<numCol;j++) // process the actor/actor collision
		{
			int collisionIndex = currentProcessedActorPtr->COL[j];

			tObject* actorTouchedPtr = &ListObjets[collisionIndex];

			actorTouchedPtr->COL_BY = currentProcessedActorIdx;

			ZVStruct* touchedZv = &actorTouchedPtr->zv;

			if(actorTouchedPtr->objectType & AF_FOUNDABLE) // takable
			{
				if(currentProcessedActorPtr->trackMode == 1 /*&& ((gameId == AITD1 && defines.field_1E == 0) || (gameId >= JACK && defines.field_6 == 0))*/) // TODO: check if character isn't dead...
				{
					FoundObjet(actorTouchedPtr->indexInWorld, 0);
				}
                continue;
			}
			{
				if(actorTouchedPtr->objectType & AF_MOVABLE) // can be pushed ?
				{
					ZVStruct localZv2;

					bool isPushPossible = true;

					CopyZV(touchedZv, &localZv2);

					localZv2.ZVX1 += stepX;
					localZv2.ZVX2 += stepX;

					localZv2.ZVZ1 += stepZ;
					localZv2.ZVZ2 += stepZ;

                    // check pushed object against walls
					if(AsmCheckListCol(&localZv2, &roomDataTable[currentProcessedActorPtr->room]))
					{
                        isPushPossible = false;
					}
                    else if (CheckObjectCol(collisionIndex, &localZv2))
                    {
                        // if no wall preventing to move object, check is there is a another object in the way
                        isPushPossible = false;
                    }

					if(!isPushPossible)
					{
						if(stepX || stepZ) //if we're trying to move
						{
							if(actorTouchedPtr->room != currentProcessedActorPtr->room)
							{
								ZVStruct localZv3;

								CopyZV(touchedZv, &localZv3);

								AdjustZV(&localZv3, actorTouchedPtr->room, currentProcessedActorPtr->room);

								hardColStepX = stepX;
								hardColStepZ = stepZ;

								GereCollision(zvPtr, &zvLocal, &localZv3);

								stepX = hardColStepX;
								stepZ = hardColStepZ;
							}
							else
							{
								hardColStepX = stepX;
								hardColStepZ = stepZ;

								GereCollision(zvPtr, &zvLocal, touchedZv); // manage as hard collision

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

						CopyZV(&localZv2,touchedZv);
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

								GereCollision(zvPtr, &zvLocal, touchedZv); // manage as hard collision

								stepX = hardColStepX;
								stepZ = hardColStepZ;
							}
							else // different room
							{
								ZVStruct localZv3;

								CopyZV(touchedZv, &localZv3);

								AdjustZV(&localZv3, actorTouchedPtr->room, currentProcessedActorPtr->room);

								hardColStepX = stepX;
								hardColStepZ = stepZ;

								GereCollision(zvPtr, &zvLocal, &localZv3); // manage as hard collision

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

			CopyZV(zvPtr, &zvLocal);

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
			tObject* actorTouchedPtr = &ListObjets[collisionIndex];

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


void StockInterAnim(sFrame& buffer, sBody* bodyPtr)
{
    if(bodyPtr->m_flags & INFO_ANIM)
    {
        *(u16*)(bodyPtr->m_scratchBuffer.data()+4) = (u16)timer;
        bodyPtr->startAnim = &buffer;

        buffer.m_groups.resize(bodyPtr->m_groups.size());
        for (int i = 0; i < bodyPtr->m_groups.size(); i++) {
            buffer.m_groups[i] = bodyPtr->m_groups[i].m_state;
        }
    }
}

void ResetStartAnim(sBody* bodyPtr) {
    *(u16*)(bodyPtr->m_scratchBuffer.data() + 4) = (u16)timer; // reset timer
    bodyPtr->startAnim = nullptr;
}

s16 GetNbFramesAnim(sAnimation* animPtr)
{
    return animPtr->m_numFrames;
}

s16 PatchType(sGroupState* bodyPtr, u16 type) // local
{
    bodyPtr->m_type = type;

    return(bodyPtr->m_type);
}

void PatchInterAngle(s16* value, s16 previousValue, s16 nextValue, int bp, int bx) // local
{
    s16 diff = nextValue - previousValue;

    if(diff == 0)
    {
        *value = nextValue;
    }
    else
    {
        if(diff <= 0x200)
        {
            if(diff >= -0x200)
            {
                *value = ((diff*bp)/bx) + previousValue;
            }
            else
            {
                nextValue += 0x400;
                nextValue -= previousValue;

                *value = ((nextValue *bp)/bx) + previousValue;
            }
        }
        else
        {
            previousValue += 0x400;
            nextValue -= previousValue;

            *value = ((nextValue *bp)/bx) + previousValue;
        }
    }
}

void PatchInterStep(s16* value, s16 previousValue, s16 nextValue, int bp, int bx) // local
{
    s16 cx = previousValue;
    s16 ax = nextValue;

    if(ax == cx)
    {
        *value = ax;
    }
    else
    {
        *value = (((ax - cx)*bp)/bx) + cx;
    }
}

s16 SetInterAnimObjet(int frame, sAnimation* pAnim, sBody* pBody)
{
    int numOfBonesInAnim = pAnim->m_numGroups;
    u16 keyframeLength;
    u16 timeOfKeyframeStart;
    int ax;
    u16 bx;
    u16 time;
    int bp;
    int flag;

    flag = pBody->m_flags;

    sFrame* pKeyframe = &pAnim->m_frames[frame];

    keyframeLength = pKeyframe->m_timestamp; // keyframe length

    if(!(pBody->m_flags & INFO_ANIM)) // do not anim if the model can't be animated
    {
        return(0);
    }

    timeOfKeyframeStart = *(u16*)(pBody->m_scratchBuffer.data() + 4); // time of start of keyframe

	sFrame* pPreviousKeyframe = pBody->startAnim;

    if(!pPreviousKeyframe)
    {
		pPreviousKeyframe = pKeyframe;
    }

    if(numOfBonesInAnim > pBody->m_groupOrder.size())
    {
        numOfBonesInAnim = pBody->m_groupOrder.size();
    }

    time = (u16)timer - timeOfKeyframeStart;

    bx = keyframeLength;
    bp = time;

    if(time<keyframeLength) // interpolate keyframe
    {
        if(!(flag&INFO_OPTIMISE))
        {
            for (int i = 0; i < numOfBonesInAnim; i++)
            {
                point3dStruct& state = pBody->m_groups[i].m_state.m_delta;
                point3dStruct& previousState = pPreviousKeyframe->m_groups[i].m_delta;
                point3dStruct& nextState = pKeyframe->m_groups[i].m_delta;
                switch(PatchType(&pBody->m_groups[i].m_state, pKeyframe->m_groups[i].m_type))
                {
                case 0: // rotate
                    PatchInterAngle(&state.x, previousState.x, nextState.x, bp, bx);
                    PatchInterAngle(&state.y, previousState.y, nextState.y, bp, bx);
                    PatchInterAngle(&state.z, previousState.z, nextState.z, bp, bx);
                    break;
                case 1: // translate
                case 2: // zoom
                    PatchInterStep(&state.x, previousState.x, nextState.x, bp, bx);
                    PatchInterStep(&state.y, previousState.y, nextState.y, bp, bx);
                    PatchInterStep(&state.z, previousState.z, nextState.z, bp, bx);
                    break;
                }
            }
        }
        else
        {
            for (int i = 0; i < numOfBonesInAnim; i++)
            {
                point3dStruct& state = pBody->m_groups[i].m_state.m_delta;
                point3dStruct& previousState = pPreviousKeyframe->m_groups[i].m_delta;
                point3dStruct& nextState = pKeyframe->m_groups[i].m_delta;
                switch (PatchType(&pBody->m_groups[i].m_state, pKeyframe->m_groups[i].m_type))
                {
                case 0:
                        break;
                case 1:
                case 2:
                    PatchInterStep(&state.x, previousState.x, nextState.x, bp, bx);
                    PatchInterStep(&state.y, previousState.y, nextState.y, bp, bx);
                    PatchInterStep(&state.z, previousState.z, nextState.z, bp, bx);
                    break;
                }

                {
                    point3dStruct& state = pBody->m_groups[i].m_state.m_rotateDelta.value();
                    point3dStruct& previousState = pPreviousKeyframe->m_groups[i].m_rotateDelta.value();
                    point3dStruct& nextState = pKeyframe->m_groups[i].m_rotateDelta.value();

                    PatchInterAngle(&state.x, previousState.x, nextState.x, bp, bx);
                    PatchInterAngle(&state.y, previousState.y, nextState.y, bp, bx);
                    PatchInterAngle(&state.z, previousState.z, nextState.z, bp, bx);
                }
            }
        }

        animStepX = (pKeyframe->m_animStep.x * bp) / bx;
        animStepY = (pKeyframe->m_animStep.y * bp) / bx;
        animStepZ = (pKeyframe->m_animStep.z * bp) / bx;

        animCurrentTime = bx;
        animKeyframeLength = bp;
        return(0);
    }
    else // change keyframe
    {
        for (int i = 0; i < numOfBonesInAnim; i++)
        {
            sGroupState& nextState = pKeyframe->m_groups[i];
            pBody->m_groups[i].m_state = nextState;
        };

        pBody->startAnim = pKeyframe;

        *(u16*)(pBody->m_scratchBuffer.data()+4) = (u16)timer;

        animCurrentTime = bx;
        animKeyframeLength = bx;

        animStepX = pKeyframe->m_animStep.x;
        animStepY = pKeyframe->m_animStep.y;
        animStepZ = pKeyframe->m_animStep.z;
        return(1);
    }

}
