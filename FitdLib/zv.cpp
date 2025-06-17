#include "common.h"

void getZvCube(sBody* bodyPtr, ZVStruct* zvPtr)
{
    *zvPtr = bodyPtr->m_zv;
    
    zvPtr->ZVZ2 = zvPtr->ZVX2 = (zvPtr->ZVX2 + zvPtr->ZVZ2)/2;
    zvPtr->ZVX1 = zvPtr->ZVZ1 = -zvPtr->ZVZ2;
}

void GiveZVObjet(sBody* bodyPtr, ZVStruct* zvPtr)
{
    *zvPtr = bodyPtr->m_zv;
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

void getZvMax(sBody* bodyPtr, ZVStruct* zvPtr)
{
    int x1;
    int x2;
    int z1;
    int z2;

    GiveZVObjet(bodyPtr,zvPtr);

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

void zvRotSub(int X, int Y, int Z, int alpha, int beta, int gamma)
{
    if (alpha || beta || gamma)
    {
        setupPointRotate(alpha, beta, gamma);
        pointRotate(X, Y, Z, &animMoveX, &animMoveY, &animMoveZ);
    }
    else
    {
        animMoveX = X;
        animMoveY = Y;
        animMoveZ = Z;
    }
}

void getZvRot(sBody* bodyPtr, ZVStruct* zvPtr, int alpha, int beta, int gamma)
{
    int X1 = 32000;
    int Y1 = 32000;
    int Z1 = 32000;

    int X2 = -32000;
    int Y2 = -32000;
    int Z2 = -32000;

    int i;
    int tempX;
    int tempY;
    int tempZ;

    GiveZVObjet(bodyPtr, zvPtr);

    for (i = 0; i < 8; i++)
    {
        switch (i)
        {
        case 0:
        {
            tempX = zvPtr->ZVX1;
            tempY = zvPtr->ZVY1;
            tempZ = zvPtr->ZVZ1;
            break;
        }
        case 1:
        {
            tempZ = zvPtr->ZVZ2;
            break;
        }
        case 2:
        {
            tempX = zvPtr->ZVX2;
            break;
        }
        case 3:
        {
            tempZ = zvPtr->ZVZ1;
            break;
        }
        case 4:
        {
            tempY = zvPtr->ZVY2;
            break;
        }
        case 5:
        {
            tempX = zvPtr->ZVX1;
            break;
        }
        case 6:
        {
            tempZ = zvPtr->ZVZ2;
            break;
        }
        case 7:
        {
            tempX = zvPtr->ZVX2;
            break;
        }
        }

        zvRotSub(tempX, tempY, tempZ, alpha, beta, gamma);

        if (animMoveX < X1)
            X1 = animMoveX;

        if (animMoveX > X2)
            X2 = animMoveX;

        if (animMoveY < Y1)
            Y1 = animMoveY;

        if (animMoveY > Y2)
            Y2 = animMoveY;

        if (animMoveZ < Z1)
            Z1 = animMoveZ;

        if (animMoveZ > Z2)
            Z2 = animMoveZ;

    }

    zvPtr->ZVX1 = X1;
    zvPtr->ZVX2 = X2;
    zvPtr->ZVY1 = Y1;
    zvPtr->ZVY2 = Y2;
    zvPtr->ZVZ1 = Z1;
    zvPtr->ZVZ2 = Z2;
}
