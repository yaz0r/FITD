#include "common.h"
#include "hybrid.h"

template <typename T>
struct hqrSubEntryStruct
{
    s16 key;
    s16 size;
    unsigned int lastTimeUsed;
    T* ptr;
};

template <typename T>
struct hqrEntryStruct
{
    std::string string;
    u16 maxFreeData;
    u16 sizeFreeData;
    u16 numMaxEntry;
    u16 numUsedEntry;
    std::vector<hqrSubEntryStruct<T>> entries;
};

template <typename T>
hqrSubEntryStruct<T>* quickFindEntry(int index, int numMax, std::vector<hqrSubEntryStruct<T>>& ptr) // no RE. Original was probably faster
{
    for(int i=0;i<numMax;i++)
    {
        if((ptr[i].key == index) && ptr[i].ptr)
        {
            return &ptr[i];
        }
    }

    return nullptr;
}

template <typename T>
hqrEntryStruct<T>* HQR_InitRessource(const char* name, int size, int numEntries)
{
    hqrEntryStruct<T>* dest = new hqrEntryStruct<T>();

    if(!dest)
        return NULL;

    numEntries = 2000;
    dest->string = name;
    dest->sizeFreeData = size;
    dest->maxFreeData = size;
    dest->numMaxEntry = numEntries;
    dest->numUsedEntry = 0;
    dest->entries.resize(numEntries);

    for(int i=0;i<numEntries;i++)
    {
        dest->entries[i].ptr = nullptr;
    }

    return(dest);
}

int HQ_Malloc(hqrEntryStruct<char>* hqrPtr,int size)
{
    int key;
    int entryNum;

    if(hqrPtr->sizeFreeData<size)
        return(-1);

    entryNum = hqrPtr->numUsedEntry;

    key = hqrKeyGen;

    hqrPtr->entries[entryNum].key = key;

    //  dataPtr1[entryNum].offset = hqrPtr->maxFreeData - hqrPtr->sizeFreeData;
    hqrPtr->entries[entryNum].size = size;
    hqrPtr->entries[entryNum].ptr = (char*)malloc(size);

    hqrPtr->numUsedEntry++;
    hqrPtr->sizeFreeData -= size;

    hqrKeyGen++;

    return(key);
}

char* HQ_PtrMalloc(hqrEntryStruct<char>* hqrPtr, int index)
{

    if(index<0)
        return NULL;

    hqrSubEntryStruct<char>* ptr = quickFindEntry(index, hqrPtr->numUsedEntry, hqrPtr->entries);

    if(!ptr)
        return NULL;

    return(ptr->ptr);
}

sAnimation* createAnimationFromPtr(void* ptr)
{
    u8* animPtr = (u8*)ptr;

    sAnimation* pAnimation = new sAnimation;

    pAnimation->m_raw = ptr;
    pAnimation->m_numFrames = READ_LE_U16(animPtr); animPtr += 2;
    pAnimation->m_numGroups = READ_LE_U16(animPtr); animPtr += 2;

    pAnimation->m_frames.resize(pAnimation->m_numFrames);
    for (int i = 0; i < pAnimation->m_numFrames; i++)
    {
        sFrame* pFrame = &pAnimation->m_frames[i];

        pFrame->m_timestamp = READ_LE_U16(animPtr); animPtr += 2;
        pFrame->m_animStep.x = READ_LE_S16(animPtr); animPtr += 2;
        pFrame->m_animStep.y = READ_LE_S16(animPtr); animPtr += 2;
        pFrame->m_animStep.z = READ_LE_S16(animPtr); animPtr += 2;

        pFrame->m_groups.resize(pAnimation->m_numGroups);
        for (int i = 0; i < pAnimation->m_numGroups; i++)
        {
            sGroupState* pGroup = &pFrame->m_groups[i];

            pGroup->m_type = READ_LE_S16(animPtr); animPtr += 2;
            pGroup->m_delta.x = READ_LE_S16(animPtr); animPtr += 2;
            pGroup->m_delta.y = READ_LE_S16(animPtr); animPtr += 2;
            pGroup->m_delta.z = READ_LE_S16(animPtr); animPtr += 2;
        }
    }
    return pAnimation;
}

