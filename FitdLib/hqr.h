#ifndef _HQR_
#define _HQR_
char* HQR_Get(hqrEntryStruct* hqrPtr, int index);
int HQ_Malloc(hqrEntryStruct* hqrPtr,int size);
char* HQ_PtrMalloc(hqrEntryStruct* hqrPtr, int index);
hqrEntryStruct* HQR_InitRessource(const char* name, int size, int numEntries);
hqrEntryStruct* HQR_Init(int size,int numEntry);
void HQR_Reset(hqrEntryStruct* hqrPtr);
void HQR_Free(hqrEntryStruct* hqrPtr);

sBody* getBodyFromPtr(void* ptr);
sAnimation* getAnimationFromPtr(void* ptr);

#endif
