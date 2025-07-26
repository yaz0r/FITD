#include "common.h"

/*
Room data:

u16 offsetToCameraCoverZones
u16 offsetToHardCol
s16 roomPositionX
s16 roomPositionY
s16 roomPositionZ

etageVar1 -> table for camera data

*/

std::vector<roomDataStruct> roomDataTable;
std::vector<cameraDataStruct*> cameraDataTable;
std::vector<cameraViewedRoomStruct*> currentCameraZoneList;

roomDefStruct* getRoomData(int roomNumber)
{
    return (roomDefStruct*)(g_currentFloorRoomRawData + READ_LE_U32(g_currentFloorRoomRawData + roomNumber * 4));
}

int getNumberOfRoom()
{
    if (g_currentFloorRoomRawData) {
        int numMax = (((READ_LE_U32(g_currentFloorRoomRawData)) / 4));

        int j = 0;
        for (int i = 0; i < numMax; i++)
        {
            if (g_currentFloorRoomRawDataSize >= READ_LE_U32(g_currentFloorRoomRawData + i * 4))
            {
                j++;
            }
            else
            {
                return j;
            }
        }
        return j;
    }
    else {
        if (fileExists(std::format("ETAGE{:02d}.PAK", g_currentFloor).c_str())) {
            return PAK_getNumFiles(std::format("ETAGE{:02d}", g_currentFloor).c_str());
        }
        else if (fileExists(std::format("SAL{:02d}.PAK", g_currentFloor).c_str())) {
            return PAK_getNumFiles(std::format("SAL{:02d}", g_currentFloor).c_str());
        }
        else {
            assert(0);
        }
    }

    assert(false);
    return 0;
}

void ChangeSalle(int roomNumber)
{
    int i;
    int cameraVar0;
    int cameraVar1;
    int cameraVar2;
    int oldCameraIdx;
    roomDefStruct* roomDataPtr;

    SaveTimerAnim();

    ASSERT(roomNumber >=0);

    if(NumCamera == -1)
    {
        oldCameraIdx = -1;
    }
    else
    {
        cameraVar0 = roomDataTable[currentRoom].worldX;
        cameraVar1 = roomDataTable[currentRoom].worldY;
        cameraVar2 = roomDataTable[currentRoom].worldZ;

        oldCameraIdx = roomDataTable[currentRoom].cameraIdxTable[NumCamera];
    }

    if(g_gameId < AITD3)
    {
        cameraPtr = (char*)getRoomData(roomNumber); // TODO: obsolete
        roomDataPtr = getRoomData(roomNumber);
        pCurrentRoomData = getRoomData(roomNumber);
    }

    currentRoom = roomNumber;

    numCameraInRoom = roomDataTable[roomNumber].numCameraInRoom;

    ASSERT(numCameraInRoom < NUM_MAX_CAMERA_IN_ROOM);

    /*
    var_20 = cameraPtr + roomDataPtr->offsetToPosDef;
    numCameraZone = *(s16*)var_20;
    var_20 += 2;
    cameraZoneData = var_20;

    var_20 = cameraPtr + roomDataPtr->offsetToCameraDef;
    numRoomZone = *(s16*)var_20;
    var_20 += 2;
    roomZoneData = var_20;*/

    ASSERT(numCameraInRoom < NUM_MAX_CAMERA_IN_ROOM);

    int newNumCamera = 0;
    int newAbsCamera = -1;

    cameraDataTable.clear();

    currentCameraZoneList.clear();
    currentCameraZoneList.resize(numCameraInRoom, nullptr);
    
    // load the new camera table and try to keep the same camera (except if changing floor)
    for(i=0;i<numCameraInRoom;i++)
    {
        unsigned int currentCameraIdx = roomDataTable[currentRoom].cameraIdxTable[i]; // indexes are between the roomDefStruct and the first zone data

        ASSERT(currentCameraIdx <= g_currentFloorCameraData.size());

        if(oldCameraIdx == currentCameraIdx)
        {
            newNumCamera = i;
            newAbsCamera = currentCameraIdx;
        }

        if(g_gameId < AITD3)
        {
            room_PtrCamera[i] = g_currentFloorCameraRawData + READ_LE_U32(g_currentFloorCameraRawData + currentCameraIdx * 4);
        }

        cameraDataTable.push_back(&g_currentFloorCameraData.at(currentCameraIdx));

        currentCameraIdx = cameraDataTable[i]->numViewedRooms;

        // scan for the zone data related to the current room
        unsigned int j;
        for(j=0;j<currentCameraIdx;j++)
        {
            if(cameraDataTable[i]->viewedRoomTable[j].viewedRoomIdx == currentRoom)
                break;
        }

        ASSERT(cameraDataTable[i]->viewedRoomTable[j].viewedRoomIdx == currentRoom);
        if(cameraDataTable[i]->viewedRoomTable[j].viewedRoomIdx == currentRoom) {
            currentCameraZoneList[i] = &cameraDataTable[i]->viewedRoomTable[j];
        }
    }

    // reajust world coordinates
    if(oldCameraIdx != -1) // if a camera was selected before loading room
    {
        int var_E = (roomDataTable[roomNumber].worldX - cameraVar0) * 10;
        int var_C = (roomDataTable[roomNumber].worldY - cameraVar1) * 10;
        int var_A = (roomDataTable[roomNumber].worldZ - cameraVar2) * 10;

        for(i=0;i<NUM_MAX_OBJECT;i++)
        {
            if(ListObjets[i].indexInWorld != -1)
            {
                /*       if(gameId == AITD1) // special case. In AITD1, the load room function was always triggered just after the actor was moved in the new room.
                // it is not always the case in later games. Maybe we could generalize the AITD2 way...
                {
                actorTable[i].worldX -= var_E;
                actorTable[i].worldY += var_C;
                actorTable[i].worldZ += var_A;
                }
                else*/
                {
                    if(i!=currentCameraTargetActor)
                    {
                        ListObjets[i].worldX -= var_E;
                        ListObjets[i].worldY += var_C;
                        ListObjets[i].worldZ += var_A;
                    }
                    else
                    {
                        ListObjets[i].worldX = ListObjets[i].roomX;
                        ListObjets[i].worldY = ListObjets[i].roomY;
                        ListObjets[i].worldZ = ListObjets[i].roomZ;
                    }
                }
            }
        }
    }

    NewNumCamera = newNumCamera;    
    FlagInitView = 1;
    FlagChangeSalle = 0;
    RestoreTimerAnim();
}