sBody* createBodyFromPtr(void* ptr)
{
    u8* bodyBuffer = (u8*)ptr;

    sBody* newBody = new sBody;

    newBody->m_raw = ptr;
    newBody->m_flags = READ_LE_U16(bodyBuffer); bodyBuffer += 2;

    newBody->m_zv.ZVX1 = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
    newBody->m_zv.ZVX2 = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
    newBody->m_zv.ZVY1 = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
    newBody->m_zv.ZVY2 = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
    newBody->m_zv.ZVZ1 = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
    newBody->m_zv.ZVZ2 = READ_LE_S16(bodyBuffer); bodyBuffer += 2;

    u16 scratchBufferSize = READ_LE_U16(bodyBuffer); bodyBuffer += 2;
    newBody->m_scratchBuffer.resize(scratchBufferSize);
    for (int i = 0; i < scratchBufferSize; i++)
    {
        newBody->m_scratchBuffer[i] = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
    }

    u16 numVertices = READ_LE_U16(bodyBuffer); bodyBuffer += 2;
    newBody->m_vertices.resize(numVertices);
    for (int i = 0; i < numVertices; i++)
    {
        newBody->m_vertices[i].x = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
        newBody->m_vertices[i].y = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
        newBody->m_vertices[i].z = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
    }

    if (newBody->m_flags & INFO_TORTUE)
    {
        assert(0); // never used
    }

    if (newBody->m_flags & INFO_ANIM)
    {
        u16 numGroups = READ_LE_U16(bodyBuffer); bodyBuffer += 2;
        newBody->m_groupOrder.reserve(numGroups);
        newBody->m_groups.resize(numGroups);

        if (newBody->m_flags & INFO_OPTIMISE) // AITD2+
        {
            for (int i = 0; i < numGroups; i++)
            {
                u16 offset = READ_LE_U16(bodyBuffer);
                assert(offset % 0x18 == 0);
                newBody->m_groupOrder.push_back(offset / 0x18);
                bodyBuffer += 2;
            }

            for (int i = 0; i < numGroups; i++)
            {
                newBody->m_groups[i].m_start = READ_LE_S16(bodyBuffer) / 6; bodyBuffer += 2;
                newBody->m_groups[i].m_numVertices = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_baseVertices = READ_LE_S16(bodyBuffer) / 6; bodyBuffer += 2;
                newBody->m_groups[i].m_orgGroup = READ_LE_S8(bodyBuffer); bodyBuffer += 1;
                newBody->m_groups[i].m_numGroup = READ_LE_S8(bodyBuffer); bodyBuffer += 1;
                newBody->m_groups[i].m_state.m_type = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta.x = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta.y = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta.z = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_rotateDelta.x = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_rotateDelta.y = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_rotateDelta.z = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_padding = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
            }
        }
        else
        {
            for (int i = 0; i < numGroups; i++)
            {
                u16 offset = READ_LE_U16(bodyBuffer);
                assert(offset % 0x10 == 0);
                newBody->m_groupOrder.push_back(offset / 0x10);
                bodyBuffer += 2;
            }

            for (int i = 0; i < numGroups; i++)
            {
                newBody->m_groups[i].m_start = READ_LE_S16(bodyBuffer) / 6; bodyBuffer += 2;
                newBody->m_groups[i].m_numVertices = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_baseVertices = READ_LE_S16(bodyBuffer) / 6; bodyBuffer += 2;
                newBody->m_groups[i].m_orgGroup = READ_LE_S8(bodyBuffer); bodyBuffer += 1;
                newBody->m_groups[i].m_numGroup = READ_LE_S8(bodyBuffer); bodyBuffer += 1;
                newBody->m_groups[i].m_state.m_type = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta.x = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta.y = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta.z = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
            }
        }

    }

    u16 numPrimitives = READ_LE_U16(bodyBuffer); bodyBuffer += 2;
    newBody->m_primitives.resize(numPrimitives);
    for (int i = 0; i < numPrimitives; i++)
    {
        newBody->m_primitives[i].m_type = (primTypeEnum)READ_LE_U8(bodyBuffer); bodyBuffer += 1;

        switch (newBody->m_primitives[i].m_type)
        {
        case primTypeEnum_Line:
            newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_points.resize(2);
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            break;
        case primTypeEnum_Poly:
            newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer)); bodyBuffer += 1;
            newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            break;
        case primTypeEnum_Point:
        case primTypeEnum_BigPoint:
        case primTypeEnum_Zixel:
            newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_points.resize(1);
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            break;
        case primTypeEnum_Sphere:
            newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_size = READ_LE_U16(bodyBuffer); bodyBuffer += 2;
            newBody->m_primitives[i].m_points.resize(1);
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            break;
        case processPrim_PolyTexture8:
            newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer)); bodyBuffer += 1;
            newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            break;
        case processPrim_PolyTexture9:
        case processPrim_PolyTexture10:
            newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer)); bodyBuffer += 1;
            newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            // load UVS?
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                READ_LE_U8(bodyBuffer); bodyBuffer += 1;
                READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            }
            break;
        default:
            assert(0);
            break;
        }
    }

    return newBody;
}

