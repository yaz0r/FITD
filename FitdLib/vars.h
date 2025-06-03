#pragma  once

#include "osystem.h"

extern char* currentFoundBody;
extern int currentFoundBodyIdx;
extern int statusVar1;

enum gameTypeEnum
{
    AITD1,
    JACK,
    AITD2,
    AITD3,
    TIMEGATE,
};

extern enum gameTypeEnum g_gameId;

struct point3dStruct
{
    s16 x;
    s16 y;
    s16 z;
};

typedef struct point3dStruct point3dStruct;

struct textEntryStruct
{
    s16 index;
    u8* textPtr;
    s16 width;
};

typedef struct textEntryStruct textEntryStruct;

struct messageStruct
{
    textEntryStruct* string;
    s16 time;
};

typedef struct messageStruct messageStruct;

struct saveEntry
{
    void* ptr;
    unsigned int size;
};

typedef struct saveEntry saveEntry;

struct regularTextEntryStruct
{
    u8* textPtr;
    s16 width;
};

typedef struct regularTextEntryStruct regularTextEntryStruct;

struct hqrSubEntryStruct
{
    s16 key;
    s16 size;
    unsigned int lastTimeUsed;
    char* ptr;
};

typedef struct hqrSubEntryStruct hqrSubEntryStruct;

struct hqrEntryStruct
{
    char string[10];
    u16 maxFreeData;
    u16 sizeFreeData;
    u16 numMaxEntry;
    u16 numUsedEntry;
    hqrSubEntryStruct* entries;
};

typedef struct hqrEntryStruct hqrEntryStruct;

struct ZVStruct
{
    s32 ZVX1;
    s32 ZVX2;
    s32 ZVY1;
    s32 ZVY2;
    s32 ZVZ1;
    s32 ZVZ2;
};

typedef struct ZVStruct ZVStruct;

struct interpolatedValue
{
    s16 oldAngle;
    s16 newAngle;
    s16 param;
    unsigned int timeOfRotate;
};

typedef struct interpolatedValue interpolatedValue;

enum actorFlags
{
    AF_ANIMATED     = 0x0001,
    // 0x02
    AF_DRAWABLE     = 0x0004,
    AF_BOXIFY       = 0x0008,
    AF_MOVABLE      = 0x0010,
    AF_SPECIAL      = 0x0020,
    AF_TRIGGER      = 0x0040,
    AF_FOUNDABLE    = 0x0080,
    AF_FALLABLE     = 0x0100,

    AF_MASK         = AF_ANIMATED + AF_MOVABLE + AF_TRIGGER + AF_FOUNDABLE + AF_FALLABLE,
};

struct tObject // used to read data from file too
{
    s16 indexInWorld;
    s16 bodyNum;
    u16 _flags;
    s16 dynFlags;
    ZVStruct zv;
    s16 screenXMin;
    s16 screenYMin;
    s16 screenXMax;
    s16 screenYMax;
    s16 roomX;
    s16 roomY;
    s16 roomZ;
    s16 worldX;
    s16 worldY;
    s16 worldZ;
    s16 alpha;
    s16 beta;
    s16 gamma;
    s16 stage;
    s16 room;
    s16 lifeMode;
    s16 life;
    unsigned int CHRONO;
    unsigned int ROOM_CHRONO;
    s16 ANIM;
    s16 animType;
    s16 animInfo;
    s16 newAnim;
    s16 newAnimType;
    s16 newAnimInfo;
    s16 FRAME;
    s16 numOfFrames;
    s16 END_FRAME;
    s16 END_ANIM;
    s16 trackMode;
    s16 trackNumber;
    s16 MARK;
    s16 positionInTrack;

    s16 stepX;
    s16 stepY;
    s16 stepZ;

	s16 animNegX;
	s16 animNegY;
	s16 animNegZ;

    interpolatedValue YHandler;
    s16 falling;
    interpolatedValue rotate;
    s16 direction;
    s16 speed;
    interpolatedValue speedChange;
    s16 COL[3];
    s16 COL_BY;
    s16 HARD_DEC;
    s16 HARD_COL;
    s16 HIT;
    s16 HIT_BY;
    s16 animActionType;
    s16 animActionANIM;
    s16 animActionFRAME;
    s16 animActionParam;
    s16 hitForce;
    s16 hotPointID;
    point3dStruct hotPoint;

    // aitd2
    s16 hardMat;
};

typedef struct tObject tObject;

