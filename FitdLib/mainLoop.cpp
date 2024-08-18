#include "common.h"

#ifndef WIN32
#include <sys/time.h>
#endif

int mainLoopSwitch = 0;

void updatePendingEvents(void)
{
    // TODO: miss pending events here

    if(currentMusic!=-1)
    {
        if(currentMusic==-2)
        {
            if(evalChrono(&musicChrono)>180)
            {
                playMusic(nextMusic);
            }
        }
        else
        {
			/*
            if(fadeMusic(0,0,0x10)==-1)
            {
                currentMusic = -1;

                if(nextMusic != -1)
                {
                    playMusic(nextMusic);
                    nextMusic = -1;
                }
            }
			*/
        }
    }
}

extern "C" {
    void mainLoop(int allowSystemMenu, int deltaTime);
}

void mainLoop(int allowSystemMenu, int deltaTime)
{
    bool bLoop = true;

    while(bLoop)
    {
		process_events();
        
        localKey = key;
        localJoyD = JoyD;
        localClick = Click;

        if(localKey)
        {
            if(localKey == 0x1B)
            {
                while(key==0x1B)
                {
                    process_events();
                }
                processSystemMenu();
                while(key==0x1B || key == 0x1C)
                {
                    process_events();
                    localKey = key;
                }
            }

            if(localKey == 0x1C || localKey == 0x17)
            {
                if(allowSystemMenu == 0)
                {
                    break;
                }

                if(statusScreenAllowed)
                {
                    processInventory();
                }
            }
        }
        else
        {
            //      input5 = 0;
        }

        if(localClick)
        {
            if(!allowSystemMenu)
            {
                break;
            }

            action = 0x2000;
        }
        else
        {
            action = 0;
        }

        executeFoundLife(inHandTable[currentInventory]);

        if(changeFloor == 0)
        {
            if(g_gameId == AITD1)
            {
                if(CVars[getCVarsIdx(LIGHT_OBJECT)] == -1)
                {
                    //        mainVar2 = 2000;
                    //        mainVar3 = 2000;
                }
            }

            currentProcessedActorPtr = objectTable;

            for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++)
            {
                if(currentProcessedActorPtr->indexInWorld >= 0)
                {
                    currentProcessedActorPtr->COL_BY = -1;
                    currentProcessedActorPtr->HIT_BY = -1;
                    currentProcessedActorPtr->HIT = -1;
                    currentProcessedActorPtr->HARD_DEC = -1;
                    currentProcessedActorPtr->HARD_COL = -1;
                }

                currentProcessedActorPtr++;
            }

            currentProcessedActorPtr = objectTable;
            for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++)
            {
                if(currentProcessedActorPtr->indexInWorld >= 0)
                {
                    int flag = currentProcessedActorPtr->_flags;

                    if((flag & AF_ANIMATED) || (g_gameId >= AITD2 && flag & 0x200))
                    {
                        updateAnimation();
                    }
                    if(flag & AF_TRIGGER)
                    {
                        processActor2();
                    }

                    if(currentProcessedActorPtr->animActionType)
                    {
                        GereFrappe();
                    }
                }

                currentProcessedActorPtr++;
            }

            currentProcessedActorPtr = objectTable;
            for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++)
            {
                if(currentProcessedActorPtr->indexInWorld >= 0)
                {
                    if(currentProcessedActorPtr->life != -1)
                    {
                        switch(g_gameId)
                        {
                        case AITD2:
                        case AITD3:
                        case TIMEGATE:
                            {
                                if(currentProcessedActorPtr->lifeMode&3)
                                    if(!(currentProcessedActorPtr->lifeMode&4))
                                        processLife(currentProcessedActorPtr->life, false);
                                break;
                            }
						case JACK:
                        case AITD1:
                            {
                                if(currentProcessedActorPtr->life != -1)
                                    if(currentProcessedActorPtr->lifeMode != -1)
                                        processLife(currentProcessedActorPtr->life, false);
                                break;
                            }
                        }
                    }
                }

                if(changeFloor)
                    break;

                currentProcessedActorPtr++;
            }

            if(giveUp)
                break;
        }

        if(changeFloor)
        {
            loadFloor(newFloor);
        }

        if(needChangeRoom)
        {
			loadRoom(newRoom);
            setupCamera();
        }
        else
        {
            checkIfCameraChangeIsRequired();
            if(g_gameId >= AITD2)
            {
                int tempCurrentCamera;

                tempCurrentCamera = currentCamera;

                currentCamera = startGameVar1;

				currentProcessedActorPtr = objectTable;
                for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++)
                {
                    if(currentProcessedActorPtr->indexInWorld >= 0)
                    {
                        if(currentProcessedActorPtr->life != -1)
                        {
                            if(currentProcessedActorPtr->_flags & 0x200)
                            {
                                if(currentProcessedActorPtr->lifeMode&3)
                                    if(!(currentProcessedActorPtr->lifeMode&4))
                                    {
                                        processLife(currentProcessedActorPtr->life, false);
                                        actorTurnedToObj = 1;
                                    }
                            }
                        }
                    }

                    if(changeFloor)
                        break;

					currentProcessedActorPtr++;
                }

                if(giveUp)
                    break;

                currentCamera = tempCurrentCamera;
            }
            if(flagInitView
#ifdef FITD_DEBUGGER
               || debuggerVar_topCamera
#endif
               )
            {
                setupCamera();
            }
        }

        //    if(FlagGenereActiveList)
        {
            updateAllActorAndObjects();
        } 

        //    if(actorTurnedToObj)
        {
            createActorList();
        }

        sortActorList();

        //    if(FlagRefreshAux2)
        {
            //      setupCameraSub4();
        }

        //    mainLoopSub1();

        //osystem_delay(100);

        mainDraw(flagRedraw);

        updatePendingEvents();
    }

    //  mainLoopVar1 = 0;
    //  shakingState = 0;

    //  stopShaking();
    //  stopSounds();
}

