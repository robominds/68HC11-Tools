
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#define TRUE  1
#define FALSE 0

int parse_file(FILE *outfile, char *loadfile, char opt, char parm[][10], char *execdir);
char *token(char *temp0, char* temp1, char* temp2, char* temp3);


int main(int argc, char *argv[]){

	int  i, lastslash = -1;
	char opt = 'V';
	char execfile[256] = "";
	char execdir[256] = "";
	char asmfile[256]  = "";
	char parm[10][10] = {"","","","","","","","","",""};
	FILE *outfile;

	for(i=0;i<10;i++) parm[i][0] = '\0';

	/* Copy argv[0] and find last slash */
	i = 0;
	while(argv[0][i] && i<255) {
		execfile[i] = argv[0][i];
		if(argv[0][i] == '/') lastslash = i;
		i++;
	}
	execfile[i] = '\0';
	
	/* Extract directory from executable path */
	if(lastslash >= 0) {
		for(i=0; i<=lastslash; i++) {
			execdir[i] = execfile[i];
		}
		execdir[lastslash+1] = '\0';
	} else {
		execdir[0] = '\0';  /* No path, running from current dir */
	}
	i = 0;
	if(argc>1)
		while(argv[1][i] && i<255) {
			asmfile[i] = argv[1][i];
			i++;
		}
	asmfile[255] = '\0';

	i = 0;
	while(asmfile[i]!='\0'&& i<255) {
		if(asmfile[i]=='/') {
			opt = asmfile[i+1];
			asmfile[i] = '\0';
		}
		i++;
	}

	if((outfile=fopen("temp.asm","w")) == NULL) {
		printf("%s not created, parse aborted\n","temp.asm");
		return 1;
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
	printf("Asm file is       %s\n",asmfile);
	parse_file(outfile,asmfile,opt,parm,execdir);
	fclose(outfile);
	return 0;
}

int parse_file(FILE *outfile, char *asmfile, char opt, char subs[][10], char *execdir) {
	char line[256];
	char tline[256];
	char temp[256];
	char file[256];
	char parm[10][10] = {"","","","","","","","","",""};
	char labs[10][10] = {"","","","","","","","","",""};
	int  i = 0;
	int  j = 0;
	int  k = 0;
	int  l = 0;
	int  match;
	FILE *infile;
	char szTemp[256];
	
	/* Build path to includes directory relative to executable */
	snprintf(szTemp, sizeof(szTemp), "%sincludes/", execdir);

	if((infile=fopen(asmfile,"r")) == NULL) {
		if((infile=fopen(strcat(szTemp,asmfile),"r")) == NULL) {
			printf("%s does not exists, parse aborted\n",asmfile);
			return(1);
		}
	}
	for(i=0;i<10;i++) {
		parm[i][0] = '\0';
		labs[i][0] = '\0';
	}
	while(fgets(line,256,infile) != NULL) {
		temp[0] = '\0';
		file[0] = '\0';
		i = 0;
		j = 0;
		k = 0;
		l = 0;
		if(line[0] == '#') {
			printf("*%s",line);
			fputc('*',outfile);
			fputs(line,outfile);
			token(temp,line," ","\"");
			printf("temp = x%sx\n",temp);
		if(strcasecmp(temp,"#include") == 0){
				l = 0;
				token(file,NULL,"(","\"");
				printf("file = x%sx\n",file);
				while(strlen(token(parm[l],NULL,",","\""))!=0) {
					printf("* parmeter %d = x%sx\n",l,parm[l]);
					l++;
				}
				token(parm[l],NULL,")","\"");
				printf("* parmeter %d = x%sx\n",l,parm[l]);
			parse_file(outfile,file,opt,parm,execdir);
		} else if(strcasecmp(temp,"#header") == 0) {
				l = 0;
				token(file,NULL,"(","\"");
				printf("file = x%sx\n",file);
				while(strlen(token(labs[l],NULL,",","\""))!=0) {
					printf("* label %d = x%sx\n",l,labs[l]);
					l++;
				}
				token(labs[l],NULL,")","\"");
				printf("* label %d = x%sx\n",l,labs[l]);
			} else {
				printf("* Pragma '%s' not supported\n",temp);
				fputs("* pragma '",outfile);
				fputs(temp,outfile);
				fputs("' not supported\n",outfile);
			}
		} else {
			printf("%s",line);
			i = 0;
			j = 0;
			k = 0;
			l = 0;
			match = 10;
			while(line[i]!='\n' && line[i]!='\0') {
				j = 0;
				match = 10;
				l = 0;
				while(j<10 && labs[j][0]!='\0') {
					if(strncmp(&line[i],labs[j],strlen(labs[j]))==0) match = j;
					j++;
				}
				if(match<10) {
					l = 0;
					while(subs[match][l] != '\0') {
						tline[k] = subs[match][l];
						l++;
						k++;
					}
					i += strlen(labs[match]);
				} else {
					tline[k] = line[i];
					i++;
					k++;
				}
			}
			tline[k] = '\n';
			tline[k+1] = '\0';
			fputs(tline,outfile);
		}
	}
	fclose(infile);
	return(0);
}

char *token(char* temp0, char* temp1, char* temp2, char* temp3) {

	size_t i,j,k=0,l=0;
	int quoteon = FALSE;
	int useasis = FALSE;
	static size_t jj;
	static char* temp1x;

	if(temp1!=NULL) {
		jj     = 0;
		temp1x = temp1;
	} else {
		temp1x = temp1x + jj;
		jj = 0;
	}
	for(j=0;j<strlen(temp1x);j++) {
		if(temp1x[j]!='\0') {
			for(i=0;i<strlen(temp3);i++) {
				if(temp1x[j]==temp3[i]) quoteon = !quoteon;
			}
			if(temp1x[j]=='\\') {
				j++;
				useasis = TRUE;
			}
			if(!quoteon) {
				for(i=0;i<strlen(temp2);i++) {
					if(temp1x[j]==temp2[i] && !useasis) {
						temp1x[j] = '\0';
						jj=j+1;
						while(temp1x[k]!='\0') {
							if(temp1x[k]=='\\') {
								k++;
							} 
							temp0[l++]=temp1x[k];
							k++;
						}
						temp0[l]='\0';
						return temp0;
					}
				}
			}
			useasis = FALSE;
		}
	}
	temp0[0] = '\0';
	return temp0;
}