struct tWorldObject
{
    s16 objIndex;
    s16 body;
    union
    {
        s16 flags;
        actorFlags bitField;
    };
    s16 typeZV;
    s16 foundBody;
    s16 foundName;
    s16 flags2;
    s16 foundLife;
    s16 x;
    s16 y;
    s16 z;
    s16 alpha;
    s16 beta;
    s16 gamma;
    s16 stage;
    s16 room;
    s16 lifeMode;
    s16 life;
    s16 floorLife;
    s16 anim;
    s16 frame;
    s16 animType;
    s16 animInfo;
    s16 trackMode;
    s16 trackNumber;
    s16 positionInTrack;

    // AITD2
    s16 mark;
};

typedef struct tWorldObject tWorldObject;

struct boxStruct
{
    s16 var0;
    s16 var1;
    s16 var2;
    s16 var3;
};

typedef struct boxStruct boxStruct;

struct roomDefStruct
{
    s16 offsetToCameraDef; // 0
    s16 offsetToPosDef; // 2
    s16 worldX;//4
    s16 worldY;//6
    s16 worldZ;//8
    s16 numCameraInRoom;//0xA
};

typedef struct roomDefStruct roomDefStruct;

extern hqrEntryStruct* HQ_Memory;

extern hqrEntryStruct* listMus;
extern hqrEntryStruct* listSamp;

extern int videoMode;
extern int musicConfigured;
extern int musicEnabled;

extern char* aux;
extern char* aux2;

#define NB_BUFFER_ANIM 25 // AITD1 was  20
#define SIZE_BUFFER_ANIM (8*41) // AITD1 was 4*31

extern std::vector<std::vector<s16>> BufferAnim;

extern char* logicalScreen;

extern int screenBufferSize;
extern int unkScreenVar2;

extern std::vector<s16> CVars;

extern char* PtrPrioritySample;

extern char* PtrFont;

extern char* PtrCadre;

//extern OSystem osystem;
extern unsigned char frontBuffer[320*200];
extern char rgbaBuffer[320*200*4];

extern unsigned int timer;
extern unsigned int timeGlobal;

extern int WindowX1;
extern int WindowY1;
extern int WindowX2;
extern int WindowY2;

extern textEntryStruct* tabTextes;
extern u8* systemTextes;

extern "C" {
    extern char JoyD;
};
extern char Click;
extern char key;
extern char localKey;
extern char localJoyD;
extern char localClick;

extern char languageNameString[];
extern const std::vector<std::string> languageNameTable;

extern regularTextEntryStruct textTable[40];

extern int turnPageFlag;

extern int hqrKeyGen;

extern char* screenSm1;
extern char* screenSm2;
extern char* screenSm3;
extern char* screenSm4;
extern char* screenSm5;

extern tObject objectTable[NUM_MAX_OBJECT];

extern s16 currentWorldTarget;

extern int fileSize;

extern hqrEntryStruct* listBody;
extern hqrEntryStruct* listAnim;
extern hqrEntryStruct* listLife;
extern hqrEntryStruct* listTrack;
extern hqrEntryStruct* listMatrix;

extern s16 maxObjects;

extern std::vector<tWorldObject> ListWorldObjets; // may be less

extern s16* vars;

extern int varSize;

extern messageStruct messageTable[5];

extern s16 currentMusic;
extern int action;

extern boxStruct genVar2[15]; // recheckSize
extern boxStruct genVar4[50];
extern boxStruct *genVar1;
extern boxStruct *genVar3;

extern int genVar5;
extern int genVar6;
extern int nextSample;
extern int nextMusic;
extern s16 currentCameraTargetActor;
extern s16 giveUp;
extern s16 lightOff;
extern int lightVar2;
extern int LastPriority;
extern int LastSample;
extern s16 statusScreenAllowed;

extern char* g_currentFloorRoomRawData;
extern char* g_currentFloorCameraRawData;

extern int changeFloor;
extern s16 currentCamera;
extern s16 g_currentFloor;
extern int needChangeRoom;

extern char* cameraPtr;
extern roomDefStruct* pCurrentRoomData;
extern s16 currentRoom;
extern int flagInitView;
extern int numCameraInRoom;
extern int numCameraZone;
extern char* cameraZoneData;
extern int numRoomZone;
extern char* roomZoneData;
extern char* room_PtrCamera[NUM_MAX_CAMERA_IN_ROOM];
extern int startGameVar1;

extern int transformX;
extern int transformY;
extern int transformZ;
extern int transformXCos;
extern int transformXSin;
extern int transformYCos;
extern int transformYSin;
extern int transformZCos;
extern int transformZSin;
extern bool transformUseX;
extern bool transformUseY;
extern bool transformUseZ;

