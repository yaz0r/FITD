#include "common.h"

hqrSubEntryStruct* quickFindEntry(int index, int numMax, hqrSubEntryStruct* ptr) // no RE. Original was probably faster
{
    int i;

    for(i=0;i<numMax;i++)
    {
        if((ptr[i].key == index) && ptr[i].ptr)
        {
            return(&ptr[i]);
        }
    }

    return(NULL);
}

hqrEntryStruct* HQR_InitRessource(const char* name, int size, int numEntries)
{
    int i;
    hqrEntryStruct* dest;

    dest = (hqrEntryStruct*)malloc(sizeof(hqrEntryStruct));

    if(!dest)
        return NULL;

    numEntries = 2000;

    strcpy(dest->string,"        ");
    strncpy(dest->string,name,8);

    dest->sizeFreeData = size;
    dest->maxFreeData = size;
    dest->numMaxEntry = numEntries;
    dest->numUsedEntry = 0;
    dest->entries = (hqrSubEntryStruct*)malloc(numEntries*sizeof(hqrSubEntryStruct));

    for(i=0;i<numEntries;i++)
    {
        dest->entries[i].ptr = NULL;
    }

    return(dest);
}

int HQ_Malloc(hqrEntryStruct* hqrPtr,int size)
{
    hqrSubEntryStruct* dataPtr1;
    hqrSubEntryStruct* dataPtr2;
    int key;
    int entryNum;

    if(hqrPtr->sizeFreeData<size)
        return(-1);

    entryNum = hqrPtr->numUsedEntry;

    dataPtr1 = dataPtr2 = hqrPtr->entries;

    key = hqrKeyGen;

    dataPtr1[entryNum].key = key;

    //  dataPtr1[entryNum].offset = hqrPtr->maxFreeData - hqrPtr->sizeFreeData;
    dataPtr1[entryNum].size = size;
	dataPtr1[entryNum].ptr = (char*)malloc(size);

    hqrPtr->numUsedEntry++;
    hqrPtr->sizeFreeData -= size;

    hqrKeyGen++;

    return(key);
}

char* HQ_PtrMalloc(hqrEntryStruct* hqrPtr, int index)
{
    hqrSubEntryStruct* ptr;
    hqrSubEntryStruct* dataPtr;

    if(index<0)
        return NULL;

    dataPtr = hqrPtr->entries;

    ptr = quickFindEntry(index, hqrPtr->numUsedEntry, dataPtr);

    if(!ptr)
        return NULL;

    return(ptr->ptr);
}

void moveHqrEntry(hqrEntryStruct* hqrPtr, int index)
{
    /*  hqrSubEntryStruct* hqrSubPtr = (hqrSubEntryStruct*)(((char*)hqrPtr)+sizeof(hqrEntryStruct));
    hqrSubEntryStruct* hqrSubPtr2 = hqrSubPtr;

    int size = hqrSubPtr[index].size;

    if(hqrPtr->numUsedEntry - 1 > index ) //if not last entry
    {
    char* dest = hqrPtr->dataPtr + hqrSubPtr2[index].offset;
    char* src = dest + size;

    memcpy(dest,src,hqrPtr->dataPtr + hqrPtr->maxFreeData - src);

    dest = (char*)&hqrSubPtr2[index];
    src = (char*)&hqrSubPtr2[index+1];
    memcpy(dest,src,hqrPtr->numMaxEntry-(index+1) * sizeof(hqrSubEntryStruct));
    }*/

    int size = hqrPtr->entries[index].size;

    free(hqrPtr->entries[index].ptr);

    hqrPtr->numUsedEntry --;
    hqrPtr->sizeFreeData += size;
}

std::vector<sAnimation*> vAnimations;

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
        pFrame->m_animStep[0] = READ_LE_S16(animPtr); animPtr += 2;
        pFrame->m_animStep[1] = READ_LE_S16(animPtr); animPtr += 2;
        pFrame->m_animStep[2] = READ_LE_S16(animPtr); animPtr += 2;

        pFrame->m_groups.resize(pAnimation->m_numGroups);
        for (int i = 0; i < pAnimation->m_numGroups; i++)
        {
            sGroupState* pGroup = &pFrame->m_groups[i];

            pGroup->m_type = READ_LE_S16(animPtr); animPtr += 2;
            pGroup->m_delta[0] = READ_LE_S16(animPtr); animPtr += 2;
            pGroup->m_delta[1] = READ_LE_S16(animPtr); animPtr += 2;
            pGroup->m_delta[2] = READ_LE_S16(animPtr); animPtr += 2;
        }
    }

    vAnimations.push_back(pAnimation);
    return pAnimation;
}

std::vector<sBody*> vBodies;

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
                newBody->m_groups[i].m_state.m_delta[0] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta[1] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta[2] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_rotateDelta[0] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_rotateDelta[1] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_rotateDelta[2] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                bodyBuffer += 2; // padding?
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
                newBody->m_groups[i].m_state.m_delta[0] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta[1] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
                newBody->m_groups[i].m_state.m_delta[2] = READ_LE_S16(bodyBuffer); bodyBuffer += 2;
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
            newBody->m_primitives[i].m_subType = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
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
            newBody->m_primitives[i].m_subType = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            break;
        case primTypeEnum_Point:
        case primTypeEnum_BigPoint:
        case primTypeEnum_Zixel:
            newBody->m_primitives[i].m_subType = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_points.resize(1);
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            break;
        case primTypeEnum_Sphere:
            newBody->m_primitives[i].m_subType = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
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
            newBody->m_primitives[i].m_subType = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
            for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++)
            {
                newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6; bodyBuffer += 2;
            }
            break;
        case processPrim_PolyTexture9:
        case processPrim_PolyTexture10:
            newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer)); bodyBuffer += 1;
            newBody->m_primitives[i].m_subType = READ_LE_U8(bodyBuffer); bodyBuffer += 1;
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

    vBodies.push_back(newBody);
    return newBody;
}

