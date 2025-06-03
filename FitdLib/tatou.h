#ifndef _TATOU_H_
#define _TATOU_H_

int make3dTatou(void);
//////////////// to mode
void process_events( void );
void FastCopyScreen(void* source, void* dest);
void Rotate(unsigned int x, unsigned int y, unsigned int z, int* xOut, int* yOut);
void FadeInPhys(int var1,int var2);
void FadeOutPhys(int var1, int var2);
void playSound(int num);
void setCameraTarget(int x,int y,int z,int alpha,int beta,int gamma,int time);

void startChrono(unsigned int* chrono);
int evalChrono(unsigned int* chrono);
//////////////
#endif
