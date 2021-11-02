#ifndef _MUSIC_H_
#define _MUSIC_H_
extern unsigned int musicChrono;
int initMusicDriver(void);

extern "C" {
int musicUpdate(void *udata, uint8 *stream, int len);
void playMusic(int musicNumber);
extern bool g_gameUseCDA;
};

void callMusicUpdate(void);
void destroyMusicDriver(void);
int fadeMusic(int param1, int param2, int param3);
#endif
