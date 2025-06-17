#include "common.h"

u32 g_currentFloorRoomRawDataSize = 0;
u32 g_currentFloorCameraRawDataSize;
std::vector<cameraDataStruct> g_currentFloorCameraData;

void LoadEtage(int floorNumber)
{
    int i;
    int expectedNumberOfRoom;
    int expectedNumberOfCamera;

    if(g_currentFloorCameraRawData)
    {
        free(g_currentFloorCameraRawData);
        free(g_currentFloorRoomRawData);
    }

    //stopSounds();

    HQR_Reset(HQ_Bodys);
    HQR_Reset(HQ_Anims);

    g_currentFloor = floorNumber;

    if(g_gameId <= AITD3)
    {
        std::string floorFileName = std::format("ETAGE{:02d}", floorNumber);
        if (fileExists((floorFileName+".PAK").c_str()))
        {
            g_currentFloorRoomRawDataSize = getPakSize(floorFileName.c_str(), 0);
            g_currentFloorCameraRawDataSize = getPakSize(floorFileName.c_str(), 1);

            g_currentFloorRoomRawData = CheckLoadMallocPak(floorFileName.c_str(), 0);
            g_currentFloorCameraRawData = CheckLoadMallocPak(floorFileName.c_str(), 1);
        }
    }

    NumCamera = -1;
    FlagChangeSalle = 1;
    FlagChangeEtage = 0;

    //////////////////////////////////

    roomDataTable.clear();

    expectedNumberOfRoom = getNumberOfRoom();
    assert(expectedNumberOfRoom);

    roomDataTable.reserve(expectedNumberOfRoom);

    for(i=0;i<expectedNumberOfRoom;i++)
    {
        u32 j;
        u8* roomData = nullptr;
        u8* hardColData;
        u8* sceZoneData;

        if (g_currentFloorRoomRawDataSize == 0) {
            if (fileExists(std::format("ETAGE{:02d}.PAK", floorNumber).c_str()))
            {
                roomData = (u8*)CheckLoadMallocPak(std::format("ETAGE{:02d}", floorNumber).c_str(), i);
            }
            else if (fileExists(std::format("SAL{:02d}.PAK", floorNumber).c_str()))
            {
                roomData = (u8*)CheckLoadMallocPak(std::format("SAL{:02d}", floorNumber).c_str(), i);
            }
            else {
                assert(0);
            }
        }
        else
        {
            roomData = (u8*)(g_currentFloorRoomRawData + READ_LE_U32(g_currentFloorRoomRawData + i * 4));
        }
        assert(roomData);

        roomDataStruct* currentRoomDataPtr = &roomDataTable.emplace_back();

        currentRoomDataPtr->worldX = READ_LE_S16(roomData+4);
        currentRoomDataPtr->worldY = READ_LE_S16(roomData+6);
        currentRoomDataPtr->worldZ = READ_LE_S16(roomData+8);

        currentRoomDataPtr->numCameraInRoom = READ_LE_U16(roomData+0xA);

        currentRoomDataPtr->cameraIdxTable.reserve(currentRoomDataPtr->numCameraInRoom);

        for(j=0;j<currentRoomDataPtr->numCameraInRoom;j++)
        {
            currentRoomDataPtr->cameraIdxTable.push_back(READ_LE_U16(roomData+0xC+2*j));
        }

        // hard col read

        hardColData = roomData + READ_LE_U16(roomData);
        currentRoomDataPtr->numHardCol = READ_LE_U16(hardColData);
        hardColData+=2;

        {
            currentRoomDataPtr->hardColTable.resize(currentRoomDataPtr->numHardCol);

            for(j=0;j<currentRoomDataPtr->numHardCol;j++)
            {
                ZVStruct* zvData = &currentRoomDataPtr->hardColTable[j].zv;

                zvData->ZVX1 = READ_LE_S16(hardColData+0x00);
                zvData->ZVX2 = READ_LE_S16(hardColData+0x02);
                zvData->ZVY1 = READ_LE_S16(hardColData+0x04);
                zvData->ZVY2 = READ_LE_S16(hardColData+0x06);
                zvData->ZVZ1 = READ_LE_S16(hardColData+0x08);
                zvData->ZVZ2 = READ_LE_S16(hardColData+0x0A);

                currentRoomDataPtr->hardColTable[j].parameter = READ_LE_U16(hardColData+0x0C);
                currentRoomDataPtr->hardColTable[j].type = READ_LE_U16(hardColData+0x0E);

                hardColData+=0x10;
            }
        }

        // sce zone read

        sceZoneData = roomData + READ_LE_U16(roomData+2);
        currentRoomDataPtr->numSceZone = READ_LE_U16(sceZoneData);
        sceZoneData+=2;

        {
            currentRoomDataPtr->sceZoneTable.resize(currentRoomDataPtr->numSceZone);

            for(j=0;j<currentRoomDataPtr->numSceZone;j++)
            {
                ZVStruct* zvData;

                zvData = &currentRoomDataPtr->sceZoneTable[j].zv;

                zvData->ZVX1 = READ_LE_S16(sceZoneData+0x00);
                zvData->ZVX2 = READ_LE_S16(sceZoneData+0x02);
                zvData->ZVY1 = READ_LE_S16(sceZoneData+0x04);
                zvData->ZVY2 = READ_LE_S16(sceZoneData+0x06);
                zvData->ZVZ1 = READ_LE_S16(sceZoneData+0x08);
                zvData->ZVZ2 = READ_LE_S16(sceZoneData+0x0A);

                currentRoomDataPtr->sceZoneTable[j].parameter = READ_LE_U16(sceZoneData+0x0C);
                currentRoomDataPtr->sceZoneTable[j].type = READ_LE_U16(sceZoneData+0x0E);

                sceZoneData+=0x10;
            }
        }
    }
    ///////////////////////////////////

    /////////////////////////////////////////////////
    // camera stuff

    if(g_currentFloorCameraRawData == nullptr)
    {
        char buffer[256];

        if(g_gameId == AITD3)
        {
            sprintf(buffer,"CAMERA%02d",floorNumber);
        }
        else
        {
            sprintf(buffer,"CAMSAL%02d",floorNumber);
        }

        expectedNumberOfCamera = PAK_getNumFiles(buffer);
    }
    else
    {
        int maxExpectedNumberOfCamera = ((READ_LE_U32(g_currentFloorCameraRawData))/4);

		expectedNumberOfCamera = 0;

		int minOffset = 0;

		for(int i=0; i<maxExpectedNumberOfCamera; i++)
		{
			int offset = READ_LE_U32(g_currentFloorCameraRawData + i * 4);
			if(offset > minOffset)
			{
				minOffset = offset;
				expectedNumberOfCamera++;
			}
			else
			{
				break;
			}
		}
    }

    g_currentFloorCameraData.clear();
    g_currentFloorCameraData.resize(expectedNumberOfCamera);

    for(i=0;i<expectedNumberOfCamera;i++)
    {
        int k;
        unsigned int offset;
        unsigned char* currentCameraData;

        if (g_currentFloorCameraRawData == nullptr)
        {
            if (fileExists(std::format("CAM{:02d}.PAK", g_currentFloor).c_str())) {
                currentCameraData = (unsigned char*)CheckLoadMallocPak(std::format("CAM{:02d}", g_currentFloor).c_str(), i);
            }
            else if (fileExists(std::format("CAMSAL{:02d}.PAK", g_currentFloor).c_str())) {
                currentCameraData = (unsigned char*)CheckLoadMallocPak(std::format("CAMSAL{:02d}", g_currentFloor).c_str(), i);
            }
            else {
                assert(0);
            }

            offset = 0;
            g_currentFloorCameraRawDataSize = 1;
        }
        else
        {
            offset = READ_LE_U32(g_currentFloorCameraRawData + i * 4);
        }

        // load cameras
        if(offset<g_currentFloorCameraRawDataSize)
        {
            unsigned char* backupDataPtr;

            if(g_currentFloorCameraRawData)
            {
                currentCameraData = (unsigned char*)(g_currentFloorCameraRawData + READ_LE_U32(g_currentFloorCameraRawData + i * 4));
            }

            backupDataPtr = currentCameraData;

            g_currentFloorCameraData[i].alpha = READ_LE_U16(currentCameraData + 0x00);
            g_currentFloorCameraData[i].beta = READ_LE_U16(currentCameraData + 0x02);
            g_currentFloorCameraData[i].gamma = READ_LE_U16(currentCameraData + 0x04);

            g_currentFloorCameraData[i].x = READ_LE_U16(currentCameraData+0x06);
            g_currentFloorCameraData[i].y = READ_LE_U16(currentCameraData+0x08);
            g_currentFloorCameraData[i].z = READ_LE_U16(currentCameraData+0x0A);

            g_currentFloorCameraData[i].focal1 = READ_LE_U16(currentCameraData+0x0C);
            g_currentFloorCameraData[i].focal2 = READ_LE_U16(currentCameraData+0x0E);
            g_currentFloorCameraData[i].focal3 = READ_LE_U16(currentCameraData+0x10);

            g_currentFloorCameraData[i].numViewedRooms = READ_LE_U16(currentCameraData+0x12);

            currentCameraData+=0x14;

            g_currentFloorCameraData[i].viewedRoomTable.reserve(g_currentFloorCameraData[i].numViewedRooms);

            for(k=0;k<g_currentFloorCameraData[i].numViewedRooms;k++)
            {
                cameraViewedRoomStruct* pCurrentCameraViewedRoom = &g_currentFloorCameraData[i].viewedRoomTable.emplace_back();

                pCurrentCameraViewedRoom->viewedRoomIdx = READ_LE_U16(currentCameraData+0x00);
                pCurrentCameraViewedRoom->offsetToMask = READ_LE_U16(currentCameraData+0x02);
                pCurrentCameraViewedRoom->offsetToCover = READ_LE_U16(currentCameraData+0x04);

				if(g_gameId == AITD1)
				{
					pCurrentCameraViewedRoom->offsetToHybrids = 0;
					pCurrentCameraViewedRoom->offsetCamOptims = 0;
					pCurrentCameraViewedRoom->lightX = READ_LE_U16(currentCameraData+0x06);
					pCurrentCameraViewedRoom->lightY = READ_LE_U16(currentCameraData+0x08);
					pCurrentCameraViewedRoom->lightZ = READ_LE_U16(currentCameraData+0x0A);
				}
				else
                {
                    pCurrentCameraViewedRoom->offsetToHybrids = READ_LE_U16(currentCameraData+0x06);
                    pCurrentCameraViewedRoom->offsetCamOptims = READ_LE_U16(currentCameraData+0x08);
					pCurrentCameraViewedRoom->lightX = READ_LE_U16(currentCameraData+0x0A);
					pCurrentCameraViewedRoom->lightY = READ_LE_U16(currentCameraData+0x0C);
					pCurrentCameraViewedRoom->lightZ = READ_LE_U16(currentCameraData+0x0E);
                }

				// load camera mask
				unsigned char* pMaskData = NULL;
				if(g_gameId >= JACK)
				{
					pMaskData = backupDataPtr + g_currentFloorCameraData[i].viewedRoomTable[k].offsetToMask;

					// for this camera, how many masks zone
					pCurrentCameraViewedRoom->numMask = READ_LE_U16(pMaskData);
					pMaskData+=2;

                    pCurrentCameraViewedRoom->masks.reserve(pCurrentCameraViewedRoom->numMask);

					for(int k=0; k<pCurrentCameraViewedRoom->numMask; k++)
					{
						cameraMaskStruct* pCurrentCameraMask = &pCurrentCameraViewedRoom->masks.emplace_back();

						// for this overlay zone, how many 
						pCurrentCameraMask->numTestRect = READ_LE_U16(pMaskData);
						pMaskData+=2;

						pCurrentCameraMask->rectTests.reserve(pCurrentCameraMask->numTestRect);

						for(int j=0; j<pCurrentCameraMask->numTestRect; j++)
						{
                            rectTestStruct* pCurrentRectTest = &pCurrentCameraMask->rectTests.emplace_back();

							pCurrentRectTest->zoneX1 = READ_LE_S16(pMaskData);
							pCurrentRectTest->zoneZ1 = READ_LE_S16(pMaskData+2);
							pCurrentRectTest->zoneX2 = READ_LE_S16(pMaskData+4);
							pCurrentRectTest->zoneZ2 = READ_LE_S16(pMaskData+6);
							pMaskData+=8;
						}
					}
				}

                // load camera cover
                {
                    unsigned char* pZoneData;
                    int numZones;
                    int j;

                    pZoneData = backupDataPtr + g_currentFloorCameraData[i].viewedRoomTable[k].offsetToCover;
					if(pMaskData)
					{
						assert(pZoneData == pMaskData);
					}
                    //pZoneData = currentCameraData;

                    pCurrentCameraViewedRoom->numCoverZones = numZones =READ_LE_U16(pZoneData);
                    pZoneData+=2;

                    pCurrentCameraViewedRoom->coverZones.resize(numZones);

                    for(j=0;j<pCurrentCameraViewedRoom->numCoverZones;j++)
                    {
                        int pointIdx;
                        int numPoints;

                        pCurrentCameraViewedRoom->coverZones[j].numPoints = numPoints = READ_LE_U16(pZoneData);
                        pZoneData+=2;

                        pCurrentCameraViewedRoom->coverZones[j].pointTable = (cameraZonePointStruct*)malloc(sizeof(cameraZonePointStruct)*(numPoints+1));

                        for(pointIdx = 0; pointIdx < pCurrentCameraViewedRoom->coverZones[j].numPoints; pointIdx++)
                        {
                            pCurrentCameraViewedRoom->coverZones[j].pointTable[pointIdx].x = READ_LE_U16(pZoneData);
                            pZoneData+=2;
                            pCurrentCameraViewedRoom->coverZones[j].pointTable[pointIdx].y = READ_LE_U16(pZoneData);
                            pZoneData+=2;
                        }

                        pCurrentCameraViewedRoom->coverZones[j].pointTable[numPoints].x = pCurrentCameraViewedRoom->coverZones[j].pointTable[0].x; // copy first point to last position
                        pCurrentCameraViewedRoom->coverZones[j].pointTable[numPoints].y = pCurrentCameraViewedRoom->coverZones[j].pointTable[0].y;
                    }
                }

                if(g_gameId == AITD1)
                    currentCameraData+=0x0C;
                else
                    currentCameraData+=0x10;

                if(g_gameId == TIMEGATE)
                {
                    currentCameraData+=6;
                }
            }
        }
        else
        {
            break;
        }
    }

    // globalCameraDataTable = (cameraDataStruct*)realloc(globalCameraDataTable,sizeof(cameraDataStruct)*numGlobalCamera);

    /*    roomCameraData+=0x14;

    }*/
}
