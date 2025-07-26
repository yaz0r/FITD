#include "common.h"
#include "anim2d.h"

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

void PlayWorld(int allowSystemMenu, int deltaTime)
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

        if(FlagChangeEtage == 0)
        {
            if(g_gameId == AITD1)
            {
                if(CVars[getCVarsIdx(LIGHT_OBJECT)] == -1)
                {
                    //        mainVar2 = 2000;
                    //        mainVar3 = 2000;
                }
            }

            for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++)
            {
                currentProcessedActorPtr = &ListObjets[currentProcessedActorIdx];
                if(currentProcessedActorPtr->indexInWorld >= 0)
                {
                    currentProcessedActorPtr->COL_BY = -1;
                    currentProcessedActorPtr->HIT_BY = -1;
                    currentProcessedActorPtr->HIT = -1;
                    currentProcessedActorPtr->HARD_DEC = -1;
                    currentProcessedActorPtr->HARD_COL = -1;
                }
            }

            for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++)
            {
                currentProcessedActorPtr = &ListObjets[currentProcessedActorIdx];
                if(currentProcessedActorPtr->indexInWorld >= 0)
                {
                    int flag = currentProcessedActorPtr->objectType;

                    if((flag & AF_ANIMATED) || (g_gameId >= AITD2 && flag & 0x200))
                    {
                        GereAnim();
                    }
                    if(flag & AF_TRIGGER)
                    {
                        GereDec();
                    }

                    if(currentProcessedActorPtr->animActionType)
                    {
                        GereFrappe();
                    }
                }
            }

            
            for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++)
            {
                currentProcessedActorPtr = &ListObjets[currentProcessedActorIdx];
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

                if(FlagChangeEtage)
                    break;
            }

            if(FlagGameOver)
                break;
        }

        if(FlagChangeEtage)
        {
            LoadEtage(NewNumEtage);
        }

        if(FlagChangeSalle)
        {
			ChangeSalle(NewNumSalle);
            InitView();
            continue;
        }

        GereSwitchCamera();

        // Handle 2d objects
        //if (g_gameId >= AITD2) // no need for this test since it would do anything if there are 2d objects
        {
            int memocam = NumCamera;

            NumCamera = NewNumCamera;

            for (currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++)
            {
                currentProcessedActorPtr = &ListObjets[currentProcessedActorIdx];
                if (currentProcessedActorPtr->indexInWorld >= 0)
                {
                    if (currentProcessedActorPtr->life != -1)
                    {
                        if (currentProcessedActorPtr->objectType & AF_OBJ_2D)
                        {
                            if (currentProcessedActorPtr->lifeMode & 3)
                                if (!(currentProcessedActorPtr->lifeMode & 4))
                                {
                                    processLife(currentProcessedActorPtr->life, false);
                                    FlagGenereAffList = 1;
                                }
                        }
                    }
                }

                if (FlagChangeEtage)
                    break;
            }

            if (FlagGameOver)
                break;

            NumCamera = memocam;
        }
        if (FlagInitView
#ifdef FITD_DEBUGGER
            || debuggerVar_topCamera
#endif
            )
        {
            InitView();
        }

        //    if(FlagGenereActiveList)
        {
            GenereActiveList();
        } 

        //    if(actorTurnedToObj)
        {
            GenereAffList();
        }

        sortActorList();

        handleAnim2d();

        //    if(FlagRefreshAux2)
        {
            //      setupCameraSub4();
        }

        //    mainLoopSub1();

        //osystem_delay(100);

        AllRedraw(flagRedraw);

        updatePendingEvents();
    }

    //  mainLoopVar1 = 0;
    //  shakingState = 0;

    //  stopShaking();
    //  stopSounds();
}

