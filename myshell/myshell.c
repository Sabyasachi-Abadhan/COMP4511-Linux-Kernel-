#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_CMDLINE_LEN 256
#define ARG_MAX_NUM 256

/* function prototypes go here... */

void show_prompt();
int get_cmd_line(char *cmdline);
void strtrim(char *cmdline);
void process_cmd(char *cmdline);
void process_amp(char *cmdline);
int process_and_or(char *cmdline);
int process_pipe(char *cmdline);
int process_unit(char *cmdline);
int find_file(char* filename);


/* The main function implementation */
int main()
{
	char cmdline[MAX_CMDLINE_LEN];
	
	while (1) 
	{
		show_prompt();
		int flag = get_cmd_line(cmdline);
		if ( flag == -1 ){
			continue; /* empty line handling */
		}
		process_cmd(cmdline);
	}
	return 0;
}

void process_cmd(char *cmdline){
   if(*cmdline=='|'&&*(cmdline+1)!='|'){
   		printf("-myshell: syntax error near unexpected token `|'\n");
   		return;
   }
   else if(*cmdline=='&'&&*(cmdline+1)!='&'){
   		printf("-myshell: syntax error near unexpected token `&'\n");
   		return;
   }
   else if (*cmdline=='&'&&*(cmdline+1)=='&'){
   		printf("-myshell: syntax error near unexpected token `&&'\n");
   		return;
   	}
	process_amp(cmdline);
}

void process_amp(char *cmdline){
	//printf("process amp: %s\n", cmdline);
	const char *AMP = "&";
	int child_status;
	int i = 0;
	int n =0;

	strlen(cmdline);
	n = strlen(cmdline);

	char* pos_amp = strstr(cmdline, AMP);
	
	if(pos_amp==NULL) process_and_or(cmdline);
	else {
		while(pos_amp+1!=NULL&&*(pos_amp+1)=='&'){	
			if(pos_amp!=cmdline+n-2){
				pos_amp = strstr(pos_amp+2, AMP);
				if(pos_amp==NULL) break;
			}
				
			else{
				pos_amp = NULL;
				break;
			} 
		}

		if(pos_amp==NULL) process_and_or(cmdline);
		else{
				if(pos_amp == cmdline) {
				printf("-myshell: syntax error near unexpected token `&'\n");
				//return 1;
			}
			else if(pos_amp!=cmdline+n-1){
				*pos_amp = '\0';
				//should fork()
				pid_t pid = fork();
				if(pid > 0){			
					wait(&child_status);
					//return 
					process_amp(++pos_amp);
				}
				else{
					printf("+++++Run in background++++++\n");
					//return process_and_or(cmdline);
					int flag = process_and_or(cmdline);
					printf("++++Finish in background++++\n");
					exit(flag);
				}
			}
			else {
				*pos_amp = '\0';
				//should fork()
				pid_t pid = fork();
				if(pid > 0){			
					wait(&child_status);
					//return 0;
				}
				else{
					printf("+++++Run in background++++++\n");
					//return process_and_or(cmdline);
					int flag = process_and_or(cmdline);
					printf("++++Finish in background++++\n");
					exit(flag);
				}			
			}			
		}
	}

}

int process_and_or(char *cmdline){
   //printf("process AND OR: %s\n", cmdline);

   const char* AND = "&&";
   const char* OR = "||";

   char *token_a = strstr(cmdline, AND);
   char *token_b = strstr(cmdline, OR);
   int n = strlen(cmdline);

   strtrim(cmdline);
   

   if(token_a==cmdline || token_a==cmdline+n-2) {
   		printf("-myshell: syntax error near unexpected token `&&'\n");
   		return 1;
   }
   else if(token_b==cmdline || token_b==cmdline+n-2) {
   		printf("-myshell: syntax error near unexpected token `||'\n");
   		return 1;
   }
   else if(token_a!=NULL && token_b!=NULL){
      if(token_a > token_b){
         *token_b = '\0';
         *(token_b+1) = '\0';

         if(process_pipe(cmdline)>0) return process_and_or(token_b+2);
         else return 0;
      }
      else if (token_a < token_b){
         *token_a = '\0';
         *(token_a+1) = '\0';

         if(process_pipe(cmdline)==0) return process_and_or(token_a+2);
         else return 1;
      }
      else return 1;
   }
   else if(token_a!=NULL && token_b==NULL){
      *token_a = '\0';
      *(token_a+1) = '\0';

      if(process_pipe(cmdline)==0) return process_and_or(token_a+2);
      else return 1;
   }
   else if(token_a==NULL && token_b!=NULL){
      *token_b = '\0';
      *(token_b+1) = '\0';

      if(process_pipe(cmdline)>0) return process_and_or(token_b+2);
      else return 0;
   } 
   else return process_pipe(cmdline);
}

