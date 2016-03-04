#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_CMDLINE_LEN 256

/* function prototypes go here... */

void show_prompt();
int get_cmd_line(char *cmdline);
void process_cmd(char *cmdline);
int process_amp(char *cmdline);
int process_and_or(char *cmdline);
int process_pipe(char *cmdline);
void process_unit(char *cmdline, int flag);


/* The main function implementation */
int main()
{
	char cmdline[MAX_CMDLINE_LEN];
	
	while (1) 
	{
		show_prompt();
		int flag = get_cmd_line(cmdline);
		if ( flag == -1 )
			continue; /* empty line handling */
		process_cmd(cmdline);
	}
	return 0;
}

void process_cmd(char *cmdline){
	process_amp(cmdline);
}

int process_amp(char *cmdline){
	printf("process amp: %s\n", cmdline);
	const char *AMP = "&";
	int i = 0;
	int n = strlen(cmdline);

	char* pos_amp = strstr(cmdline, AMP);
	
	if(pos_amp==NULL) return process_and_or(cmdline);
	else {
		while(pos_amp+1!=NULL&&*(pos_amp+1)=='&'){	
			if(pos_amp!=cmdline+n-2)
				pos_amp = strstr(pos_amp+2, AMP);
			else{
				pos_amp = NULL;
				break;
			} 
		}

		while (i < n && isspace(cmdline[i])){
			++i;
		} 
	
		if(pos_amp == cmdline+i) {
			printf("-myshell: syntax error near unexpected token `&'\n");
			return 1;
		}
		else if(pos_amp!=cmdline+n-1){
			*pos_amp = '\0';
			//should fork()
			return process_and_or(cmdline)*process_amp(++pos_amp);
		}
		else {
			*pos_amp = '\0';
			//should fork()
			return process_and_or(cmdline);
		}
	}

}

int process_and_or(char *cmdline){
   printf("process AND OR: %s\n", cmdline);

   const char* AND = "&&";
   const char* OR = "||";

   char *token_a = strstr(cmdline, AND);
   char *token_b = strstr(cmdline, OR);
   int i = 0;
   int n = strlen(cmdline);

   while (i < n && isspace(cmdline[i])){
		++i;
	}

   if(token_a==cmdline+i || token_a==cmdline+n-2) {
   		printf("-myshell: syntax error near unexpected token `&&'\n");
   		return 1;
   }
   else if(token_b==cmdline+i || token_b==cmdline+n-2) {
   		printf("-myshell: syntax error near unexpected token `||'\n");
   		return 1;
   }
   else if(token_a!=NULL && token_b!=NULL){
      if(token_a > token_b){
         *token_b = '\0';
         *(token_b+1) = '\0';

         if(process_pipe(cmdline)==1) return process_and_or(token_b+2);
      }
      else if (token_a < token_b){
         *token_a = '\0';
         *(token_a+1) = '\0';

         if(process_pipe(cmdline)==0) return process_and_or(token_a+2);
      }
   }
   else if(token_a!=NULL && token_b==NULL){
      *token_a = '\0';
      *(token_a+1) = '\0';

      if(process_pipe(cmdline)==0) return process_and_or(token_a+2);
   }
   else if(token_a==NULL && token_b!=NULL){
      *token_b = '\0';
      *(token_b+1) = '\0';

      if(process_pipe(cmdline)==1) return process_and_or(token_b+2);
   } 
   else return process_pipe(cmdline);
   return 1;
}

int process_pipe(char* cmdline){
	printf("process pipe: %s\n", cmdline);
	return 0;
}


void process_unit(char *cmdline, int flag)
{
	// printf("%s\n", cmdline);
	const char* EXIT = "exit";
	const char* CHILD = "child";
	const char* CD = "cd";
	const char* SPACE = " ";
	const char* COMMA = ":";
	int child_status;
	pid_t child_pid;

	char* token;
	char file[10] = "         ";
	int t;

	token = strtok(cmdline, SPACE);
	if(token!=NULL){
		if(strcmp (token, EXIT)==0){
			exit(0);
		}
		else if(*token=='&') {
			// this line should be modified after adding operator &&
			if(token[1]=='&') printf("-myshell: syntax error near unexpected token `&&'\n"); 
			else if(flag==1) printf("-myshell: syntax error near unexpected token `&'\n");
			else printf("-myshell: syntax error near unexpected token `&'\n");
		}
		else if(strcmp (token, CD)==0 && flag==1){
			return;
		}
		else if(strcmp (token, CD)==0 && flag==0){
			token = strtok(NULL, SPACE);
			if(token==NULL) chdir(getenv("HOME"));
            else if(chdir(token)!=0) printf("-myshell: cd: %s: no such file or directory\n", token);
		}
		/*//Find a file
		char *paths = getenv("PATH");
		char* path;

		path = strtok(paths, COMMA);

		while(path!=NULL){

    		path = strtok(NULL, COMMA);
		} 

		free(paths);

		token = strtok(cmdline, SPACE);
		while(token!=NULL){

		}*/
	}
	else if(flag==1) printf("-myshell: syntax error near unexpected token `&'\n");
	

	/*sscanf(cmdline, "%s %d", cmd, &t);
	
	if(strcmp (cmd, EXIT)==0){
		printf("my shell is terminated with pid %d\n", getpid());
		exit(0);
	}
	else if(strcmp(cmd, CHILD)==0){
		pid_t pid = fork();
		int len = strlen(cmdline) - 6;
		if(pid > 0){			
			child_pid = wait(&child_status);
			printf("child pid %d is terminated with status %d\n", child_pid, child_status);
			return;
		}
		else{
			printf("child pid %d is started\n", getpid());
			sleep(t);
			exit(0);
		}
	}
	else {
		printf("Invalid input!\n");
		exit(-1);
	}*/

}


void show_prompt() 
{
	/*Find the current directory*/
	char* cwd;
	char buff[PATH_MAX + 1];
	char* token;
	char* dir;
	const char* SLASH = "/";

    cwd = getcwd( buff, PATH_MAX + 1 );
    token = strtok(cwd, SLASH);
	while(token!=NULL){
		dir = token;
		token = strtok(NULL, SLASH);
	}

	printf("[%s] myshell> ", dir);
}


int get_cmd_line(char *cmdline) 
{
    int i;
    int n;
    int ret;
    if (!fgets(cmdline, MAX_CMDLINE_LEN, stdin))
        return -1;
    // Ignore the newline character
    n = strlen(cmdline);
    cmdline[--n] = '\0';
    i = 0;
    while (i < n && cmdline[i] == ' ')
    {
        ++i;
    }
    if (i == n)
    {
        // Empty command
        return -1;
    }
    else {cmdline=cmdline+i;}
    i = n-1;
    while (i > -1 && isspace(cmdline[i])){
    	cmdline[i] = '\0';
    	--i;
    }

    //should handle the case where the cmdline ends with an operator


    return 0;
}
