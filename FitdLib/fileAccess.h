#ifndef _FILE_ACCESS_
#define _FILE_ACCESS_
void fatalError(int type, const char* name);
char* loadFromItd(const char* name);
char* loadPakSafe(const char* name, int index);
#endif
