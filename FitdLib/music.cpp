#include "common.h"

bool g_gameUseCDA = false;

int musicVolume = 0x7F;

unsigned int musicChrono;

typedef int(*musicDrvFunctionType)(void * ptr);

#ifndef AITD_UE4
struct FM_OPL* virtualOpl;
#endif

char OPLinitialized = 0;

#define OPL_INTERNAL_FREQ    3579545

void callMusicUpdate(void);

struct channelTable2Element
{
    u16 index;
    struct channelTable2Element *var2;
    u16 var4;
    u8* dataPtr;
    u8* commandPtr;
    s16 varE;
    u16 var10;
    u8 var12;
    u16 var13;
    u16 var15;
    u8 var17;
    u16 var18;
    u8 var1A;
    u8 var1B;
    u8 var1C;
    u8 var1D;
    u8 var1E;
};

typedef struct channelTable2Element channelTable2Element;

typedef void(*musicCommandType)(channelTable2Element* entry, int param,u8* ptr);

struct channelTableElement
{
    u16 var0;
    u16 var2;
    u8 var4;
    u8 var5;
    u8 var6;
    u8 var7;
    u16 var8;
};

typedef struct channelTableElement channelTableElement;

channelTableElement channelDataTable[11] = 
{
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF},
    {0xFFFF, 0x40, 0xFF, 0xFF, 0xFF, 0x9C, 0xFFFF}
};

extern channelTable2Element channelTable3[11];
extern channelTable2Element channelTable2[11];

