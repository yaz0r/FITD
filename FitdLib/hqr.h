#pragma once

template <typename T>
struct hqrSubEntryStruct;

template <typename T>
struct hqrEntryStruct;

template <typename T>
T* HQR_Get(hqrEntryStruct<T>* hqrPtr, int index);

hqrEntryStruct<char>* HQR_Init(int size, int numEntry);
int HQ_Malloc(hqrEntryStruct<char>* hqrPtr,int size);
char* HQ_PtrMalloc(hqrEntryStruct<char>* hqrPtr, int index);
void HQ_Free_Malloc(hqrEntryStruct<char>* hqrPtr, int index);

template <typename T>
void HQR_Free(hqrEntryStruct<T>* hqrPtr);

template <typename T>
hqrEntryStruct<T>* HQR_InitRessource(const char* name, int size, int numEntries);

template <typename T>
void HQR_Reset(hqrEntryStruct<T>* hqrPtr);

template <typename T>
void configureHqrHero(hqrEntryStruct<T>* hqrPtr, const char* name);

struct sBody* createBodyFromPtr(void* ptr);


