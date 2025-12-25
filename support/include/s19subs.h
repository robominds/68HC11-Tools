

int UploadFile(char *szLoadFile, char *szBootFile, char cOpt);
int ConvertSRecord(char *szSrec, char *cType, int *iNum, unsigned int *uiAddress,
									 int iValues[]);
int S19CheckSum(int iValues[], int iNum);
int UploadSrec(char *szSrec, char *szSrecOut);
void BuildSrec(int iType, unsigned int uiAddress, int iValues[], char *szSrec);
int LoadLoader(FILE *pfFile);
int timeout(int i);

