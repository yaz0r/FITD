#ifndef _MAIN_
#define _MAIN_

extern int input5;

void menuWaitVSync();
void AffSpfI(int x, int y, int param, char* gfx);
void processActor2();
void loadFloor(int floorNumber);
void loadRoom(int roomNumber);
void executeFoundLife(int objIdx);
void setupCamera();
void checkIfCameraChangeIsRequired(void);
void updateAllActorAndObjects();
void createActorList();
void mainDraw(int mode);
int checkObjectCollisions(int actorIdx, ZVStruct* zvPtr);
int checkLineProjectionWithActors( int actorIdx, int X, int Y, int Z, int beta, int room, int param );
void GiveZVObjet(char* bodyPtr, ZVStruct* zvPtr);
int AsmCheckListCol(ZVStruct* zvPtr, roomDataStruct* pRoomData);
void DeleteInventoryObjet(int objIdx);
void walkStep(int angle1, int angle2, int angle3);
void throwStoppedAt(int x, int z);
sceZoneStruct* processActor2Sub(int x, int y, int z, roomDataStruct* pRoomData);
void initEngine(void);
void initVars();
void configureHqrHero(hqrEntryStruct* hqrPtr, const char* name);
void flushScreen(void);
int parseAllSaves(int arg);
textEntryStruct* getTextFromIdx(int index);
void cleanupAndExit(void);
void drawProjectedBox(int x1,int x2,int y1,int y2,int z1,int z2, int color, int transparency);
void HQ_Free_Malloc(hqrEntryStruct* hqrPtr, int index);

void playSequence(int sequenceIdx, int fadeStart, int fadeOutVar);

bool fileExists(const char* name);

void OpenProgram(void);
void freeAll(void);
void startGame(int startupEtage, int startupRoom, int allowSystemMenu);
void fillBox(int x1, int y1, int x2, int y2, char color); // fast recode. No RE
void copyZv(ZVStruct* source, ZVStruct* dest);
void getZvRelativePosition(ZVStruct* zvPtr, int startRoom, int destRoom);
void take(int objIdx);
void foundObject(int objIdx, int param);
void hit(int animNumber,int arg_2,int arg_4,int arg_6,int hitForce,int arg_A);
int checkZvCollision(ZVStruct* zvPtr1,ZVStruct* zvPtr2);
void getZvCube(char* bodyPtr, ZVStruct* zvPtr);
void PutAtObjet(int objIdx, int objIdxToPutAt);
void SetClip(int left, int top, int right, int bottom);

///
void readBook(int index, int type);
int Lire(int index, int left, int top, int right, int bottom, int mode, int color, int shadow);

void setupCameraProjection(int centerX, int centerY, int x, int y, int z);
void SetAngleCamera(int x, int y, int z);
void SetPosCamera(int x, int y, int z);
s16 computeDistanceToPoint(int x1, int z1, int x2, int z2);
void setMoveMode(int trackMode, int trackNumber);
void InitRealValue(s16 beta, s16 newBeta, s16 param, interpolatedValue* rotatePtr);
s16 updateActorRotation(interpolatedValue* rotatePtr);
s16 GetNbFramesAnim(char* animPtr);
int SetAnimObjet(int frame, char* anim, char* body);
void deleteObject(int objIdx);
void removeFromBGIncrust(int actorIdx);
void makeMessage(int messageIdx);
int drawTextOverlay(void);
s16 SetInterAnimObjet(int frame, char* animPtr, char* bodyPtr);
void handleCollision(ZVStruct* startZv, ZVStruct* zvPtr2, ZVStruct* zvPtr3);

void getZvRot(char* bodyPtr, ZVStruct* zvPtr, int alpha, int beta, int gamma);

void addActorToBgInscrust(int actorIdx);

#endif

