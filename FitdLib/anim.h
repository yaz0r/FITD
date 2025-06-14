#define INFO_TRI 1
#define INFO_ANIM 2
#define INFO_TORTUE 4
#define INFO_OPTIMISE 8

int InitAnim(int animNum,int animType, int animInfo);
int SetAnimObjet(int frame, char* anim, char* body);
s16 SetInterAnimObjet(int frame, char* animPtr, char* bodyPtr);
s16 GetNbFramesAnim(char* animPtr);
void StockInterAnim(std::vector<s16>& animBuffer, char* bodyPtr);
void ResetStartAnim(char* bodyPtr);
void updateAnimation(void);
void InitCopyBox(char* var0, char* var1);

