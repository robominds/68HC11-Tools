
#include <stdio.h>
#include <stdlib.h>
#include "dos_compat.h"

#include "s19subs.h"
#include "hexsubs.h"
#include "sersubs.h"
#include "serial.h"

#define checkadd(a) (256-(a))


int UploadFile(char *szLoadFile, char *szBootFile, char cOpt) {
	int iError=0,iErrorCount=0;
	char szLine[80],szLineOut[80];
	FILE *pfLoadFile,*pfBootFile;

	if((pfBootFile=fopen(szBootFile,"br")) == NULL) {
		perror(szBootFile);
		printf("Error:  Could not open %s\n",szBootFile);
		return(-1);
	}
	if(LoadLoader(pfBootFile)) {
		switch(iError) {
			case -1:
				printf("Communications Error while sending loader file\n");
				break;
			default:
				printf("Error sending loader\n");
				break;
		}
	}
	if((pfLoadFile=fopen(szLoadFile,"br")) == NULL) {
		printf("Error:  Could not open %s\n",szLoadFile);
		return(-1);
	}

	if(cOpt == 'P') {
		printf("Loading file %s:",szLoadFile);
	} else {
		printf("Verifing file %s:",szLoadFile);
	}

	while(fgets(szLine,80,pfLoadFile) != NULL) {
		if(UploadSrec(szLine,szLineOut) < 0) {
			printf("Error: Could not send -\n");
			printf("%s",szLine);
			iErrorCount++;
		}
	}

	if(iErrorCount > 0) {
		if(cOpt == 'P') {
			printf("\nFailure during load : %d ERRORS\n",iErrorCount);
			return(-2);
		} else {
			printf("\nFailure during verify : %d ERRORS\n",iErrorCount);
			return(-2);
		}
	} else {
		if(cOpt == 'P') {
			printf("\nLoad Complete");
		} else {
			printf("\nVerify complete");
		}
	}
	return(0);
}

int UploadSrec(char *szSrec, char *szSrecOut) {
	int  i,kk=0;
	int  iNum;
	int  iError=0;
	unsigned int uiAddress;
	int iValues[80];
	int iTemp;
	char cType;

	ConvertSRecord(szSrec,&cType,&iNum,&uiAddress,iValues);
	szSrecOut[kk] = '\0';
	if(S19CheckSum(iValues,iNum)) {
		for(i=0;i<iNum+3;i++) {
			put_serial((char)iValues[i]);
			if((iTemp=GetSerialTimed()) < 0) {
				return(-2);
			} else {
				if(i > 1) {
					uchar2hex((unsigned char)iTemp,&szSrecOut[kk]);
					kk = kk + 2;
				} else {
					szSrecOut[kk++] = (char)iTemp;
					szSrecOut[kk]   = '\0';
				}
			}
		}
	} else {
		iError = -1;
	}
	return(iError);
}

int ConvertSRecord(char *szSrec, char *cType, int *iNum, unsigned int *uiAddress,
									 int iValues[]) {
	int iError = 0;
	int i;

	if((szSrec[0]=='S') && (ishex(&szSrec[1]))) {
		*cType = szSrec[1];
		*iNum = hex2uint(&(szSrec[2]),2);
		*uiAddress = hex2uint(&szSrec[4],4);
		iValues[0] = szSrec[0];
		iValues[1] = szSrec[1];
		for(i=2;i<=*iNum+2;i++) iValues[i] = hex2uint(&(szSrec[(i-1)*2]),2);
	} else {
		iError = -1;
	}
	return iError;
}

int S19CheckSum(int iValues[], int iNum){
	int iCheckSum=0;
	int i;

	for(i=2;i<iNum+2;i++) iCheckSum += checkadd(iValues[i]);
	iCheckSum = (iCheckSum-1) & 255;
	return (iCheckSum == iValues[iNum+2]);
}

void BuildSrec(int iType, unsigned int uiAddress, int iValues[], char *szSrec){
	int i;
	int iCheckSum = 0;
	int iNum      = 0;

	while(iValues[iNum] >= 0) iNum++;
	szSrec[0] = 'S';
	szSrec[1] = '0' + iType;
	uint2hex((char)iNum,&szSrec[2]);
	iCheckSum = checkadd(iNum);
	uint2hex(uiAddress,&szSrec[4]);
	iCheckSum += checkadd(uiAddress/16) + checkadd(uiAddress%16);
	for(i=0;i<iNum;i++) {
		uchar2hex(uiAddress,&szSrec[8+i*2]);
		iCheckSum += checkadd(iValues[i]);
	}
	uchar2hex((char)iCheckSum,&szSrec[8+iNum*2]);
	iCheckSum += checkadd(iCheckSum);
}

int LoadLoader(FILE *pfFile) {
	int  cs[1024];
	int  cr[1024];
	int  i = 0;
	int  j = 0;
	int  k = 0;
	long int liBaud;

	liBaud = get_baud();
	set_baud(1200L);
	get_serial();
	while(in_ready()) get_serial();
	while((cs[i]=getc(pfFile)) != EOF) put_serial(cs[i++]);
	timeout(TIME_OUT_RESET);
	while(!timeout(TIME_OUT_LIM))	;
	while(in_ready()) cr[j++] = get_serial();
	set_baud(liBaud);
	for(k=-1;(k<i-1) & (cs[1] != cr[1+k]);k++) ;
	printf("Offset = %d\n",k);
	for(j=0;j<i;j++) {
		printf("%2.2x:%2.2x ",cs[j],cr[j]);
		if(j%10 == 9) printf("\n");
	}
	printf("\n");
	for(j=1;j<i-1;j++) if(cs[j] != cr[j+k]) return(-1);

	return(0);
}


