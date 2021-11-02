#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#ifdef FITD_DEBUGGER

////// debug var used in engine
extern bool debuggerVar_drawModelZv;
extern bool debuggerVar_drawCameraCoverZone;
extern bool debuggerVar_noHardClip;
extern bool debuggerVar_topCamera;
extern long int debufferVar_topCameraZoom;

extern bool debuggerVar_useBlackBG;
extern bool debuggerVar_fastForward;
///////////////////////////////

void debugger_enterMainDebug(void);
void debugger_processDebugMenu(void);
void debugger_draw(void);
#endif // INTERNAL_DEBUGGER

#endif