int process_pipe(char* cmdline){
	//printf("process pipe: %s\n", cmdline);
	char* pos_pip;
	int child_status;

	strtrim(cmdline);

	if(cmdline[strlen(cmdline)-1]=='|'||*cmdline=='|') {
		printf("-myshell: syntax error near unexpected token `|'\n");
		return 1;
	}
	else{
		pos_pip = strchr(cmdline, '|');
		if(pos_pip==NULL) return process_unit(cmdline);
		else{
			*pos_pip = '\0';
			int pfds[2];
			pipe(pfds);
			pid_t pid =fork();
			if(pid == 0){
				close(1);
				dup(pfds[1]);
				close(pfds[0]);
				exit(process_unit(cmdline));
			}
			else{
				wait(&child_status);
				if(child_status==0){
					int stdin_copy = dup(0);
					close(0);
					dup(pfds[0]);
					close(pfds[1]);
				
					int flag = process_pipe(++pos_pip);
					close(pfds[0]);
					dup2(stdin_copy, 0);
					close(stdin_copy);
					return flag;					
				}
				else return 1;

			}
		}
	}
}


int process_unit(char *cmdline)
{
	const char* EXIT = "exit";
	const char* CHILD = "child";
	const char* CD = "cd";
	const char* SPACE = " ";
	int child_status;
	pid_t child_pid;

	char* token;

	token = strtok(cmdline, SPACE);
	//if(token!=NULL){
		if(strcmp (token, EXIT)==0){
			exit(0);
		}
		else if(strcmp (token, CD)==0){
			token = strtok(NULL, SPACE);
			if(token==NULL) {
				chdir(getenv("HOME"));
				return 0;
			}
            else if(chdir(token)!=0) {
            	printf("-myshell: cd: %s: no such file or directory\n", token);
            	return 1;
            }
            else return 0;
		}
		else {
			if(find_file(token)){
				char *argv [ARG_MAX_NUM];
				int i = 0;
				int flag = 0;;

				while(token!=NULL){
					if(i<ARG_MAX){
						argv[i] = token;
					}
					else{
						printf("-myshell: %s: too many parameters\n", argv[0]);
						return 1;
					}
					/*As strtok is called in find_file, 
					  reset the string to the new start after the '\0' of token*/
					token = strtok(token+strlen(token)+1, SPACE);
					++i;
				}
				argv[i]=0;

				pid_t pid = fork();
				if(pid > 0){			
					wait(&child_status);
					return child_status;
				}
				else{
					flag = execvp(argv[0], argv);
					
					if(flag<0) {
						printf("-myshell: %s: %s\n", argv[0], strerror(errno));
						exit(1);
					}
					exit(0);
				} 
			}
			else return 1;
			
		}
	
}

int find_file(char* filename){
	char *paths = strdup(getenv("PATH"));
   char* path;
   const char* COLON = ":";
   int exist = 0;

   path = strtok(paths, COLON);

   strtrim(filename);
   while(path!=NULL){
      DIR *d;
      struct dirent *dir;
      d = opendir(path);
      if(d){
         while ((dir = readdir(d)) != NULL){
            if(!strcmp(filename, dir->d_name)) {
               exist = 1;
               break;
            }
         }
         closedir(d);
      }
      if(exist) break;
      path = strtok(NULL, COLON);
   }

   free(paths);

   if(exist){
      return 1;
   } 
   else {
      printf("-myshell: %s: command not found\n", filename);
      return 0;
   }
}

void strtrim(char *cmdline){
   int i = 0;
   while(cmdline[i]!='\0'){
      if(isspace(cmdline[i])){
         if(i==0){
            for(int j = 0; j<strlen(cmdline); ++j){
               cmdline[j] = cmdline[j+1];
            }
         }
         else if(isspace(cmdline[i+1])||i==0){
            for(int j = i+1; j<strlen(cmdline); ++j){
               cmdline[j] = cmdline[j+1];
            }
         }
         else{
            if(cmdline[i+1]=='\0'){
               cmdline[i] = '\0';
            }
            else ++i;
         } 
      }
      else ++i;
   }
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
	if(dir==NULL) dir = " ";

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
    while (i < n && isspace(cmdline[i]))
    {
        ++i;
    }
    if (i == n)
    {
        // Empty command
        return -1;
    }
    else strtrim(cmdline);

    //should handle the case where the cmdline ends with an operator


    return 0;
}