char* HQR_Get(hqrEntryStruct* hqrPtr, int index)
{
    hqrSubEntryStruct* foundEntry;

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
        /*    int size;
        unsigned int time;
        char* ptr;

        freezeTime();
        size = getPakSize(hqrPtr->string,index);

        if(size>=hqrPtr->maxFreeData)
        {
        theEnd(1,hqrPtr->string);
        }

        time = timer;

        foundEntry = hqrSubPtr;

        while(size>hqrPtr->sizeFreeData || hqrPtr->numUsedEntry>= hqrPtr->numMaxEntry)
        {
        int bestEntry = 0;
        unsigned int bestTime = 0;
        int entryIdx = 0;

        for(entryIdx = 0; entryIdx<hqrPtr->numUsedEntry; entryIdx++)
        {
        if(time - foundEntry[entryIdx].lastTimeUsed > bestTime)
        {
        bestTime = time - foundEntry[entryIdx].lastTimeUsed;
        bestEntry = entryIdx;
        }
        }

        moveHqrEntry(hqrPtr,bestEntry);
        }

        ptr = hqrPtr->dataPtr + (hqrPtr->maxFreeData - hqrPtr->sizeFreeData);

        if(!loadPakToPtr(hqrPtr->string,index,ptr))
        {
        theEnd(1,hqrPtr->string);
        }

        hqrVar1 = 1;

        foundEntry[hqrPtr->numUsedEntry].key = index;
        foundEntry[hqrPtr->numUsedEntry].lastTimeUsed = timer;
        foundEntry[hqrPtr->numUsedEntry].offset = hqrPtr->maxFreeData - hqrPtr->sizeFreeData;
        foundEntry[hqrPtr->numUsedEntry].size = size;

        hqrPtr->numUsedEntry++;
        hqrPtr->sizeFreeData -= size;

        unfreezeTime();*/

        int size;
        unsigned int time;
        char* ptr;
        int i;

        freezeTime();
        size = getPakSize(hqrPtr->string,index);

		if(size == 0)
			return NULL;

        if(size>=hqrPtr->maxFreeData)
        {
            fatalError(1,hqrPtr->string);
        }

        time = timer;

        for(i=0;i<hqrPtr->numMaxEntry;i++)
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
        foundEntry->ptr = (char*)malloc(size);

        ptr = foundEntry->ptr;

        LoadPak(hqrPtr->string,index,foundEntry->ptr);

        hqrPtr->numUsedEntry++;
        hqrPtr->sizeFreeData -= size;

        unfreezeTime();

        return(ptr);
    }
}

hqrEntryStruct* HQR_Init(int size,int numEntry)
{
    int i;
    hqrEntryStruct* dest;
    char* dest2;

    ASSERT(size > 0);
    ASSERT(numEntry > 0);

    dest = (hqrEntryStruct*)malloc(sizeof(hqrEntryStruct));

    numEntry = 2000;

    ASSERT_PTR(dest);

    if(!dest)
        return NULL;

    dest2 = (char*)malloc(size);

    ASSERT_PTR(dest2);

    if(!dest2)
        return NULL;

    strcpy(dest->string,"_MEMORY_");

    dest->sizeFreeData = size;
    dest->maxFreeData = size;
    dest->numMaxEntry = numEntry;
    dest->numUsedEntry = 0;
    dest->entries = (hqrSubEntryStruct*)malloc(numEntry*sizeof(hqrSubEntryStruct));

    for(i=0;i<numEntry;i++)
    {
        dest->entries[i].ptr = NULL;
    }

    return(dest);
}

void HQR_Reset(hqrEntryStruct* hqrPtr)
{
    int i;
    hqrPtr->sizeFreeData = hqrPtr->maxFreeData;
    hqrPtr->numUsedEntry = 0;

    if (hqrPtr == listBody)
    {
        for (int i = 0; i < vBodies.size(); i++)
        {
            delete vBodies[i];
        }
        vBodies.resize(0);
    }

    for(i=0;i<hqrPtr->numMaxEntry;i++)
    {
        if(hqrPtr->entries[i].ptr)
            free(hqrPtr->entries[i].ptr);

        hqrPtr->entries[i].ptr = NULL;
    }

}

void HQR_Free(hqrEntryStruct* hqrPtr)
{
    int i;

    if(!hqrPtr)
        return;

    if (hqrPtr == listBody)
    {
        for (int i = 0; i < vBodies.size(); i++)
        {
            delete vBodies[i];
        }
        vBodies.clear();
    }

    for(i=0;i<hqrPtr->numMaxEntry;i++)
    {
        if(hqrPtr->entries[i].ptr)
            free(hqrPtr->entries[i].ptr);
    }

    free(hqrPtr);
}

sBody* getBodyFromPtr(void* ptr)
{
    for (int i = 0; i < vBodies.size(); i++)
    {
        if (vBodies[i]->m_raw == ptr)
        {
            return vBodies[i];
        }
    }

    return createBodyFromPtr(ptr);
}

sAnimation* getAnimationFromPtr(void* ptr)
{
    for (int i = 0; i < vAnimations.size(); i++)
    {
        if (vAnimations[i]->m_raw == ptr)
        {
            return vAnimations[i];
        }
    }

    return createAnimationFromPtr(ptr);
}
