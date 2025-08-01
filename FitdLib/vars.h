#pragma  once

#include "osystem.h"

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

struct ZVStruct16
{
    s16 ZVX1;
    s16 ZVX2;
    s16 ZVY1;
    s16 ZVY2;
    s16 ZVZ1;
    s16 ZVZ2;
};

struct RealValue
{
    s16 startValue;
    s16 endValue;
    s16 numSteps;
    unsigned int memoTicks;
};

typedef struct RealValue RealValue;

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

    // >= JACK
    AF_OBJ_2D       = 0x0200,
    AF_WATER        = 0x0400,

    AF_MASK         = AF_ANIMATED + AF_MOVABLE + AF_TRIGGER + AF_FOUNDABLE + AF_FALLABLE + AF_WATER,
};

struct tObject // used to read data from file too
{
    s16 indexInWorld;
    s16 bodyNum;
    u16 objectType;
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
    s16 frame;
    s16 numOfFrames;
    s16 END_FRAME;
    s16 flagEndAnim;
    u32 memoTicks; // for 2d objects
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

    RealValue YHandler;
    s16 falling;
    RealValue rotate;
    s16 direction;
    s16 speed;
    RealValue speedChange;
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
    s16 foundFlag;
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

#include "hqr.h"
extern hqrEntryStruct<char>* HQ_Memory;
extern hqrEntryStruct<char>* listMus;
extern hqrEntryStruct<char>* listSamp;

extern int videoMode;
extern int musicConfigured;
extern int musicEnabled;

extern char* aux;
extern char* aux2;

extern char* logicalScreen;

extern int screenBufferSize;
extern int unkScreenVar2;

extern std::vector<s16> CVars;

extern char* PtrPrioritySample;

extern char* PtrFont;

extern char* PtrCadre;

//extern OSystem osystem;
extern unsigned char frontBuffer[320*200];

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

extern std::string languageNameString;
extern const std::vector<std::string> languageNameTable;

extern regularTextEntryStruct textTable[40];

extern int turnPageFlag;

extern int hqrKeyGen;

extern char* screenSm1;
extern char* screenSm2;
extern char* screenSm3;
extern char* screenSm4;
extern char* screenSm5;

extern std::array<tObject, NUM_MAX_OBJECT> ListObjets;

extern s16 currentWorldTarget;

extern int fileSize;

extern hqrEntryStruct<sBody>* HQ_Bodys;
extern hqrEntryStruct<char>* listLife;
extern hqrEntryStruct<char>* listTrack;
extern hqrEntryStruct<char>* HQ_Matrices;

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

extern int NbPhysBoxs;
extern int NbLogBoxs;
extern int nextSample;
extern int nextMusic;
extern s16 currentCameraTargetActor;
extern s16 FlagGameOver;
extern s16 lightOff;
extern int lightVar2;
extern int LastPriority;
extern int LastSample;
extern s16 statusScreenAllowed;

extern char* g_currentFloorRoomRawData;
extern char* g_currentFloorCameraRawData;

extern int FlagChangeEtage;
extern s16 NumCamera;
extern s16 g_currentFloor;
extern int FlagChangeSalle;

extern char* cameraPtr;
extern roomDefStruct* pCurrentRoomData;
extern s16 currentRoom;
extern int FlagInitView;
extern int numCameraInRoom;
extern int numCameraZone;
extern char* cameraZoneData;
extern int numRoomZone;
extern char* roomZoneData;
extern char* room_PtrCamera[NUM_MAX_CAMERA_IN_ROOM];
extern int NewNumCamera;

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

extern int FlagGenereAffList;

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

extern int NbAffObjets;
extern std::array<int, NUM_MAX_OBJECT> Index;

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

extern s16 NewNumEtage;

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

extern s16 NewNumSalle;

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

extern std::array<unsigned char, 320 * 200> uiLayer;

#if defined(FITD_DEBUGGER)
enum backgroundModeEnum
{
    backgroundModeEnum_2D,
    backgroundModeEnum_3D,
};

typedef enum backgroundModeEnum backgroundModeEnum;

extern backgroundModeEnum backgroundMode;

enum collisiosDisplayMode : int
{
    displayNone,
    displayWired,
    displayFull,
};
extern collisiosDisplayMode hardColDisplayMode;
extern collisiosDisplayMode sceColDisplayMode;

#endif

struct sGroupState
{
    s16 m_type; // 0
    point3dStruct m_delta; // 2
    // (AITD2+) if Info_optimise
    std::optional<point3dStruct> m_rotateDelta; // 8
    // 8 / 0x10
};

struct sGroup
{
    s16 m_start; // 0
    s16 m_numVertices; // 2
    s16 m_baseVertices; // 4
    s8 m_orgGroup; // 6
    s8 m_numGroup; // 7
    sGroupState m_state;//8
    // 0x16 / 0x22 (AITD2+) if Info_optimise
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

    u16 m_flags; //0 size 0x2
    ZVStruct16 m_zv; //2 size 0xC
    struct sFrame* startAnim = nullptr; // This is normally stored as 4 bytes at the beginning of scratchBuffer, but we store it here due to pointer size
    std::vector<u8> m_scratchBuffer; //0xE size u16 + data
    std::vector<point3dStruct> m_vertices; // size u16 count * 6
    std::vector<uint16> m_groupOrder; // size u16 * 2
    std::vector<sGroup> m_groups; // size u16 
    std::vector<sPrimitive> m_primitives;
};
