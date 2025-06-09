
extern int fontHeight;
void SetFont(char* fontData, int color);
void SetFontSpace(int interWordSpace, int interLetterSpace);
int ExtGetSizeFont(u8* string);
void PrintFont(int x, int y, char* surface, u8* string);
void SelectedMessage(int x, int y, int index, int color1, int color2);
void SimpleMessage(int x, int y, int index, int color);