extern int translateX;
extern int translateY;
extern int translateZ;

extern int cameraCenterX;
extern int cameraCenterY;
extern int cameraPerspective;
extern int cameraFovX;
extern int cameraFovY;

extern char currentCameraVisibilityList[30];

extern int actorTurnedToObj;

extern int currentProcessedActorIdx;
extern tObject* currentProcessedActorPtr;

extern int currentLifeActorIdx;
extern tObject* currentLifeActorPtr;
extern int currentLifeNum;

extern char* currentLifePtr;

s16 readNextArgument(const char* name = NULL);

extern bool cameraBackgroundChanged;
extern int flagRedraw;

extern float renderPointList[6400];

extern int numActorInList;
extern int sortedActorTable[NUM_MAX_OBJECT];

extern int angleCompX;
extern int angleCompZ;
extern int angleCompBeta;

extern int bufferAnimCounter;

extern int animCurrentTime;
extern int animKeyframeLength;
extern int animMoveX;
extern int animMoveY;
extern int animMoveZ;
extern int animStepZ;
extern int animStepX;
extern int animStepY;
extern char* animVar1;
extern char* animVar3;
extern char* animVar4;

extern s16 newFloor;

extern int fadeState;

extern char cameraBuffer[256];
extern char cameraBuffer2[256];
extern char cameraBuffer3[400];
extern char cameraBuffer4[400];

extern char* cameraBufferPtr;
extern char* cameraBuffer2Ptr;
extern char* cameraBuffer3Ptr;

extern int overlaySize1;
extern int overlaySize2;

extern int bgOverlayVar1;

extern s16 newRoom;

extern const char* listBodySelect[];
extern const char* listAnimSelect[];

extern s16 shakeVar1;
extern s16 shakingAmplitude;
extern unsigned int timerFreeze1;

extern hardColStruct* hardColTable[10];

extern s16 hardColStepX;
extern s16 hardColStepZ;

extern ZVStruct hardClip;

extern saveEntry saveTable[];

extern int HQ_Load;
extern int lightX;
extern int lightY;

extern int clipLeft;
extern int clipTop;
extern int clipRight;
extern int clipBottom;

extern unsigned char* g_MaskPtr;

#if defined(FITD_DEBUGGER)
enum backgroundModeEnum
{
    backgroundModeEnum_2D,
    backgroundModeEnum_3D,
};

typedef enum backgroundModeEnum backgroundModeEnum;

extern backgroundModeEnum backgroundMode;
#endif

struct sGroupState
{
    s16 m_type; // 8
    s16 m_delta[3]; // A
    s16 m_rotateDelta[3]; // 10 (AITD2+) if Info_optimise
};

struct sGroup
{
    s16 m_start; // 0
    s16 m_numVertices; // 2
    s16 m_baseVertices; // 4
    s8 m_orgGroup; // 6
    s8 m_numGroup; // 7
    sGroupState m_state;
};

enum primTypeEnum
{
    primTypeEnum_Line = 0,
    primTypeEnum_Poly = 1,
    primTypeEnum_Point = 2,
    primTypeEnum_Sphere = 3,
    primTypeEnum_Disk = 4,
    primTypeEnum_Cylinder = 5,
    primTypeEnum_BigPoint = 6,
    primTypeEnum_Zixel = 7,
    processPrim_PolyTexture8 = 8,
    processPrim_PolyTexture9 = 9,
    processPrim_PolyTexture10 = 10,
};

struct sPrimitive
{
    primTypeEnum m_type;
    u8 m_material;
    u8 m_color;
    u8 m_even;
    u16 m_size;
    std::vector<u16> m_points;
};

struct sExtraBody
{
    u16 m_startOfKeyframe; // 2
};

// scratch buffer:
// 4: u16 timer


struct sBody
{
    void* m_raw;

    u16 m_flags;
    ZVStruct m_zv;
    std::vector<u8> m_scratchBuffer;
    std::vector<point3dStruct> m_vertices;
    std::vector<uint16> m_groupOrder;
    std::vector<sGroup> m_groups;
    std::vector<sPrimitive> m_primitives;

    void sync();
};

struct sFrame
{
    u16 m_timestamp;
    s16 m_animStep[3];
    std::vector<sGroupState> m_groups;
};

struct sAnimation
{
    void* m_raw;

    u16 m_numFrames;
    u16 m_numGroups;
    std::vector<sFrame> m_frames;
};