channelTable2Element channelTable3[11] = 
{
    { 0, &channelTable2[0],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 1, &channelTable2[1],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 2, &channelTable2[2],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 3, &channelTable2[3],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 4, &channelTable2[4],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 5, &channelTable2[5],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 6, &channelTable2[6],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 7, &channelTable2[7],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 8, &channelTable2[8],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 9, &channelTable2[9],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    {10,&channelTable2[10],0x8040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0}
};

channelTable2Element channelTable2[11] = 
{
    { 0, &channelTable3[0],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 1, &channelTable3[1],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 2, &channelTable3[2],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 3, &channelTable3[3],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 4, &channelTable3[4],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 5, &channelTable3[5],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 6, &channelTable3[6],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 7, &channelTable3[7],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 8, &channelTable3[8],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    { 9, &channelTable3[9],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
    {10,&channelTable3[10],0x0040,NULL,NULL,0,0,0,0,0,0,0,0x7F,1,1,0x7F,0},
};

unsigned char regBDConf = 0xC0;

unsigned char channelTableMelodic[] =
{
    0x00,
    0x03,
    0x01,
    0x04,
    0x02,
    0x05,
    0x08,
    0x0B,
    0x09,
    0x0C,
    0x0A,
    0x0D,
    0x10,
    0x13,
    0x11,
    0x14,
    0x12,
    0x15,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF
};

unsigned char channelTableRythme[] =
{
    0x00,
    0x03,
    0x01,
    0x04,
    0x02,
    0x05,
    0x08,
    0x0B,
    0x09,
    0x0C,
    0x0A,
    0x0D,
    0x10,
    0x13,
    0x14,
    0xFF,
    0x12,
    0xFF,
    0x15,
    0xFF,
    0x11,
    0xFF
};

unsigned char* channelTable;

// global table is 300 entry long
u16 globTableEntry[300] = {
    0x157,
    0x158,
    0x159,
    0x15A,
    0x15A,
    0x15B,
    0x15C,
    0x15D,
    0x15E,
    0x15F,
    0x15F,
    0x160,
    0x161,
    0x162,
    0x163,
    0x164,
    0x164,
    0x165,
    0x166,
    0x167,
    0x168,
    0x168,
    0x169,
    0x16A,
    0x16B,
    0x16C,
    0x16D,
    0x16D,
    0x16E,
    0x16F,
    0x170,
    0x171,
    0x172,
    0x173,
    0x174,
    0x174,
    0x175,
    0x176,
    0x177,
    0x178,
    0x179,
    0x17A,
    0x17B,
    0x17B,
    0x17C,
    0x17D,
    0x17E,
    0x17F,
    0x180,
    0x181,
    0x181,
    0x183,
    0x183,
    0x184,
    0x185,
    0x186,
    0x187,
    0x188,
    0x189,
    0x18A,
    0x18B,
    0x18C,
    0x18D,
    0x18E,
    0x18E,
    0x18F,
    0x190,
    0x191,
    0x192,
    0x193,
    0x194,
    0x195,
    0x196,
    0x197,
    0x198,
    0x198,
    0x19A,
    0x19A,
    0x19B,
    0x19C,
    0x19D,
    0x19F,
    0x19F,
    0x1A0,
    0x1A1,
    0x1A2,
    0x1A3,
    0x1A4,
    0x1A5,
    0x1A6,
    0x1A7,
    0x1A8,
    0x1A9,
    0x1AA,
    0x1AB,
    0x1AC,
    0x1AD,
    0x1AE,
    0x1AF,
    0x1B0,
    0x1B1,
    0x1B2,
    0x1B3,
    0x1B4,
    0x1B5,
    0x1B6,
    0x1B7,
    0x1B8,
    0x1B9,
    0x1BA,
    0x1BB,
    0x1BC,
    0x1BD,
    0x1BF,
    0x1C0,
    0x1C1,
    0x1C2,
    0x1C3,
    0x1C4,
    0x1C5,
    0x1C6,
    0x1C7,
    0x1C8,
    0x1C9,
    0x1CA,
    0x1CB,
    0x1CC,
    0x1CD,
    0x1CE,
    0x1CF,
    0x1D0,
    0x1D2,
    0x1D3,
    0x1D4,
    0x1D5,
    0x1D6,
    0x1D7,
    0x1D8,
    0x1D9,
    0x1DA,
    0x1DB,
    0x1DD,
    0x1DE,
    0x1DF,
    0x1E0,
    0x1E1,
    0x1E2,
    0x1E3,
    0x1E4,
    0x1E5,
    0x1E6,
    0x1E8,
    0x1E9,
    0x1EA,
    0x1EB,
    0x1EC,
    0x1ED,
    0x1EF,
    0x1F0,
    0x1F1,
    0x1F2,
    0x1F3,
    0x1F4,
    0x1F6,
    0x1F7,
    0x1F8,
    0x1F9,
    0x1FA,
    0x1FB,
    0x1FD,
    0x1FE,
    0x1FF,
    0x200,
    0x201,
    0x202,
    0x203,
    0x205,
    0x206,
    0x207,
    0x208,
    0x20A,
    0x20B,
    0x20C,
    0x20D,
    0x20F,
    0x210,
    0x211,
    0x212,
    0x214,
    0x215,
    0x216,
    0x217,
    0x219,
    0x21A,
    0x21B,
    0x21C,
    0x21D,
    0x21F,
    0x220,
    0x221,
    0x222,
    0x224,
    0x225,
    0x226,
    0x227,
    0x229,
    0x22A,
    0x22C,
    0x22D,
    0x22E,
    0x22F,
    0x231,
    0x232,
    0x234,
    0x235,
    0x236,
    0x237,
    0x239,
    0x23A,
    0x23B,
    0x23C,
    0x23E,
    0x23F,
    0x241,
    0x242,
    0x243,
    0x245,
    0x246,
    0x247,
    0x248,
    0x24A,
    0x24B,
    0x24D,
    0x24E,
    0x250,
    0x251,
    0x252,
    0x254,
    0x255,
    0x257,
    0x258,
    0x259,
    0x25B,
    0x25C,
    0x25E,
    0x25F,
    0x260,
    0x262,
    0x263,
    0x264,
    0x266,
    0x267,
    0x269,
    0x26A,
    0x26B,
    0x26D,
    0x26E,
    0x270,
    0x271,
    0x273,
    0x274,
    0x276,
    0x277,
    0x279,
    0x27A,
    0x27C,
    0x27D,
    0x27F,
    0x280,
    0x282,
    0x283,
    0x285,
    0x286,
    0x288,
    0x289,
    0x28A,
    0x28C,
    0x28D,
    0x28F,
    0x291,
    0x292,
    0x294,
    0x295,
    0x297,
    0x299,
    0x29A,
    0x29C,
    0x29D,
    0x29F,
    0x2A0,
    0x2A2,
    0x2A3,
    0x2A5,
    0x2A7,
    0x2A8,
    0x2AA,
    0x2AB,
    0x2AD,
    0x2AF,
    0x2B0,
};

u16* globTable[13] = 
{
    &globTableEntry[0],
    &globTableEntry[25],
    &globTableEntry[50],
    &globTableEntry[75],
    &globTableEntry[100],
    &globTableEntry[125],
    &globTableEntry[150],
    &globTableEntry[175],
    &globTableEntry[200],
    &globTableEntry[225],
    &globTableEntry[250],
    &globTableEntry[275],
};

unsigned char musicParam1 = 0;

u8* currentMusicPtr = NULL;
u8* currentMusicPtr2 = NULL;
u8* currentMusicPtr3 = NULL;
u8 generalVolume = 0;

void sendAdlib(int regIdx, int value)
{
    YM3812Write(0,0,regIdx);
    YM3812Write(0,1,value);
}

#define musicSync 1500
int musicTimer = 0;
int nextUpdateTimer = musicSync;

int musicUpdate(void *udata, uint8 *stream, int len)
{
    if(OPLinitialized)
    {
        int fillStatus = 0;

        while(fillStatus < len)
        {
            int timeBeforNextUpdate = nextUpdateTimer - musicTimer;

            if(timeBeforNextUpdate > (len - fillStatus))
            {
                timeBeforNextUpdate = len-fillStatus;
            }

            if(timeBeforNextUpdate) // generate
            {
                YM3812UpdateOne(0,(int16*)(stream+fillStatus),(timeBeforNextUpdate)/2);
                fillStatus+=timeBeforNextUpdate;
                musicTimer+=timeBeforNextUpdate;
            }

            if(musicTimer == nextUpdateTimer)
            {
                callMusicUpdate();

                nextUpdateTimer += musicSync;
            }
        }
    }

    return 0;
}

void createDefaultChannel(int index)
{
    channelDataTable[index].var5 = 0xFF;
    channelDataTable[index].var4 = 0xFF;
    channelDataTable[index].var2 = 0x40;
    channelDataTable[index].var0 = 0xFFFF;
    channelDataTable[index].var7 = 0x9C;
    channelDataTable[index].var8 = 0xFFFF;
}

void resetChannelFrequency(int channelIdx)
{
    sendAdlib(0xA0+channelIdx,0);
    sendAdlib(0xB0+channelIdx,0);
}

void setupChannelFrequency(int channelIdx, int cl, int dx,int bp)
{
    u16* di;
    u16 frequency;
    u8 frequencyLow;
    u8 frequencyHigh;

    u8 blockNumber;

    if(!(bp&0x8000))
    {
        sendAdlib(0xB0+channelIdx, 0);
    }

    di = globTable[cl&0xF];

    if(bp & 0x80)
    {
        // assert(0);
    }

    if(cl & 0x80)
    {
        dx = 0x40;
    }

    frequency = di[bp&0xFF];

    frequencyLow = frequency&0xFF;

    sendAdlib(0xA0+channelIdx,frequencyLow);

    blockNumber = (cl&0x70)>>2;

    frequencyHigh = ((frequency>>8)&0x3)|blockNumber;

    if(!(dx&0x40))
    {
        frequencyHigh |= 0x20; // set key on
    }

    sendAdlib(0xB0+channelIdx,frequencyHigh);
}

int musicStart(void* dummy)
{
    int i;

    sendAdlib(1,0x20);
    sendAdlib(8,0);
    sendAdlib(0xBD,regBDConf);

    for(i=0;i<18;i++)
    {
        sendAdlib(0x60+channelTableMelodic[i],0xFF);
        sendAdlib(0x80+channelTableMelodic[i],0xFF);
    }

    for(i=0;i<9;i++)
    {
        resetChannelFrequency(i);
    }

    for(i=0;i<11;i++)
    {
        createDefaultChannel(i);
    }

    if(!musicParam1)
    {
        resetChannelFrequency(6);
        setupChannelFrequency(6,0,0x40,0);

        resetChannelFrequency(7);
        setupChannelFrequency(7,7,0x40,0);

        resetChannelFrequency(8);
        setupChannelFrequency(8,0,0x40,0);
    }

    return 0;
}

int musicLoad(void* ptr)
{
    int i;
    u8 flag1;

    u8* musicPtr = (u8*)ptr;

    channelTable = channelTableMelodic;

    flag1 = musicPtr[0x3C] & 0xC0;
    musicParam1 = musicPtr[0x3D];

    if(!musicParam1)
    {
        flag1 |= 0x20;
        channelTable = channelTableRythme;
    }

    regBDConf = flag1;

    for(i=0;i<11;i++)
    {
        unsigned long int offset;

        offset = READ_LE_U32(musicPtr + i*4 + 8);

        if(offset)
        {
            channelTable2[i].dataPtr = musicPtr + offset;
        }
        else
        {
            channelTable2[i].dataPtr = NULL;
        }

        channelTable2[i].var4 |= 0x40;
    }

    currentMusicPtr = musicPtr + READ_LE_U16(musicPtr + 0x34);

    return 0;
}

int initialialize(void* dummy)
{
    int i;

    //OPLBuildTables(FMOPL_ENV_BITS_HQ, FMOPL_EG_ENT_HQ);

    YM3812Init(1,OPL_INTERNAL_FREQ,44100);
    /*  virtualOpl = OPLCreate(OPL_TYPE_YM3812, OPL_INTERNAL_FREQ, 44100);

    if(!virtualOpl)
    return 0; */

    for(i=0;i<11;i++)
    {
        channelTable2[i].var4 |= 0x20;
        channelTable2[i].var2->var4 |= 0x20;

        createDefaultChannel(i);
    }

    musicStart(NULL);

    OPLinitialized = 1;

    return 0;
}

int getSignature(void* dummy)
{
    return 0;
}

void commandNop(channelTable2Element* entry, int param,u8* ptr)
{
}

void command0(channelTable2Element* entry, int param,u8* ptr)
{
    entry->var4 |= 2;

    if(entry->var4 & 0x20)
    {
        return;
    }

    entry->var4 |= 0x40;

    if(!(entry->var4 & 0x8000))
    {
        return;
    }

    entry->var2->var4 &= 0xFFFB;
}

void command1(channelTable2Element* entry, int param,u8* ptr)
{
    u16 ax;

    ax = READ_LE_U16(ptr-1);

    entry->var10 = entry->varE = ax+entry->var13;

    entry->commandPtr ++;
}

void command2(channelTable2Element* entry, int param,u8* ptr)
{
    entry->var18++;
    entry->var15 = param;
}

void command3(channelTable2Element* entry, int param,u8* ptr)
{
    entry->var12 = param;
}

void command4(channelTable2Element* entry, int param,u8* ptr)
{
    entry->var1E = param;
}

void command5(channelTable2Element* entry, int param,u8* ptr)
{
    entry->var17 = param;
}

void command6(channelTable2Element* entry, int param,u8* ptr)
{
    assert(0);
}

musicCommandType musicCommandTable[10] = 
{
    command0,
    command1,
    command2,
    command3,
    command4,
    command5,
    command6,
    commandNop,
    commandNop,
    commandNop,
};

void executeMusicCommand(channelTable2Element* entry)
{
    u16 opcode;

    if(entry->var4&0x40)
        return;

    if(entry->var4&0x02) // start channel
    {
        entry->commandPtr = entry->dataPtr;
        entry->var4 &= 0xFFFD;
        entry->var18 = 0;
    }
    else
    {
        if(entry->var1A != entry->var1D)
        {
            assert(0);
        }

        entry->varE--; // voice delay

        if(entry->varE<=0)
        {
            entry->varE = entry->var10;
        }
        else
        {
            return;
        }
    }

    do
    {
        opcode = READ_LE_U16(entry->commandPtr);
        entry->commandPtr+=2;

        ASSERT(musicCommandTable[opcode&0x7F]);
        ASSERT((opcode&0x7F)<=10);

        musicCommandTable[opcode&0x7F](entry,opcode>>8,entry->commandPtr);
    }while(!(opcode&0x80));
}

u8 smallTable[] = { 0x10, 8, 4, 2, 1 };

void applyDirectFrequency(int index, int param1, int param2, int param3)
{
    if(musicParam1)
    {
        setupChannelFrequency(index,param1,param2,param3);
        return;
    }
    else
    {
        int ah;

        if(index<6)
        {
            setupChannelFrequency(index,param1,param2,param3);
            return;
        }

        if(index==6)
        {
            setupChannelFrequency(index,param1,0x40,param3);
        }
        else
            if(index==8 && !(param1&0x80))
            {
                int indexBackup = index;
                int param1Backup = param1;

                setupChannelFrequency(8,param1,0x40,param3);
                {
                    int al = param1&0x70;

                    index = 7;

                    param1 &= 0xF;
                    param1+=7;

                    if(param1 >= 0xC)
                    {
                        param1 -= 0xC;

                        if(al != 0x70)
                            al += 0x10;
                    }

                    setupChannelFrequency( index, param1, 0x40,param3);

                }

                param1 = param1Backup;
                index = indexBackup;
            }

            ah = (~(smallTable[index-6])) & regBDConf;

            sendAdlib(0xBD,ah);

            if(!(param2 & 0x40) && !(param1 & 0x80))
            {
                ah |= smallTable[index-6];

                sendAdlib(0xBD,ah);
            }

            regBDConf = ah;
    }
}

unsigned char smallData2[] =
{
    0,
    1, 
    2, 
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    3 ,
    4 ,
    5 ,
    0xFF,  
    0xFF,  
    0xFF,  
    0xFF,  
    0xFF,  
    6 ,
    7 ,
    8 ,
    0xFF,  
    0xFF,  
    0xFF,
};


void configChannel(u8 value, u8* data)
{
    if(smallData2[value] != 0xFF)
    {
        sendAdlib(0xC0 + smallData2[value], data[2]);
    }

    sendAdlib(0x60 + value, data[4]); // Attack Rate  Decay Rate
    sendAdlib(0x80 + value, data[5]); // Sustain Level  Release Rate
    sendAdlib(0x20 + value, data[1]); // Tremolo  Vibrato   Sustain   KSR   Frequency Multiplication Factor
    sendAdlib(0xE0 + value, data[3]); //  Waveform Select
}

void changeOuputLevel(u8 value, u8* data,int bp)
{
    int keyScaleLevel;
    int outputLevel;

    if(value == 0xFF)
        return;

    data++;

    outputLevel = (*data)&0x3F;

    outputLevel = 0x3F - ((((outputLevel*bp)*2) + 0x7F)/0xFE);

    ASSERT((outputLevel & 0x3F) == outputLevel);

    keyScaleLevel = data[0]&0xC0;

    sendAdlib(0x40+value,(data[0]&0xC0) | (outputLevel&0x3F));
}

void applyMusicCommandToOPL(channelTable2Element* element2, channelTableElement* element)
{
    char al;
    u16 dx;
    u16 bp;

    u8 operator1;
    u8 operator2;

    if((element2->var4 & 0x40) != element->var2)
    {
        element->var2 = element2->var4 & 0x40;

        if(element2->var4 & 0x40)
        {
            applyDirectFrequency(element2->index,element2->var15 | 0x80, 0x40, element2->var17);
            createDefaultChannel(element2->index);
            return;
        }
    }

    if(element2->var4 & 0x40)
        return;

    if((element->var8 & 1) || (element->var8 != (element2->var4&0x8000)))
    {
        element->var8 = element2->var4&0x8000;
        element->var5 = 0xFF;
        element->var4 = 0xFF;
        element->var0 = 0xFFFF;
        element->var7 = 0x9C;
    }

    operator1 = channelTable[element2->index*2];
    operator2 = channelTable[(element2->index*2)+1];

    if(operator1 == 0xFF && operator2 == 0xFF) // do we have an operator ?
        return;

    if(element2->var12 != element->var4) // change channel main config
    {
        element->var4 = element2->var12;

        configChannel(operator1,(currentMusicPtr2+0xD*element2->var12)+1);

        if(operator2 != 0xFF)
        {
            configChannel(operator2,(currentMusicPtr2+0xD*element2->var12)+7);
        }

        element->var5 = 0xFF;
    }

    // Ouput level handling

    al = element2->var1D - element2->var1E;

    if(al < 0)
        al = 0;

    if(element->var5 != al)
    {
        int dx;

        element->var5 = al;

        dx = element2->var1D;

        if(operator2==0xFF)
        {
            dx = element->var5;
        }

        changeOuputLevel(operator1,currentMusicPtr2+0xD*element2->var12,dx);

        if(operator2 != 0xFF)
        {
            changeOuputLevel(operator2,(currentMusicPtr2+0xD*element2->var12)+6,element->var5);
        }
    }

    ////

    bp = dx = element2->var17;

    if(element2->var17 != element->var7)
    {
        element->var7 = element2->var17;

        if(element2->var15 == element->var0)
        {
            bp |= 0x8000;
        }
    }
    else
    {
        if(element2->var15 == element->var0)
            return;
    }

    element->var0 = element2->var15 = element2->var15 | 0x8000;

    applyDirectFrequency(element2->index,element->var0&0xFF,element2->var4,bp);
}

int update(void* dummy)
{
    int i;

    channelTable2Element* si;

    if(generalVolume & 0xFF)
    {
        return 0;
    }

    for(i=0;i<11;i++)
    {
        currentMusicPtr2 = currentMusicPtr;

        executeMusicCommand(&channelTable2[i]);

        si = &channelTable2[i];

        if(channelTable2[i].var4 & 4)
        {
            currentMusicPtr2 = currentMusicPtr3;

            si = channelTable2[i].var2;
            executeMusicCommand(channelTable2[i].var2);
        }

        applyMusicCommandToOPL(si,&channelDataTable[i]);
    }

    return 0;
}

int musicFade(void * param)
{
    int i;
    int cx;
    int si;
    int dx;
    int bp;

    cx = ((int*)param)[0];
    si = ((int*)param)[1];
    dx = ((int*)param)[2];

    bp = si;

    si = -1;

    if(!bp)
        bp = 0x7FF;

    for(i=0;i<11;i++)
    {
        //  if((bp&i))
        {
            if(channelTable2[i].dataPtr)
            {
                if(dx & 0x100)
                {
                    assert(0);
                }

                if(dx & 0x40)
                {
                    if(!(channelTable2[i].var4&0x40))
                        channelTable2[i].var4|=0x40;
                }

                if(dx & 0x80) // start all
                {
                    channelTable2[i].var4 = 0x40;
                    cx &= 0x7F;

                    channelTable2[i].var1D = cx;
                    channelTable2[i].var1A = cx;

                    channelTable2[i].var1E = 0;

                    createDefaultChannel(channelTable2[i].index);

                    channelTable2[i].var4 = 2;
                }

                if(dx & 0x20)
                {
                    assert(0);
                }

                if(dx & 0x2000)
                {
                    assert(0);
                }

                if(dx & 0x8000)
                {
                    channelTable2[i].var1A = cx;
                }

                if(dx & 0x1000)
                {
                    assert(0);
                }

                if(dx & 0x10) // still running ?
                {
                    if(!(dx & 0x2000))
                    {
                        if(!(channelTable2[i].var4 & 0x40))
                        {
                            if(si < channelTable2[i].var18)
                                si = channelTable2[i].var18;
                        }
                    }
                    else
                    {
                        if(channelTable2[i].var1D != cx)
                        {
                            si = 0;
                        }
                    }
                }
            }
        }
    }

    return si;
}

musicDrvFunctionType musicDrvFunc[14]=
{
    update,
    initialialize,
    musicStart,
    musicLoad,
    NULL,
    musicFade,
    NULL,
    NULL,
    getSignature,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

int callMusicDrv(int commandArg,void* ptr)
{
    if(!musicDrvFunc[commandArg])
    {
        assert(0);
    }

    return musicDrvFunc[commandArg](ptr);
}

int initMusicDriver(void)
{
    callMusicDrv(1,NULL);

    return callMusicDrv(8,NULL);
}

void loadMusic(int param, char* musicPtr)
{
    callMusicDrv(3,musicPtr);
    callMusicDrv(2,NULL);
}

int fadeParam[3];

int fadeMusic(int param1, int param2, int param3)
{
    fadeParam[0] = param1;
    fadeParam[1] = param2;
    fadeParam[2] = param3;

    return callMusicDrv(5,&fadeParam);
}

void playMusic(int musicNumber)
{
	if(currentMusic == musicNumber)
		return;

	currentMusic = musicNumber;

	int trackNumber = musicNumber;
	
	if(g_gameId == AITD2)
	{
		trackNumber = AITD2MusicToTrackMapping[musicNumber];
	}
	
	if(osystem_playTrack(trackNumber))
		return;

    //  if(musicEnabled)
    {
        //if(currentMusic != musicNumber)
        {
            char* musicPtr;

            currentMusic = musicNumber;

            if(musicNumber>=0)
            {
                fadeMusic(0,0,0x40);

                musicPtr = HQR_Get(listMus,musicNumber);

				if(musicPtr)
				{
					loadMusic(0,musicPtr);

					fadeMusic(musicVolume,0,0x80);
#ifndef TARGET_OS_IPHONE
					osystem_playAdlib();
#endif
				}
            }
        }
    }
}

int updateLoop = 0;

int oldTimer = 0;

void callMusicUpdate(void)
{
    if(OPLinitialized)
    {
        callMusicDrv(0,NULL);
    }
}

void destroyMusicDriver(void)
{
    YM3812Shutdown();
}
