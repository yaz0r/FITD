#ifndef _TATOU_H_
#define _TATOU_H_

int make3dTatou(void);
//////////////// to mode
void paletteFill(void* palette, unsigned char r, unsigned char g, unsigned b);
void setPalette(void* palette);
void startChrono(unsigned int* chrono);
int evalChrono(unsigned int* chrono);
void process_events( void );
void copyPalette(unsigned char* source, unsigned char* dest);
void copyToScreen(void* source, void* dest);
void Rotate(unsigned int x, unsigned int y, unsigned int z, int* xOut, int* yOut);
void FadeInPhys(int var1,int var2);
void fadeOut(int var1, int var2);
void playSound(int num);
void setCameraTarget(int x,int y,int z,int alpha,int beta,int gamma,int time);
//////////////
#endif
