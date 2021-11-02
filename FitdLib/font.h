
extern int fontHeight;
void initFont(char* fontData, int color);
void setFontSpace(int interWordSpace, int interLetterSpace);
int computeStringWidth(u8* string);
void renderText(int x, int y, char* surface, u8* string);
void drawSlectedText(int x, int y, int index, int color1, int color2);
void SimpleMessage(int x, int y, int index, int color);
