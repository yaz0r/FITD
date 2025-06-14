#ifndef _ROOM_H_
#define _ROOM_H_

struct hardColStruct;
typedef struct hardColStruct hardColStruct;

#include "vars.h" // temporary fix to cross include

struct hardColStruct
{
  ZVStruct zv;
  u32 type;
  u32 parameter;
};

struct sceZoneStruct
{
  ZVStruct zv;
  u32 type;
  u32 parameter;
};

typedef struct sceZoneStruct sceZoneStruct;

struct cameraZonePointStruct
{
  s16 x;
  s16 y;
};

typedef struct cameraZonePointStruct cameraZonePointStruct;

struct cameraZoneEntryStruct
{
  u16 numPoints;

  cameraZonePointStruct* pointTable;
};

typedef struct cameraZoneEntryStruct cameraZoneEntryStruct;

struct rectTestStruct
{
	s16 zoneX1;
	s16 zoneZ1;
	s16 zoneX2;
	s16 zoneZ2;
};

struct cameraMaskStruct
{
	u16 numTestRect;
	std::vector<rectTestStruct> rectTests;
};

struct cameraViewedRoomStruct
{
  s16 viewedRoomIdx;
  s16 offsetToMask;
  s16 offsetToCover;
  s16 offsetToHybrids;
  s16 offsetCamOptims;
  s16 lightX;
  s16 lightY;
  s16 lightZ;

  u16 numMask;
  std::vector<cameraMaskStruct> masks;
  u16 numCoverZones;
  std::vector<cameraZoneEntryStruct> coverZones;
};

struct cameraDataStruct
{
  s16 alpha;
  s16 beta;
  s16 gamma;

  s16 x;
  s16 y;
  s16 z;

  s16 focal1;
  s16 focal2;
  s16 focal3;

  u16 numViewedRooms;
  std::vector<cameraViewedRoomStruct> viewedRoomTable;
};

struct roomDataStruct
{
  u32 numCameraInRoom;

  u32 numHardCol;
  std::vector<hardColStruct> hardColTable;

  u32 numSceZone;
  std::vector<sceZoneStruct> sceZoneTable;

  s32 worldX;
  s32 worldY;
  s32 worldZ;

  std::vector<u16> cameraIdxTable;
};
typedef struct roomDataStruct roomDataStruct;

extern std::vector<cameraDataStruct*> cameraDataTable;
extern std::vector<cameraViewedRoomStruct*> currentCameraZoneList;
extern std::vector<roomDataStruct> roomDataTable;

roomDefStruct* getRoomData(int roomNumber);
void ChangeSalle(int roomNumber);
int getNumberOfRoom();

#endif
