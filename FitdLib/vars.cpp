#include "common.h"

enum gameTypeEnum g_gameId;

char* currentFoundBody;
int currentFoundBodyIdx;
int statusVar1;

hqrEntryStruct* HQ_Memory;

int videoMode;
int musicConfigured;
int musicEnabled;

char* aux;
char* aux2;
std::vector<std::vector<s16>> BufferAnim;

char* logicalScreen;

int screenBufferSize;
int unkScreenVar2;

std::vector<s16> CVars;

char* PtrPrioritySample;

char* PtrFont;

char* PtrCadre;

unsigned char currentGamePalette[256*3];

//OSystem osystem;

char rgbaBuffer[320*200*4];

unsigned int timer;
unsigned int timeGlobal;

int currentMenuLeft;
int currentMenuTop;
int currentMenuRight;
int currentMenuBottom;

textEntryStruct* tabTextes;
u8* systemTextes;

char JoyD = 0;
char Click = 0;
char key = 0;
char localKey;
char localJoyD;
char localClick;

const std::vector<std::string> languageNameTable =
{
    "FRANCAIS",
    "ITALIANO",
    "ENGLISH",
    "ESPAGNOL",
    "DEUTSCH",
};

char languageNameString[20] = "";

regularTextEntryStruct textTable[NUM_MAX_TEXT];

int turnPageFlag;

int hqrKeyGen = 0;

char* screenSm1;
char* screenSm2;
char* screenSm3;
char* screenSm4;
char* screenSm5;

tObject objectTable[NUM_MAX_OBJECT];

s16 currentWorldTarget;

int fileSize;

hqrEntryStruct* listBody;
hqrEntryStruct* listAnim;
hqrEntryStruct* listLife;
hqrEntryStruct* listTrack;
hqrEntryStruct* listMatrix;

s16 maxObjects;

std::vector<tWorldObject> worldObjects;

s16* vars;

int varSize;

messageStruct messageTable[NUM_MAX_MESSAGE];

s16 currentMusic;
int action;

boxStruct genVar2[15]; // recheckSize
boxStruct genVar4[50];
boxStruct *genVar1;
boxStruct *genVar3;

int genVar5;
int genVar6;
int nextSample;
int nextMusic;
s16 currentCameraTargetActor;
s16 giveUp;
s16 lightOff;
int lightVar2;
int LastPriority;
int LastSample;
s16 statusScreenAllowed;

char* g_currentFloorRoomRawData = NULL;
char* g_currentFloorCameraRawData = NULL;

int changeFloor;
s16 currentCamera;
s16 g_currentFloor;
int needChangeRoom;

char* cameraPtr;
roomDefStruct* pCurrentRoomData;

s16 currentRoom;
int flagInitView;
int numCameraInRoom;
int numCameraZone;
char* cameraZoneData;
int numRoomZone;
char* roomZoneData;
char* room_PtrCamera[NUM_MAX_CAMERA_IN_ROOM];
int startGameVar1;

int transformX;
int transformY;
int transformZ;
int transformXCos;
int transformXSin;
int transformYCos;
int transformYSin;
int transformZCos;
int transformZSin;
bool transformUseX;
bool transformUseY;
bool transformUseZ;

int translateX;
int translateY;
int translateZ;

int cameraCenterX;
int cameraCenterY;
int cameraPerspective;
int cameraFovX;
int cameraFovY;

char currentCameraVisibilityList[30];

int actorTurnedToObj = 0;

int currentProcessedActorIdx;
tObject* currentProcessedActorPtr;

int currentLifeActorIdx;
tObject* currentLifeActorPtr;
int currentLifeNum;

char* currentLifePtr;

s16 readNextArgument(const char* name)
{
    s16 value = *(s16*)(currentLifePtr);
    currentLifePtr+=2;

    if (name)
    {
        appendFormated("%s:%d, ",name, value);
    }
    else
    {
        appendFormated("%d, ", value);
    }

    return value;
}

bool cameraBackgroundChanged = false;
int flagRedraw;

float renderPointList[6400];

int numActorInList;
int sortedActorTable[NUM_MAX_OBJECT];

int angleCompX;
int angleCompZ;
int angleCompBeta;

int bufferAnimCounter = 0;

int animCurrentTime;
int animKeyframeLength;

int animMoveX;
int animMoveY;
int animMoveZ;

int animStepX;
int animStepZ;
int animStepY;

char* animVar1;
char* animVar3;
char* animVar4;

s16 newFloor;

int fadeState;

char cameraBuffer[256];
char cameraBuffer2[256];
char cameraBuffer3[400];
char cameraBuffer4[400];

char* cameraBufferPtr = cameraBuffer;
char* cameraBuffer2Ptr = cameraBuffer2;
char* cameraBuffer3Ptr = cameraBuffer3;

int overlaySize1;
int overlaySize2;

int bgOverlayVar1;

s16 newRoom;



s16 shakeVar1;
s16 shakingAmplitude;
unsigned int timerFreeze1;

hardColStruct* hardColTable[10];

s16 hardColStepX;
s16 hardColStepZ;

ZVStruct hardClip;

const char* listBodySelect[] = {
    "LISTBODY",
    "LISTBOD2",
};

const char* listAnimSelect[] = {
    "LISTANIM",
    "LISTANI2",
};

saveEntry saveTable[40];

int HQ_Load = 0;
int lightX = 4000;
int lightY = -2000;

int clipLeft = 0;
int clipTop = 0;
int clipRight = 319;
int clipBottom = 119;

unsigned char* g_MaskPtr = NULL;

hqrEntryStruct* listMus;
hqrEntryStruct* listSamp;

#if defined(FITD_DEBUGGER)
backgroundModeEnum backgroundMode = backgroundModeEnum_2D;
#endif
