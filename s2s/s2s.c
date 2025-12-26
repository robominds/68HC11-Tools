
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hexsubs.h"
#include "filesubs.h"
#include "s19subs.h"

#define checkadd(a) (256-(a))

int main(int argc, char *argv[]) {
	unsigned int  iAddress=0;
	int  iSum=0,iCheckSum=0;
	int  iNum,iDone=0,iAddSize;
	int  i;
	int  iHexFF=255;
	int  iBytes[40];
	int  iNumBytes=0;
	int  iLineLen = 32;
	long int  iAdd;
	long int  iGoAdd = -1;
	char szS19File[256]="";
	char szBooFile[256]="";
	char szLine[256]="";
	FILE *pfS19File,*pfBooFile;

	if(argc==1){
		printf("S2S <S19 FILE> [<S19 FILE>] [<Line Length>] [<Go Addr>]");
		exit(1);
	}
	strcpy(szS19File,argv[1]);
	if(argc >= 3) strcpy(szBooFile,argv[2]);
	if(argc >= 4) iLineLen = atoi(argv[3]);
	if(argc >= 5) iGoAdd = atoi(argv[4]);
	defaultfilename(szS19File,"",".S19");
	defaultfilename(szBooFile,szS19File,".x19");
	printf("S-Record Input File  : %s\n",szS19File);
	printf("S-Record Output File : %s\n",szBooFile);
	printf("S-Record Line Length : %d\n",iLineLen);
	if(iGoAdd > 0) {
	  printf("S-Record Go Address  : $%lx\n",iGoAdd);
	} else {
	  printf("S-Record Go Address  : default");
	}
	if((pfS19File = fopen(szS19File,"r")) == NULL) {
		printf("Error: Can not open file %s\n",szS19File);
		exit(1);
	}
	if((pfBooFile = fopen(szBooFile,"w")) == NULL) {
		printf("Error: Can not open file %s\n",szBooFile);
		exit(1);
	}
	char bHexFF = (char)iHexFF;
	fwrite(&bHexFF,1,1,pfBooFile);
		do {
			fgets(szLine, sizeof(szLine), pfS19File);
		if(feof(pfS19File) == 0) {
			if(szLine[0] == 'S') {
					int addrBytes = 0;
					switch(szLine[1]) {
						case '0': addrBytes = 0; break; /* header */
						case '1': addrBytes = 2; break; /* 16-bit addr */
						case '2': addrBytes = 3; break; /* 24-bit addr */
						case '3': addrBytes = 4; break; /* 32-bit addr */
						case '7': addrBytes = 4; break; /* termination */
						case '8': addrBytes = 3; break; /* termination */
						case '9': addrBytes = 2; break; /* termination */
						default: addrBytes = 0; break;
					}

					switch(szLine[1]) {
						case '0':
							/* ignore header */
							break;
						case '1':
						case '2':
						case '3': {
							/* Parse count and addresses */
							iNum = hex2uint(&(szLine[2]), 2);
							iAdd = hex2uint(&(szLine[4]), addrBytes * 2);
							int dataLen = iNum - addrBytes - 1; /* exclude addr + checksum */
							int dataPos = 4 + addrBytes * 2;

							/* Verify checksum using original project style */
							int sumCheck = 0;
							/* include count */
							int tmp = hex2uint(&(szLine[2]), 2);
							sumCheck = (sumCheck + checkadd(tmp)) & 0xFF;
							/* include address bytes */
							for(i = 0; i < addrBytes; i++) {
								int b = hex2uint(&(szLine[4 + i*2]), 2);
								sumCheck = (sumCheck + checkadd(b)) & 0xFF;
							}
							/* include data bytes */
							for(i = 0; i < dataLen; i++) {
								int b = hex2uint(&(szLine[dataPos + i*2]), 2);
								sumCheck = (sumCheck + checkadd(b)) & 0xFF;
							}
							/* final checksum per S-record spec */
							int computed = (sumCheck - 1) & 0xFF;
							/* locate checksum at end of line: starts at 2 + 2 + iNum*2 */
							int chkPos = 4 + addrBytes*2 + dataLen*2;
							iCheckSum = hex2uint(&(szLine[chkPos]), 2);
							if(computed != iCheckSum) {
								printf("Error: Checksum Error, computed = %2.2x, file = %2.2x\n", computed, iCheckSum);
								exit(1);
							}

							/* Write data bytes to output */
							for(i = 0; i < dataLen; i++) {
								char bByte = (char)hex2uint(&(szLine[dataPos + i*2]), 2);
								fwrite(&bByte, 1, 1, pfBooFile);
								iNumBytes++;
							}
							iAddress += dataLen;
							break;
						}
						case '4':
						case '5':
						case '6':
							printf("Record type '%c' not supported\n", szLine[1]);
							break;
						case '7':
						case '8':
						case '9':
							/* termination records; stop processing */
							iDone = 1;
							break;
						default:
							printf("Error: Record type '%c' not illegal\n", szLine[1]);
							exit(0);
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
