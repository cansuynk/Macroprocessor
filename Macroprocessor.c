#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct symbol {
	char name[10];
	char value[10];
};
struct symbol def_symbols[50];

struct mac {
	char mname[10];
	int nparams;
	char p[3][10];
	char mbody[1000];
};
struct mac macros[20];

struct param {
	char dp[3][10];
	char rp[3][10];
};
struct param PT[5];

int main( void ){
	int symbolnum=0;
	int macronum=0;
	int paramnum=0;
	int i,j,a;
	char word[100];
	char chr;
	FILE *fps, *fpe;
	fps=fopen("SourceCode.txt","r");
	fpe=fopen("ExpandedCode.txt","w");
	if ( fps==NULL )
		printf("Error opening SourceCode");
	if ( fpe==NULL )
		printf("Error opening ExpandedCode");
	
	fscanf(fps,"%s",word);
	while (strcmp(word,"@DEFINE")==0 || strcmp(word,"@MACRO")==0){  //Reading define and macro definitions and printing-Step1

		if(strcmp(word,"@DEFINE")==0){
			fscanf(fps,"%s %s",def_symbols[symbolnum].name,def_symbols[symbolnum].value);

			printf("Def-Symbols Table:\n");
			printf("Symbol Name:\t\t%s\n",def_symbols[symbolnum].name);
			printf("Replacement Text:\t%s\n\n",def_symbols[symbolnum].value);
			symbolnum++;	
		}
		
		else if(strcmp(word,"@MACRO")==0){
			macros[macronum].nparams=0;
			fscanf(fps,"%s",macros[macronum].mname);
			chr=fgetc(fps);
			if(chr==' ' || chr=='\n'){
				chr=fgetc(fps);
			}
			a=0;
			i=0;
			j=0;
			
			if(chr=='('){          //Reading macroparameters and inserting in table
				while(chr!=')'){
					chr=fgetc(fps);
					if (chr==',' || chr==')'){
						i++;
						j=0;
						macros[macronum].nparams++;
					}
					if(chr==','){
						chr=fgetc(fps);
					}
					if(chr==' '){
						chr=fgetc(fps);
					}if(chr!=')'){
						macros[macronum].p[i][j]=chr;
						j++;
					}	
				}
			}
			chr=fgetc(fps);
			while (chr!='@'){        //Reading macrobody and inserting in table
				macros[macronum].mbody[a]=chr;
				chr=fgetc(fps);
				a++;
				if(chr=='@'){
					char chr2=chr;
					chr=fgetc(fps);
					if(chr=='M'){
						macros[macronum].mbody[a]=chr2;
						a++;
					}else{
						break;
					}
				}				
			}
			macros[macronum].mbody[a-1]='\0';
			printf("Macros Table:\n");
			printf("Macro Name:\t\t%s\n",macros[macronum].mname);
			printf("Number of Parameters:\t%d\n",macros[macronum].nparams);
			if (macros[macronum].nparams==1)
				printf("Dummy Parameters:\t%s\n",macros[macronum].p[0]);
			else if (macros[macronum].nparams==2){
				printf("Dummy Parameters:\t%s, ",macros[macronum].p[0]);
				printf("%s\n",macros[macronum].p[1]);
			}
			else if (macros[macronum].nparams==3){
				printf("Dummy Parameters:\t%s, ",macros[macronum].p[0]);
				printf("%s, ",macros[macronum].p[1]);
				printf("%s\n",macros[macronum].p[2]);
			}	
			printf("Macro Body:\n%s",macros[macronum].mbody);
			macronum++;	
		}
		fscanf(fps,"%s",word);
		if(strcmp(word,"NDM")==0){
			fscanf(fps,"%s",word);
		}
	}
	fputc('\t',fpe);
	fprintf(fpe, "%s",word);
	while (!feof(fps)){           //Reading the rest of the input file-Step2
		
		chr=fgetc(fps);
		while (chr!='@' || feof(fps)){
			fputc(chr,fpe);
			chr=fgetc(fps);
			if(feof(fps)){return 0;}
		}
		fscanf(fps,"%s",word);
			if(strcmp(word,"MCALL")==0){     //If there is a macrocall
				fscanf(fps,"%s",word);
				int x;
				int currentmacro=-1;
				for(x=0; x<macronum; x++){
					if(strcmp(word,macros[x].mname)==0){
						currentmacro=x;
					}
				}
				if(currentmacro==-1){
					printf("Error:There is no macro with this name");
				}
				else{               //Creating paramTable and filling it
					int index=0;
					while(macros[currentmacro].p[0][index]!='\0'){   //inserting dummy params
						
						PT[paramnum].dp[0][index]=macros[currentmacro].p[0][index];
						index++;
						
					}PT[paramnum].dp[0][index]='\0';
					index=0;
					while(macros[currentmacro].p[1][index]!='\0'){
						
						PT[paramnum].dp[1][index]=macros[currentmacro].p[1][index];
						index++;
					}PT[paramnum].dp[1][index]='\0';
					index=0;
					while(macros[currentmacro].p[2][index]!='\0'){
						
						PT[paramnum].dp[2][index]=macros[currentmacro].p[2][index];
						index++;
					}PT[paramnum].dp[2][index]='\0';
					chr=fgetc(fps);
					if(chr==' ' || chr=='\n' || chr=='\t'){
						chr=fgetc(fps);
					}
					a=0;
					i=0;
					j=0;
			
					if(chr=='('){            //inserting real params
						while(chr!=')'){
							chr=fgetc(fps);
							if (chr==',' || chr==')'){
								i++;
								j=0;
							}
							if(chr==','){
								chr=fgetc(fps);
							}
							if(chr==' '){
								chr=fgetc(fps);
							}
							if(chr!=')'){
								PT[paramnum].rp[i][j]=chr;
								j++;
							}	
						}
					}
					int index1=2;
					int index2=0;
					char dummy[10];
					while(macros[currentmacro].mbody[index1+1]!='\0'){    //Writing macrobody to ExpandedCode file
						
						fputc(macros[currentmacro].mbody[index1],fpe);
						index1++;
						if(macros[currentmacro].mbody[index1-1]==' ' || macros[currentmacro].mbody[index1-1]=='\t' 
							&&  macros[currentmacro].mbody[index1-2]!='\n'){
							while(macros[currentmacro].mbody[index1]!='\n'){
								if(macros[currentmacro].mbody[index1]==',' 
								|| macros[currentmacro].mbody[index1]=='\n')
									break;
								
								dummy[index2]=macros[currentmacro].mbody[index1];
								index1++;
								index2++;
							}dummy[index2]='\0';
							
							if(strcmp(dummy,PT[paramnum].dp[0])==0){    //substituting
								
								fprintf(fpe, "%s",PT[paramnum].rp[0]);
							}
							else if(strcmp(dummy,PT[paramnum].dp[1])==0){
								fprintf(fpe, "%s",PT[paramnum].rp[1]);
							}
							else if(strcmp(dummy,PT[paramnum].dp[2])==0){
								fprintf(fpe, "%s",PT[paramnum].rp[2]);
							}else{
								fprintf(fpe, "%s",dummy);
							}
							index2=0;	
						}
						
					}paramnum++;
				}
			}
			else if(strcmp(word,"IF")==0){      //If there is @IF
				chr=fgetc(fps);chr=fgetc(fps);
				int index3=0;
				char name[10];char value[10];
				char condition;int ok=0;
				chr=fgetc(fps);
				while(chr!='='){
					if(chr=='>' || chr=='<')
						break;
					name[index3]=chr;
					chr=fgetc(fps);
					index3++;
					
				}name[index3]='\0';
				condition=chr;
				index3=0;
				chr=fgetc(fps);
				while(chr!=')'){
					value[index3]=chr;
					chr=fgetc(fps);
					index3++;	
				}value[index3]='\0';
				index3=0;
				chr=fgetc(fps);
				int xx;
				int currentname=-1;
				for(xx=0; xx<symbolnum; xx++){
					if(strcmp(name,def_symbols[xx].name)==0){
						currentname=xx;
						break;
					}
				}
				if(currentname!=-1){        //Checking
					if(condition=='=' && (strcmp(def_symbols[xx].value,value)==0)){
						ok=1;
					}
					else if(condition=='<' && (strcmp(def_symbols[xx].value,value)>0)){
						ok=1;
					}
					else if(condition=='>' && (strcmp(def_symbols[xx].value,value)<0)){
						ok=1;
					}
					if(ok!=1){
						fscanf(fps,"%s",word);
						while(strcmp(word,"@ELSE")!=0){
							fscanf(fps,"%s",word);
						}
						chr=fgetc(fps);
					}
					chr=fgetc(fps);chr=fgetc(fps);
				}
				
			}
			else if(strcmp(word,"ELSE")==0){
				
				fscanf(fps,"%s",word);
				while(strcmp(word,"@ENDIF")!=0){
					fscanf(fps,"%s",word);
				}
				chr=fgetc(fps);chr=fgetc(fps);
			}else if(strcmp(word,"ENDIF")==0){
				chr=fgetc(fps);chr=fgetc(fps);
			}
			
	}
	fclose(fps);
	fclose(fpe);
	return 0;
}
