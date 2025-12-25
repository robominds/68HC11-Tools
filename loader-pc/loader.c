
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s19subs.h"
#include "serial.h"
#include "sersubs.h"

int UploadFilex(char *szLoadFile, char *szBootFile, char cOpt);

int main(int argc, char *argv[]){
	int  i;
	int  comport=1;
	char opt = 'V';
	char execfile[80] = "";
	char loadfile[80] = "";
	char bootfile[80] = "prgf1v11.boo";

	if(argc < 2) {
		printf("LOADER-PC <FILE> [<BOOT_FILE>] [/P|/V] [/2]\n");
		printf("  /P = Program mode\n");
		printf("  /V = Verify mode (default)\n");
		printf("  /2 = Use COM2 (default is COM1)\n");
		return 1;
	}

	i = 0;  if(argc>0) while(((execfile[i]=argv[0][i])!=0) && (i<80)) i++;
	i = 0;  if(argc>1) while(((loadfile[i]=argv[1][i])!=0) && (i<80)) i++;
	i = 0;  if(argc>2) while(((bootfile[i] = argv[2][i])!=0) && (i<80)) i++;

	loadfile[79] = '\0';
	bootfile[79] = '\0';

	i = 0;
	while(loadfile[i]!='\0'&& i<79) {
		if(loadfile[i]=='/') {
			switch(loadfile[i+1]) {
				case 'P':
				case 'p':
					opt = loadfile[i+1];
					loadfile[i] = '\0';
					break;
				case '2':
					if(loadfile[i+1]=='2') comport = 2;
					break;
				default:
					break;
			}
		}
		i++;
	}

	open_port(comport, 32000);
	set_port(9600, 8, NO_PARITY, 1);

	if(strcspn(loadfile,".") == strlen(loadfile)) {
		loadfile[76] = '\0';
		strcat(strtok(loadfile,"/"),".S19");
	}

	if(strcspn(bootfile,".") == strlen(bootfile)) {
		bootfile[76] = '\0';
		strcat(strtok(bootfile,"/"),".BOO");
	}

	switch(opt){
		case 'v':
		case 'V':
			opt = 'V';
			break;
		case 'p':
		case 'P':
			opt = 'P';
			break;
		default:
			opt = '\0';
	}

	if(opt != 'P' && opt != 'V') {
		printf("\tOption seleted must be 'P'rogram or 'V'erfiy");
		exit(0);
	}
	
	if(opt == 'V') {
		printf("Option selected is Verify\n");
	} else {
		printf("Option selected is Program\n");
	}
	printf("Boot file is       %s\n",bootfile);
	printf("Load file is       %s\n",loadfile);
	printf("Using COM%1d\n",comport);

	UploadFilex(loadfile,bootfile,opt);

	close_port();
}

int UploadFilex(char *szLoadFile, char *szBootFile, char cOpt) {
	int iError=0,iErrorCount=0,i,j;
	char szLine[80],szLineOut[80];
	FILE *pfLoadFile,*pfBootFile;

	if((pfBootFile=fopen(szBootFile,"rb")) == NULL) {
		perror(szBootFile);
		printf("Error:  Could not open %s\n",szBootFile);
		return(-1);
	}
	iError=LoadLoader(pfBootFile);
	switch(iError) {
		case  0:
			break;
		default:
			printf("Communications Error while sending loader file %s\n",szBootFile);
			printf("Make sure prcessor is reset in boot mode before running\n");
			return(-1);
	}
	if((pfLoadFile=fopen(szLoadFile,"rb")) == NULL) {
		printf("Error:  Could not open %s\n",szLoadFile);
		return(-1);
	}

	if(cOpt == 'P') {
		printf("Loading file %s:\n",szLoadFile);
	} else {
		printf("Verifing file %s:\n",szLoadFile);
	}

	put_serial(cOpt);
	GetSerialTimed();

	while((fgets(szLine,80,pfLoadFile) != NULL) && (iError == 0)) {
		i = 0;
		while(szLine[i]!=13) i++;
		szLine[i] = '\0';
		if(UploadSrec(szLine,szLineOut) < 0) {
			printf("\nError sending line -\n");
			printf("%s\n",szLine);
			printf("%s\n",szLineOut);
			exit(-1);
		} else {
			if(strncmp(szLine,szLineOut,8) == 0) {
				printf("%8.8s",szLineOut);
				for(j=4;j*2<i;j++) {
					if(strncmp(&szLine[j*2],&szLineOut[j*2],2) == 0) {
						printf("%2.2s",&szLineOut[j*2]);
					} else {
						printf("(%2.2s:%2.2s)",&szLine[j*2],&szLineOut[j*2]);
						iErrorCount++;
					}
				}
				printf("\n");
			} else {
				printf("Error in returned S19 record\n%s",szLineOut);
				exit(-1);
			}
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
			printf("\nVerify complete, no errors");
		}
	}
	return(0);
}
