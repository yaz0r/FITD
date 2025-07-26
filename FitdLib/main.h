#ifndef _MAIN_
#define _MAIN_

extern int AntiRebond;

void menuWaitVSync();
void AffSpfI(int x, int y, int param, char* gfx);
void GereDec();
void LoadEtage(int floorNumber);
void ChangeSalle(int roomNumber);
void executeFoundLife(int objIdx);
void InitView();
void GereSwitchCamera(void);
void GenereActiveList();
void GenereAffList();
void AllRedraw(int mode);
int CheckObjectCol(int actorIdx, ZVStruct* zvPtr);
int checkLineProjectionWithActors( int actorIdx, int X, int Y, int Z, int beta, int room, int param );
int AsmCheckListCol(ZVStruct* zvPtr, roomDataStruct* pRoomData);
void DeleteInventoryObjet(int objIdx);
void walkStep(int angle1, int angle2, int angle3);
void throwStoppedAt(int x, int z);
sceZoneStruct* processActor2Sub(int x, int y, int z, roomDataStruct* pRoomData);
void LoadWorld(void);
void initVars();

void flushScreen(void);
int parseAllSaves(int arg);
textEntryStruct* getTextFromIdx(int index);
void cleanupAndExit(void);
void drawProjectedBox(int x1,int x2,int y1,int y2,int z1,int z2, int color, int transparency);

void playSequence(int sequenceIdx, int fadeStart, int fadeOutVar);

bool fileExists(const char* name);

void OpenProgram(void);
void freeAll(void);
void startGame(int startupEtage, int startupRoom, int allowSystemMenu);
void AffRect(int x1, int y1, int x2, int y2, char color); // fast recode. No RE
void CopyZV(ZVStruct* source, ZVStruct* dest);
void AdjustZV(ZVStruct* zvPtr, int startRoom, int destRoom);
void take(int objIdx);
void FoundObjet(int objIdx, int param);
void hit(int animNumber,int arg_2,int arg_4,int arg_6,int hitForce,int arg_A);
int CubeIntersect(ZVStruct* zvPtr1,ZVStruct* zvPtr2);
void PutAtObjet(int objIdx, int objIdxToPutAt);
void SetClip(int left, int top, int right, int bottom);

void drawZv(const ZVStruct& localZv);

///
void readBook(int index, int type);
int Lire(int index, int left, int top, int right, int bottom, int mode, int color, int shadow);

void SetProjection(int centerX, int centerY, int x, int y, int z);
void SetAngleCamera(int x, int y, int z);
void SetPosCamera(int x, int y, int z);
s16 GiveDistance2D(int x1, int z1, int x2, int z2);
void InitDeplacement(int trackMode, int trackNumber);
void InitRealValue(s16 beta, s16 newBeta, s16 param, RealValue* rotatePtr);
s16 updateActorRotation(RealValue* rotatePtr);
void deleteObject(int objIdx);
void removeFromBGIncrust(int actorIdx);
void makeMessage(int messageIdx);
int drawTextOverlay(void);
void GereCollision(ZVStruct* startZv, ZVStruct* zvPtr2, ZVStruct* zvPtr3);

void addActorToBgInscrust(int actorIdx);

void setupPointRotate(int alpha, int beta, int gamma);
void pointRotate(int x, int y, int z, int* destX, int* destY, int* destZ);

#endif

