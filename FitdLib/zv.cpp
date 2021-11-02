#include "common.h"

void getZvCube(char* bodyPtr, ZVStruct* zvPtr)
{
    s16* ptr;

    ptr = (s16*)(bodyPtr+2);

    zvPtr->ZVX1 = *(ptr++);
    zvPtr->ZVX2 = *(ptr++);
    zvPtr->ZVY1 = *(ptr++);
    zvPtr->ZVY2 = *(ptr++);
    zvPtr->ZVZ1 = *(ptr++);
    zvPtr->ZVZ2 = *(ptr++);

    zvPtr->ZVZ2 = zvPtr->ZVX2 = (zvPtr->ZVX2 + zvPtr->ZVZ2)/2;
    zvPtr->ZVX1 = zvPtr->ZVZ1 = -zvPtr->ZVZ2;
}

void getZvNormal(char* bodyPtr, ZVStruct* zvPtr)
{
    s16* ptr;

    ptr = (s16*)(bodyPtr+2);

    zvPtr->ZVX1 = *(ptr++);
    zvPtr->ZVX2 = *(ptr++);
    zvPtr->ZVY1 = *(ptr++);
    zvPtr->ZVY2 = *(ptr++);
    zvPtr->ZVZ1 = *(ptr++);
    zvPtr->ZVZ2 = *(ptr++);
}

void makeDefaultZV(ZVStruct* zvPtr)
{
    zvPtr->ZVX1 = -100;
    zvPtr->ZVX2 = 100;

    zvPtr->ZVY1 = -2000;
    zvPtr->ZVY2 = 0;

    zvPtr->ZVZ1 = -100;
    zvPtr->ZVZ2 = 100;
}

void getZvMax(char* bodyPtr, ZVStruct* zvPtr)
{
    int x1;
    int x2;
    int z1;
    int z2;

    getZvNormal(bodyPtr,zvPtr);

    x1 = zvPtr->ZVX1;
    x2 = zvPtr->ZVX2;

    z1 = zvPtr->ZVZ1;
    z2 = zvPtr->ZVZ2;

    x2 = - x1 + x2;
    z2 = - z1 + z2;

    if(x2 < z2)
    {
        x2 = z2;
    }

    x2 /= 2;

    zvPtr->ZVX1 = -x2;
    zvPtr->ZVX2 = x2;

    zvPtr->ZVZ1 = -x2;
    zvPtr->ZVZ2 = x2;
}
