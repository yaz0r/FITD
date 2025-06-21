#include "common.h"

void AffSpf(int left, int top, int index, char* gfxData)
{
    char* outPtr;
    char* inPtr;

    int width;
    int height;

    int offset;

    int i;
    int j;

    if(g_gameId >= AITD3)
        return;

    outPtr = logicalScreen + top*320 + left;
    inPtr = gfxData + READ_LE_U16(index * 2 + gfxData);

    inPtr +=4;

    width = READ_LE_U16(inPtr);
    inPtr+=2;
    height = READ_LE_U16(inPtr);
    inPtr+=2;

    offset = 320 - width;

    // Prevent overflow
    if(height + top > 200)
    {
        height = top - 200;
    }

    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            *(outPtr++) = *(inPtr++);
        }

        outPtr+=offset;
    }
}

void AffSpfI(int left, int top, int index, char* gfxData)
{
	char* outPtr;
	char* inPtr;

	int width;
	int height;

	int offset;

	int i;
	int j;

	if(g_gameId >= AITD3)
		return;

	outPtr = logicalScreen + top*320 + left;
	inPtr = gfxData + READ_LE_U16(index * 2 + gfxData); // alignement unsafe

	inPtr +=4;

	width = READ_LE_U16(inPtr); // alignement unsafe
	inPtr+=2;
	height = READ_LE_U16(inPtr); // alignement unsafe
	inPtr+=2;

	offset = 320 - width;

	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			char color = *(inPtr++);
			if(color)
			{
				*(outPtr) = color;
			}
			outPtr++;
		}

		outPtr+=offset;
	}
}

void AffBigCadre(int x, int y, int width, int height)
{
    int top;
    int right;
    int left;
    int bottom;

    int currentLeftPosition;
    int currentTopPosition;

    int halfWidth;
    int halfHeight;

    SetClip(0,0,319,199);

    halfWidth = width/2;
    currentLeftPosition = left = x - halfWidth;

    halfHeight = height/2;
    currentTopPosition = top = y - halfHeight;

    right = x + halfWidth;
    bottom = y + halfHeight;

    AffSpf(currentLeftPosition,currentTopPosition,0,PtrCadre); // draw top left corner

    while(1) // draw top bar
    {
        currentLeftPosition += 20;

        if(right - 20 <= currentLeftPosition)
            break;

        AffSpf(currentLeftPosition,currentTopPosition,4,PtrCadre);
    }

    AffSpf(currentLeftPosition,currentTopPosition,1,PtrCadre); // draw top right corner

    currentLeftPosition = left;

    while(1) // draw left bar
    {
        currentTopPosition += 20;

        if(bottom - 20 <= currentTopPosition)
            break;

        AffSpf(currentLeftPosition,currentTopPosition,6,PtrCadre);
    }

    currentLeftPosition = right - 8;
    currentTopPosition = top + 20;

    while(bottom - 20 > currentTopPosition)
    {
        AffSpf(currentLeftPosition,currentTopPosition,7,PtrCadre);

        currentTopPosition += 20;
    }

    currentLeftPosition = left;

    AffSpf(currentLeftPosition,currentTopPosition,2,PtrCadre); // draw bottom left corner

    while(1) // draw bottom bar
    {
        currentLeftPosition += 20;

        if(right-20 <= currentLeftPosition)
            break;

        AffSpf(currentLeftPosition,currentTopPosition+12,5,PtrCadre);
    }

    AffSpf(currentLeftPosition,currentTopPosition,3,PtrCadre); // draw bottom right corner

    AffSpf(x-20,currentTopPosition+12,8,PtrCadre); // draw "in the dark"

    WindowX1 = left + 8;
    WindowY1 = top + 8;
    WindowX2 = right - 9;
    WindowY2 = bottom - 9;

    AffRect(WindowX1,WindowY1,WindowX2,WindowY2,0);
    SetClip(WindowX1,WindowY1,WindowX2,WindowY2);

}
