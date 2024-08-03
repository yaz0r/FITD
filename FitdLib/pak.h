#ifndef _PAK_
#define _PAK_

char* loadPak(const char* name, int index);
int LoadPak(const char* name, int index, char* ptr);
int getPakSize(const char* name, int index);
unsigned int PAK_getNumFiles(const char* name);
void dumpPak(const char* name);

#endif
