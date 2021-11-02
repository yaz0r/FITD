#ifndef _FLOOR_H_
#define _FLOOR_H_

extern std::vector<cameraDataStruct> g_currentFloorCameraData;
extern u32 g_currentFloorRoomRawDataSize;
extern u32 g_currentFloorNumCamera;

void loadFloor(int floorNumber);

#endif

