#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/

#define clear() system("clear")

char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void print_cur_Dir(){
	char cwd[MAX_INPUT_SIZE];
	getcwd(cwd, sizeof(cwd));
	printf("%s ", cwd);	
}


int basic(char ** token, char **envp, char ***history, int j){
	int NoOfCmds = 10, i;
  char* ListOfCmds[NoOfCmds];

  ListOfCmds[0] = "clear";
  ListOfCmds[1] = "env";
  ListOfCmds[2] = "cd";
  ListOfCmds[3] = "pwd";
  ListOfCmds[4] = "mkdir";
  ListOfCmds[5] = "rmdir";
  ListOfCmds[6] = "ls";
  ListOfCmds[7] = "history";
  ListOfCmds[8] = "exit";

  for (i = 0; i < NoOfCmds; i++) {
    if (strcmp(token[0], ListOfCmds[i]) == 0) {
      break;
    }
  }
  // if(i == 9) return 0;

  switch (i) {
  case 0:
  	clear();
  	break;
  case 1:
  	for(char **x = envp; *x; ++x){
  		printf("%s\n", *x);
  	}
  	break;
  case 2:
  	chdir(token[1]);
  	break;
  case 3:
  	print_cur_Dir();
  	printf("\n");
  	break;
  case 4:
  	for(i = 1; token[i]; ++i){
	  	if(mkdir(token[i], 0777) == -1){
	  		printf("Cannot create the %s directory.\n", token[i]);
	  	}
  	}
  	break;
  case 7:
  	for(i = 0; i < j; ++i){
  		for(int k = 0; history[i][k]; ++k){
				printf("%s %d\n", history[i][k], i);
			}
  	}
  	break;
  case 8:
  	exit(0);
  default:
  	return 2;
  }

  return 1;
}


void syscom(char** tokens)
{
    int pid = fork(); 
  
    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    }
    else if (pid == 0) {
        if (execvp(tokens[0], tokens) < 0) {
            printf("\nCould not execute command..\n");
        }
        exit(0);
    }
    else {
        wait(NULL); 
        return;
    }
}


int main(int argc, char* argv[], char **envp) {
	clear();
	char ***history;
	history = (char***)malloc(MAX_INPUT_SIZE * sizeof(char **));
	int j = 0;
	char line[MAX_INPUT_SIZE];         
	char **tokens;              
	int i;
	int ok = (argc == 2);
	FILE* fp;
	
	if(ok) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	while(1) {
		tokens = NULL;
		/* BEGIN: TAKING INPUT */
			
			bzero(line, sizeof(line));
		if(ok){
			fgets(line, MAX_INPUT_SIZE, fp);
		}else{
			printf("\033[0;31m");
			print_cur_Dir();

			printf("$ ");
			printf("\033[0m");
			scanf("%[^\n]", line);
			getchar();
		}


		// printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);


   
    //do whatever you want with the commands, here we just print them
		if(basic(tokens, envp, history, j) == 2){
			syscom(tokens);
		}
		
		history[j++] = tokens;

	}
	for(; j > -1; j--){
		for(i = 0; history[j][i]; ++i){
			free(history[j][i]);
		}
		free(history[j]);
	}
	free(history);

	fclose(fp);

	return 0;
}
