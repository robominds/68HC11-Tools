
#include <string.h>

#include "filesubs.h"

char* defaultfilename(char *szFileName, char *szDefaultFileName, char *szDefaultFileExt) {

	if(szFileName[0] == '\0') strcpy(szFileName,szDefaultFileName);
	strtok(szFileName,".");
	if(strstr(szFileName,".") == NULL) strcat(szFileName,szDefaultFileExt);
	return szFileName;
}