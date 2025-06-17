#include "common.h"

#define		NO_FRAPPE			0
#define		WAIT_FRAPPE_ANIM	1
#define		FRAPPE_OK			2
#define		DONE_FRAPPE			3
#define		WAIT_TIR_ANIM		4
#define		DO_TIR				5
#define		WAIT_ANIM_THROW		6
#define		WAIT_FRAME_THROW	7
#define		HIT_OBJECT			8
#define		THROW_OBJECT		9
#define		WAIT_FRAPPE_FRAME	10

void GereFrappe(void)
{
    switch(currentProcessedActorPtr->animActionType)
    {
    case WAIT_FRAPPE_ANIM:
        if(currentProcessedActorPtr->ANIM == currentProcessedActorPtr->animActionANIM)
        {
            currentProcessedActorPtr->animActionType = WAIT_FRAPPE_FRAME;
        }
        [[fallthrough]];
    case WAIT_FRAPPE_FRAME:
        if (currentProcessedActorPtr->ANIM != currentProcessedActorPtr->animActionANIM)
        {
            currentProcessedActorPtr->animActionType = NO_FRAPPE;
            return;
        }

        if (currentProcessedActorPtr->frame == currentProcessedActorPtr->animActionFRAME)
        {
            currentProcessedActorPtr->animActionType = FRAPPE_OK;
        }
        return;

    case FRAPPE_OK:
        {
            int x;
            int y;
            int z;
            int range;
            int collision;
            int i;
            ZVStruct rangeZv;

            if(currentProcessedActorPtr->ANIM != currentProcessedActorPtr->animActionANIM)
            {
                currentProcessedActorPtr->animActionType = 0;
            }

            x = currentProcessedActorPtr->roomX + currentProcessedActorPtr->hotPoint.x + currentProcessedActorPtr->stepX;
            y = currentProcessedActorPtr->roomY + currentProcessedActorPtr->hotPoint.y + currentProcessedActorPtr->stepY;
            z = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->hotPoint.z + currentProcessedActorPtr->stepZ;

            range = currentProcessedActorPtr->animActionParam;

            rangeZv.ZVX1 = x - range;
            rangeZv.ZVX2 = x + range;
            rangeZv.ZVY1 = y - range;
            rangeZv.ZVY2 = y + range;
            rangeZv.ZVZ1 = z - range;
            rangeZv.ZVZ2 = z + range;

            if (backgroundMode == backgroundModeEnum_3D) {
                drawZv(rangeZv);
            }
            
            //drawProjectedBox(rangeZv.ZVX1,rangeZv.ZVX2,rangeZv.ZVY1,rangeZv.ZVY2,rangeZv.ZVZ1,rangeZv.ZVZ2,60,255);

            collision = checkObjectCollisions(currentProcessedActorIdx,&rangeZv);

            for(i=0;i<collision;i++)
            {
                tObject* actorPtr2;

                currentProcessedActorPtr->HIT = currentProcessedActorPtr->COL[i];
                actorPtr2 = &objectTable[currentProcessedActorPtr->COL[i]];

                actorPtr2->HIT_BY = currentProcessedActorIdx;
                actorPtr2->hitForce = currentProcessedActorPtr->hitForce;

                if(actorPtr2->objectType & AF_ANIMATED)
                {
                    currentProcessedActorPtr->animActionType = 0;
                    return;
                }
            }
            break;
        }
    case 4: // WAIT_TIR_ANIM
        {
            if(currentProcessedActorPtr->ANIM != currentProcessedActorPtr->animActionANIM)
                return;

            if(currentProcessedActorPtr->frame != currentProcessedActorPtr->animActionFRAME)
                return;

            currentProcessedActorPtr->animActionType = 5;

            break;
        }
    case 5: // DO_TIR
        {
            int touchedActor;

            InitSpecialObjet( 3,
                currentProcessedActorPtr->roomX + currentProcessedActorPtr->hotPoint.x,
                currentProcessedActorPtr->roomY + currentProcessedActorPtr->hotPoint.y,
                currentProcessedActorPtr->roomZ + currentProcessedActorPtr->hotPoint.z,
                currentProcessedActorPtr->stage,
                currentProcessedActorPtr->room,
                0,
                currentProcessedActorPtr->beta,
                0,
                NULL);

            touchedActor = checkLineProjectionWithActors(
                currentProcessedActorIdx,
                currentProcessedActorPtr->roomX + currentProcessedActorPtr->hotPoint.x,
                currentProcessedActorPtr->roomY + currentProcessedActorPtr->hotPoint.y,
                currentProcessedActorPtr->roomZ + currentProcessedActorPtr->hotPoint.z,
                currentProcessedActorPtr->beta - 0x100,
                currentProcessedActorPtr->room,
                currentProcessedActorPtr->animActionParam );

            if(touchedActor == -1) // no one has been touched
            {
                InitSpecialObjet( 2, animMoveX, animMoveY, animMoveZ, currentProcessedActorPtr->stage, currentProcessedActorPtr->room, 0, -currentProcessedActorPtr->beta, 0, NULL );

                currentProcessedActorPtr->animActionType = 0;
            }
            else
            {
                InitSpecialObjet( 2, animMoveX, animMoveY, animMoveZ, currentProcessedActorPtr->stage, currentProcessedActorPtr->room, 0, -currentProcessedActorPtr->beta, 0, NULL );

                currentProcessedActorPtr->hotPoint.x = animMoveX - currentProcessedActorPtr->roomX;
                currentProcessedActorPtr->hotPoint.y = animMoveY - currentProcessedActorPtr->roomY;
                currentProcessedActorPtr->hotPoint.z = animMoveZ - currentProcessedActorPtr->roomZ;

                currentProcessedActorPtr->HIT = touchedActor;

                objectTable[touchedActor].HIT_BY = currentProcessedActorIdx;
                objectTable[touchedActor].hitForce = currentProcessedActorPtr->hitForce;

                currentProcessedActorPtr->animActionType = 0;
            }
            break;
        }
    case 6: // WAIT_ANIM_THROW
        {
            if(currentProcessedActorPtr->ANIM == currentProcessedActorPtr->animActionANIM)
            {
                int objIdx = currentProcessedActorPtr->animActionParam;

                tWorldObject* objPtr = &ListWorldObjets[objIdx];

                int x = currentProcessedActorPtr->roomX + currentProcessedActorPtr->hotPoint.x + currentProcessedActorPtr->stepX;
                int y = currentProcessedActorPtr->roomY + currentProcessedActorPtr->hotPoint.y + currentProcessedActorPtr->stepY;
                int z = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->hotPoint.z + currentProcessedActorPtr->stepZ;

                ZVStruct rangeZv;

                GiveZVObjet(getBodyFromPtr(HQR_Get(HQ_Bodys, objPtr->body)),&rangeZv);

                rangeZv.ZVX1 += x;
                rangeZv.ZVX2 += x;
                rangeZv.ZVY1 += y;
                rangeZv.ZVY2 += y;
                rangeZv.ZVZ1 += z;
                rangeZv.ZVZ2 += z;

                if(AsmCheckListCol(&rangeZv, &roomDataTable[currentProcessedActorPtr->room]))
                {
                    currentProcessedActorPtr->animActionType = 0;
                    PutAtObjet(objIdx, currentProcessedActorPtr->indexInWorld);
                }
                else
                {
                    if(currentProcessedActorPtr->frame == currentProcessedActorPtr->animActionFRAME)
                    {
                        currentProcessedActorPtr->animActionType = 7;

                        int x = currentProcessedActorPtr->roomX + currentProcessedActorPtr->hotPoint.x + currentProcessedActorPtr->stepX;
                        int y = currentProcessedActorPtr->roomY + currentProcessedActorPtr->hotPoint.y + currentProcessedActorPtr->stepY;
                        int z = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->hotPoint.z + currentProcessedActorPtr->stepZ;

                        DeleteInventoryObjet(objIdx);

                        objPtr->x = x;
                        objPtr->y = y;
                        objPtr->z = z;

                        objPtr->room = currentProcessedActorPtr->room;
                        objPtr->stage = currentProcessedActorPtr->stage;
                        objPtr->alpha = currentProcessedActorPtr->alpha;
                        objPtr->beta = currentProcessedActorPtr->beta+0x200; 

                        objPtr->foundFlag &= 0xBFFF;
                        objPtr->flags |= 0x85;
                        objPtr->flags &= 0xFFDF;

                        //FlagGenereActiveList = 1;
                    }
                }
            }
            break;
        }
    case 7: // THROW
        {
            int x;
            int y;
            int z;
            int objIdx;
            int actorIdx;
            tObject* actorPtr;

            currentProcessedActorPtr->animActionType = 0;

            x = currentProcessedActorPtr->roomX + currentProcessedActorPtr->hotPoint.x + currentProcessedActorPtr->stepX;
            y = currentProcessedActorPtr->roomY + currentProcessedActorPtr->hotPoint.y + currentProcessedActorPtr->stepY;
            z = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->hotPoint.z + currentProcessedActorPtr->stepZ;

            objIdx = currentProcessedActorPtr->animActionParam;

            actorIdx = ListWorldObjets[objIdx].objIndex;

            if(actorIdx == -1)
                return;

            actorPtr = &objectTable[actorIdx];

            actorPtr->roomX = x;
            actorPtr->roomY = y;
            actorPtr->roomZ = z;

            GiveZVObjet(getBodyFromPtr(HQR_Get(HQ_Bodys,actorPtr->bodyNum)),&actorPtr->zv);

            actorPtr->zv.ZVX1 += x;
            actorPtr->zv.ZVX2 += x;
            actorPtr->zv.ZVY1 += y;
            actorPtr->zv.ZVY2 += y;
            actorPtr->zv.ZVZ1 += z;
            actorPtr->zv.ZVZ2 += z;

            actorPtr->objectType |= AF_ANIMATED;
            actorPtr->objectType &= ~AF_BOXIFY;

            ListWorldObjets[objIdx].x = x;
            ListWorldObjets[objIdx].y = y;
            ListWorldObjets[objIdx].z = z;

            ListWorldObjets[objIdx].alpha = currentProcessedActorPtr->indexInWorld; // original thrower

            actorPtr->dynFlags = 0;
            actorPtr->animActionType = 9;
            actorPtr->animActionParam = 100;
            actorPtr->hitForce = currentProcessedActorPtr->hitForce;
            actorPtr->hotPointID = -1;
            actorPtr->speed = 3000;

            InitRealValue(0, actorPtr->speed, 60, &actorPtr->speedChange);

            break;
        }
    case 8: // HIT_OBJ
        {
            break;
        }
    case 9: // during throw
        {
            tWorldObject* objPtr = &ListWorldObjets[currentProcessedActorPtr->indexInWorld];

            ZVStruct rangeZv;
            ZVStruct rangeZv2;
            int xtemp;
            int ytemp;
            int ztemp;
            int x1;
            int x2;
            int x3;
            int y1;
            int y2;
            int z1;
            int z2;
            int z3;
            int step;

            copyZv(&currentProcessedActorPtr->zv, &rangeZv);
            copyZv(&currentProcessedActorPtr->zv, &rangeZv2);

            xtemp = currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
            ytemp = currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
            ztemp = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

            rangeZv2.ZVX1 -= xtemp;
            rangeZv2.ZVX2 -= xtemp;
            rangeZv2.ZVY1 -= ytemp;
            rangeZv2.ZVY2 -= ytemp;
            rangeZv2.ZVZ1 -= ztemp;
            rangeZv2.ZVZ2 -= ztemp;

            x1 = objPtr->x;
            x2 = objPtr->x;
            x3 = objPtr->x;

            y1 = objPtr->y;
            y2 = objPtr->y;

            z1 = objPtr->z;
            z2 = objPtr->z;
            z3 = objPtr->z;

            step = 0;

            animMoveZ = 0;
            animMoveX = 0;

            do
            {
                int collision;
                sceZoneStruct* ptr;

                walkStep(0,-step,currentProcessedActorPtr->beta);
                step += 100;
                x2 = x1 + animMoveX;
                y2 = y1;
                z2 = z1 + animMoveZ;

                copyZv(&rangeZv2,&rangeZv);

                rangeZv.ZVX1 = x2 - 200;
                rangeZv.ZVX2 = x2 + 200;
                rangeZv.ZVY1 = y2 - 200;
                rangeZv.ZVY2 = y2 + 200;
                rangeZv.ZVZ1 = z2 - 200;
                rangeZv.ZVZ2 = z2 + 200;

                collision =  checkObjectCollisions(currentProcessedActorIdx,&rangeZv);

                if(collision)
                {
                    int collision2 = collision;
                    int i;

                    currentProcessedActorPtr->hotPoint.x = 0;
                    currentProcessedActorPtr->hotPoint.y = 0;
                    currentProcessedActorPtr->hotPoint.z = 0;

                    for(i=0;i<collision;i++)
                    {
                        int currentActorCol = currentProcessedActorPtr->COL[i];

                        if(objectTable[currentActorCol].indexInWorld == objPtr->alpha)
                        {
                            collision2--;
                            objPtr->x = xtemp;
                            objPtr->y = ytemp;
                            objPtr->z = ztemp;

                            return;
                        }

                        if(objectTable[currentActorCol].indexInWorld == CVars[getCVarsIdx((enumCVars)REVERSE_OBJECT)])
                        {
                            objPtr->alpha = CVars[getCVarsIdx((enumCVars)REVERSE_OBJECT)];
                            currentProcessedActorPtr->beta += 0x200;
                            xtemp = x3;
                            ztemp = z3;

                            currentProcessedActorPtr->worldX = currentProcessedActorPtr->roomX = x3;
                            currentProcessedActorPtr->worldY = currentProcessedActorPtr->roomY = y1;
                            currentProcessedActorPtr->worldZ = currentProcessedActorPtr->roomZ = z3;

                            currentProcessedActorPtr->stepX = 0;
                            currentProcessedActorPtr->stepZ = 0;

                            copyZv(&rangeZv2, &rangeZv);

                            rangeZv.ZVX1 += x3;
                            rangeZv.ZVX2 += x3;
                            rangeZv.ZVY1 += y1;
                            rangeZv.ZVY2 += y1;
                            rangeZv.ZVZ1 += z3;
                            rangeZv.ZVZ2 += z3;

                            copyZv(&rangeZv, &currentProcessedActorPtr->zv);

                            objPtr->x = xtemp;
                            objPtr->y = ytemp;
                            objPtr->z = ztemp;

                            return;

                        }
                        else
                        {
                            tObject* actorPtr;

                            currentProcessedActorPtr->HIT = currentActorCol;
                            actorPtr = &objectTable[currentActorCol];
                            actorPtr->HIT_BY = currentProcessedActorIdx;
                            actorPtr->hitForce = currentProcessedActorPtr->hitForce;
                        }
                    }

                    if(collision2)
                    {
                        playSound(CVars[getCVarsIdx((enumCVars)SAMPLE_CHOC)]);
                        throwStoppedAt(x3,z3);
                        return;
                    }
                }

                ptr = processActor2Sub(x2,y2,z2, &roomDataTable[currentProcessedActorPtr->room]);

                if(ptr)
                {
                    if(ptr->type == 0 || ptr->type == 10)
                    {
                        playSound(CVars[getCVarsIdx((enumCVars)SAMPLE_CHOC)]);
                        throwStoppedAt(x3,z3);
                        return;
                    }
                }

                if(AsmCheckListCol(&rangeZv, &roomDataTable[currentProcessedActorPtr->room]))
                {
                    currentProcessedActorPtr->hotPoint.x = 0;
                    currentProcessedActorPtr->hotPoint.y = 0;
                    currentProcessedActorPtr->hotPoint.z = 0;

                    playSound(CVars[getCVarsIdx((enumCVars)SAMPLE_CHOC)]);
                    throwStoppedAt(x3,z3);
                    return;
                }
            }while(   currentProcessedActorPtr->zv.ZVX1 - 100 > x2 ||
                currentProcessedActorPtr->zv.ZVX2 + 100 < x2 ||
                currentProcessedActorPtr->zv.ZVZ1 - 100 > z2 ||
                currentProcessedActorPtr->zv.ZVZ2 + 100 < z2 );

            objPtr->x = xtemp;
            objPtr->y = ytemp;
            objPtr->z = ztemp;

            break;
        }
#ifdef FITD_DEBUGGER
    default:
        {
            printf("Unsupported processAnimAction type %d\n",currentProcessedActorPtr->animActionType);
            break;
        }
#endif
    }
}
