
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hexsubs.h"
#include "filesubs.h"

#define checkadd(a) (256-(a))

int main(int argc, char *argv[]) {
	unsigned int  iAddress=0;
    unsigned int  iAdd;
	int  iSum=0,iCheckSum=0;
	int  iNum,iDone=0;
	int  i;
	int  iHexFF=255;
	int  iBytes[40];
	int  iNumBytes=0;
	char szS19File[13]="";
	char szBooFile[13]="";
	char szLine[81]="";
	FILE *pfS19File,*pfBooFile;

	if(argc==1){
		printf("S2BOO <S19 FILE> [<BOOT FILE>]");
		exit(1);
	}
	strcpy(szS19File,argv[1]);
	if(argc >= 3) strcpy(szBooFile,argv[2]);
	defaultfilename(szS19File,"",".S19");
	defaultfilename(szBooFile,szS19File,".BOO");
	if((pfS19File = fopen(szS19File,"r")) == NULL) {
		printf("Error: Can not open file %s\n",szS19File);
		exit(1);
	}
	if((pfBooFile = fopen(szBooFile,"wb")) == NULL) {
		printf("Error: Can not open file %s\n",szBooFile);
		exit(1);
	}
	fwrite(((char*)(&iHexFF)),1,1,pfBooFile);
	do {
		fgets(szLine,81,pfS19File);
		if(feof(pfS19File) == 0) {
			if(szLine[0] == 'S') {
				switch(szLine[1]) {
					case '0':
						break;
					case '1':
						iNum = hex2uint(&(szLine[2]),2);
						iAdd = hex2uint(&szLine[4],4);
						iSum = checkadd(iNum)+checkadd(iAdd/256)+checkadd(iAdd%256);
						if(iAdd == iAddress) {
							for(i=0;i<iNum-3;i++) {
								iBytes[i] = hex2uint(&(szLine[8+i*2]),2);
								iSum = iSum + checkadd(iBytes[i]);
							}
							iSum = (iSum - 1) & 255;
							iCheckSum = hex2uint(&(szLine[8+(iNum-3)*2]),2);
							if(iSum != iCheckSum) {
								printf("Error: Checksum Error\n");
								exit(1);
							}
							for(i=0;i<iNum-3;i++) {
								fwrite(((char*)(&iBytes[i])),1,1,pfBooFile);
								iNumBytes++;
							}
							iAddress += iNum-3;
						} else {
							printf("Error: S record address must start at zero and be contiguous\n");
							iDone = -1;
						}
						break;
					default:
						break;
				}
			} else {
				printf("File error, not an S record\n");
				printf("%s",szLine);
			}
		}
	} while((feof(pfS19File) == 0) && (iDone == 0));
	while(iNumBytes++ < 256) fwrite("\0",1,1,pfBooFile);
	fclose(pfS19File);
	fclose(pfBooFile);
	return 0;
}