template <typename T>
T* HQR_Get(hqrEntryStruct<T>* hqrPtr, int index)
{
    hqrSubEntryStruct<T>* foundEntry;

    if(index<0)
        return NULL;

    foundEntry = quickFindEntry(index,hqrPtr->numUsedEntry,hqrPtr->entries);

    if(foundEntry)
    {
        foundEntry->lastTimeUsed = timer;
        HQ_Load = 0;

        return(foundEntry->ptr);
    }
    else
    {
        SaveTimerAnim();
        int size = getPakSize(hqrPtr->string.c_str(),index);

		if(size == 0)
			return NULL;

        if(size>=hqrPtr->maxFreeData)
        {
            fatalError(1,hqrPtr->string.c_str());
        }

        unsigned int time = timer;

        for(int i=0;i<hqrPtr->numMaxEntry;i++)
        {
            if(hqrPtr->entries[i].ptr == NULL)
            {
                foundEntry = &hqrPtr->entries[i];
                break;
            }
        }

        ASSERT(foundEntry);

        //    foundEntry = hqrSubPtr;

        HQ_Load = 1;

        foundEntry->key = index;
        foundEntry->lastTimeUsed = timer;
        //foundEntry[hqrPtr->numUsedEntry].offset = hqrPtr->maxFreeData - hqrPtr->sizeFreeData;
        foundEntry->size = size;

        char* buffer = new char[size];
        LoadPak(hqrPtr->string.c_str(), index, buffer);

        if constexpr (std::is_same_v<T, char>) {
            foundEntry->ptr = buffer;
        }
        else if constexpr (std::is_same_v<T, sBody>) {
            foundEntry->ptr = createBodyFromPtr(buffer);
            delete[] buffer;
        }
        else if constexpr (std::is_same_v<T, sAnimation>) {
            foundEntry->ptr = createAnimationFromPtr(buffer);
            delete[] buffer;
        }
        else if constexpr (std::is_same_v<T, sHybrid>) {
            foundEntry->ptr = new sHybrid((uint8_t*)buffer, size);
            delete[] buffer;
        }
        else {
            assert(0);
        }

        hqrPtr->numUsedEntry++;
        hqrPtr->sizeFreeData -= size;

        RestoreTimerAnim();

        return(foundEntry->ptr);
    }
}

hqrEntryStruct<char>* HQR_Init(int size,int numEntry)
{
    ASSERT(size > 0);
    ASSERT(numEntry > 0);

    hqrEntryStruct<char>* dest = new hqrEntryStruct<char>();

    numEntry = 2000;

    ASSERT_PTR(dest);

    if(!dest)
        return NULL;

    dest->string = "_MEMORY_";
    dest->sizeFreeData = size;
    dest->maxFreeData = size;
    dest->numMaxEntry = numEntry;
    dest->numUsedEntry = 0;
    dest->entries.resize(numEntry);

    for(int i=0;i<numEntry;i++)
    {
        dest->entries[i].ptr = nullptr;
    }

    return(dest);
}

template <typename T>
void HQR_Reset(hqrEntryStruct<T>* hqrPtr)
{
    hqrPtr->sizeFreeData = hqrPtr->maxFreeData;
    hqrPtr->numUsedEntry = 0;

    for(int i =0;i<hqrPtr->numMaxEntry;i++)
    {
        if (hqrPtr->entries[i].ptr)
        {
            if constexpr (std::is_same_v<T, char>) {
                delete[] hqrPtr->entries[i].ptr;
            }
            else {
                delete hqrPtr->entries[i].ptr;
            }
            hqrPtr->entries[i].ptr = nullptr;
        }
    }

}

template <typename T>
void HQR_Free(hqrEntryStruct<T>* hqrPtr)
{
    HQR_Reset(hqrPtr);

    delete hqrPtr;
}

template <typename T>
void configureHqrHero(hqrEntryStruct<T>* hqrPtr, const char* name)
{
    hqrPtr->string = name;
}

void HQ_Free_Malloc(hqrEntryStruct<char>* hqrPtr, int index)
{
}

template
hqrEntryStruct<char>* HQR_InitRessource(const char* name, int size, int numEntries);

template
char* HQR_Get(hqrEntryStruct<char>* hqrPtr, int index);

template
void HQR_Free(hqrEntryStruct<char>* hqrPtr);

template
void HQR_Reset(hqrEntryStruct<char>* hqrPtr);

template
void configureHqrHero(hqrEntryStruct<char>* hqrPtr, const char* name);

/// body
template hqrEntryStruct<sBody>* HQR_InitRessource(const char* name, int size, int numEntries);
template sBody* HQR_Get(hqrEntryStruct<sBody>* hqrPtr, int index);
template void HQR_Free(hqrEntryStruct<sBody>* hqrPtr);
template void configureHqrHero(hqrEntryStruct<sBody>* hqrPtr, const char* name);

/// anim
template hqrEntryStruct<sAnimation>* HQR_InitRessource(const char* name, int size, int numEntries);
template sAnimation* HQR_Get(hqrEntryStruct<sAnimation>* hqrPtr, int index);
template void HQR_Free(hqrEntryStruct<sAnimation>* hqrPtr);
template void HQR_Reset(hqrEntryStruct<sAnimation>* hqrPtr);
template void configureHqrHero(hqrEntryStruct<sAnimation>* hqrPtr, const char* name);

/// hybrids
template hqrEntryStruct<sHybrid>* HQR_InitRessource(const char* name, int size, int numEntries);
template sHybrid* HQR_Get(hqrEntryStruct<sHybrid>* hqrPtr, int index);
template void HQR_Free(hqrEntryStruct<sHybrid>* hqrPtr);
