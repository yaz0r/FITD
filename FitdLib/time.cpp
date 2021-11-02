#include "common.h"

int timerSaved = false;
unsigned int timerSavedValue = 0;

void freezeTime(void)
{
    if(timerSaved==0)
    {
        timerSavedValue = timeGlobal;
    }
	timerSaved++;
}

void unfreezeTime(void)
{
	assert(timerSaved);
	timerSaved--;
    if(timerSaved == 0)
    {
        timeGlobal = timerSavedValue;
		timer = timeGlobal;
    }
}
