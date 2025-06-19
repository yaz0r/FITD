#include "common.h"

int fontHeight = 16;

char* fontVar1 = NULL;
s16 fontSm1 = 0;
s16 fontSm2 = 0x1234;
char* fontVar4 = NULL;
extern char* fontVar4;
char* fontVar5 = NULL;
extern char* fontVar5;
s16 currentFontColor = 0;
extern s16 currentFontColor;
s16 g_fontInterWordSpace = 2;
s16 g_fontInterLetterSpace = 1;
s16 fontSm3 = 18;
s16 fontVar6 = 0;
s16 fontSm7 = 0x1234;
s16 fontSm8 = 0x1234;
s16 fontSm9 = 0x80;

unsigned char flagTable[]= {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void SetFont(char* fontData, int color)
{
    s16 tempDx;
    s16 tempAxFlip;

    fontVar1 = fontData; // fontPtr

    tempDx = READ_LE_S16(fontData); // alignement
    fontData+=2;

    fontSm1 = *(fontData++); // character height
    fontSm2 = *(unsigned char*)(fontData++); // character size

    if(!fontSm2)
    {
        fontSm2 = READ_LE_S16(fontData);
    }

    fontData+=2;

    tempAxFlip = READ_LE_S16(fontData);
    fontData+=2;

    tempAxFlip = ((tempAxFlip & 0xFF) << 8 ) | ((tempAxFlip &0xFF00) >> 8);

    fontVar4 = fontData;

    fontVar5 = fontVar1 + tempAxFlip - (tempDx&0xFF)*2;

    currentFontColor = color;

    fontSm3 = color;
}

void SetFontSpace(int interWordSpace, int interLetterSpace)
{
    g_fontInterWordSpace = interWordSpace;
    g_fontInterLetterSpace = interLetterSpace;
}

int ExtGetSizeFont(u8* string)
{
    int width = 0;
    unsigned char character;

    while((character = *(string++)))
    {
        char* dataPtr;
        u16 data;

        dataPtr = fontVar5 + character*2;
        data = READ_LE_S16(dataPtr);

        data>>=4;

        data &= 0xF;

        if(!data)
        {
            width += g_fontInterWordSpace;
        }

        width += g_fontInterLetterSpace;
        width += data;
    }

    return(width);
}

void PrintFont(int x, int y, char* surface, u8* string)
{
    unsigned char character;

    fontVar6 = x;
    fontSm7 = y;

    while ((character = *(string++)))
    {
        char* dataPtr;
        u16 data;
        u16 dx;

        dataPtr = fontVar5 + character*2;
        data = READ_LE_U16(dataPtr);

        data = ((data & 0xFF)<<8)| ((data&0xFF00)>>8);

        dx = data;

        data >>= 12;

        if(data&0xF) // real character (width != 0)
        {
            char* characterPtr;
            int bp;
            int ch;

            dx &= 0xFFF;

            characterPtr = (dx>>3) + fontVar4;

            fontSm9 = flagTable[dx & 7];

            bp = fontSm7;

            fontSm8 = fontVar6;

            ch;

            for(ch = fontSm1; ch>0; ch--)
            {
                if (bp >= 200)
                    return;
                char* outPtr = (char*)uiLayer.data() + bp * 320 + fontSm8;


                int dh = fontSm9;
                int cl = data&0xF;

                int al = *characterPtr;

                int bx;

                bp++;

                for(bx = 0; cl>0; cl--)
                {
                    if(dh & al)
                    {
                        *(outPtr) = (char)fontSm3;
                    }

                    outPtr++;

                    dh = ((dh>>1) & 0x7F) | ((dh<<7)&0x80);

                    if(dh&0x80)
                    {
                        bx++;
                        al = *(characterPtr + bx);
                    }
                }

                characterPtr += fontSm2;
            }

            fontVar6 += data&0xF;
        }
        else // space character
        {
            fontVar6 += g_fontInterWordSpace;
        }

        fontVar6 += g_fontInterLetterSpace;
    }
}

void SelectedMessage(int x, int y, int index, int color1, int color2)
{
    textEntryStruct* entryPtr;
    u8* textPtr;

    entryPtr = getTextFromIdx(index);

    if(!entryPtr)
        return;

    x -= (entryPtr->width/2); // center

    textPtr = entryPtr->textPtr;

    SetFont(PtrFont,color2);
    PrintFont(x,y+1,logicalScreen,textPtr);

    SetFont(PtrFont,color1);
    PrintFont(x,y,logicalScreen,textPtr);

}

void SimpleMessage(int x, int y, int index, int color)
{
    textEntryStruct* entryPtr = getTextFromIdx(index);

    if(!entryPtr)
        return;

    x -= (entryPtr->width/2); // center

    u8* textPtr = entryPtr->textPtr;

    SetFont(PtrFont,color);

    PrintFont(x,y+1,logicalScreen,textPtr);
}
