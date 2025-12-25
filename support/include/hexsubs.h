
#include <_defs.h>

#define hex2nib(a) (((a)<'A')?((a)-'0'):(((a)<'a')?((a)-'A'+10):((a)-'a'+10)))
#define nib2hex(a) (((a)<10)?((a)+'0'):((a)-10+'A'))

unsigned int hex2uint(char *szHex, int iNumHex);
int ishex(char *szHex);
char* uint2hex(unsigned int uiValue, char *szHex);
char* uchar2hex(unsigned char ucValue, char *szHex);

