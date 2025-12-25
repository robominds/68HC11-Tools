
#include <string.h>

#include "hexsubs.h"

unsigned int hex2uint(char *szHex, int iNumHex) {
	unsigned int uiValue=0;

	while((*szHex != '\0') && (iNumHex-- > 0)) {
		uiValue = uiValue*16 + hex2nib(*szHex);
		szHex++;
	}
	return uiValue;
}

int ishex(char *szHex) {

	return (szHex[strspn(szHex,"0123456789ABCDEFabcdef")] == '\0');
}

char* uint2hex(unsigned int uiValue, char *szHex) {
	int i = 4;

	szHex[i] = '\0';
	while(i-- > 0) {
		szHex[i] = nib2hex(uiValue%16);
		uiValue  = (uiValue-(uiValue%16))/16;
	}
	return szHex;
}

char* uchar2hex(unsigned char ucValue, char *szHex) {
	int i = 2;

	szHex[i] = '\0';
	while(i-- > 0) {
		szHex[i] = nib2hex(ucValue%16);
		ucValue  = (ucValue-(ucValue%16))/16;
	}
	return szHex;
}

