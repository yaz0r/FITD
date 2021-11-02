// seg 048

#include "common.h"

void setupScreen(void)
{
    logicalScreen = (char*)malloc(64800);

    screenBufferSize = 64800;

    unkScreenVar2 = 3;

    // TODO: remain of screen init

}

void flushScreen(void)
{
    int i;
    int j;

    for(i=0;i<200;i++)
    {
        for(j=0;j<320;j++)
        {
            *(logicalScreen+i*320+j) = 0;
        }
    }
}
