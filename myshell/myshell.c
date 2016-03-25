#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#define MAX_CMDLINE_LEN 256
#define ARG_MAX_NUM 256

/* Sirui Xie */
/* sxieab@ust.hk */
/* stuID: 20091029 */
/* COMP 4511 PA 2*/
/* function prototypes go here... */

void show_prompt();
int get_cmd_line(char *cmdline);
void strtrim(char *cmdline);
void process_cmd(char *cmdline);
void process_amp(char *cmdline);
int process_and_or(char *cmdline);
int process_pipe(char *cmdline);
int process_redirect(char *cmdline);
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
   		fprintf(stderr, "-myshell: syntax error near unexpected token `|'\n");
   		return;
   }
   else if(*cmdline=='&'&&*(cmdline+1)!='&'){
   		fprintf(stderr, "-myshell: syntax error near unexpected token `&'\n");
   		return;
   }
   else if (*cmdline=='&'&&*(cmdline+1)=='&'){
   		fprintf(stderr, "-myshell: syntax error near unexpected token `&&'\n");
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
				fprintf(stderr, "-myshell: syntax error near unexpected token `&'\n");
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

   strtrim(cmdline);
   int n = strlen(cmdline);
   

   if(token_a==cmdline || token_a==cmdline+n-2) {
   		fprintf(stderr, "-myshell: syntax error near unexpected token `&&'\n");
   		return 1;
   }
   else if(token_b==cmdline || token_b==cmdline+n-2) {
   		fprintf(stderr, "-myshell: syntax error near unexpected token `||'\n");
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
		fprintf(stderr, "-myshell: syntax error near unexpected token `|'\n");
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
	int in = 0;
	int out = 0;
	char* input;
	char* output;

	char* cmd = strdup(cmdline);

	token = strtok(cmd, SPACE);
	//if(token!=NULL){
		if(strcmp (token, EXIT)==0){
			free(cmd);
			exit(0);
		}
		else if(strcmp (token, CD)==0){
			token = strtok(NULL, SPACE);
			if(token==NULL) {
				chdir(getenv("HOME"));
				free(cmd);
				return 0;
			}
            else if(chdir(token)!=0) {
            	fprintf(stderr, "-myshell: cd: %s\n", strerror(errno));
            	free(cmd);
            	return 1;
            }
            else {
            	free(cmd);
            	return 0;
            }
		}
		else {
			if(strchr(token, '<')!=NULL) {in = 1;}
			if(strchr(token, '>')!=NULL) {out = 1;}

			if(in&&out){
				char* tmp_in = strchr(token, '<');
				char* tmp_out = strchr(token, '>');
				input = tmp_in+1;
				strtrim(input);
				output = tmp_out+1;
				strtrim(output);
				*tmp_in = '\0';
				*tmp_out = '\0';
			}
			else if(in){
				char* tmp_in = strchr(token, '<');
				input = tmp_in+1;
				strtrim(input);
				*tmp_in = '\0';
			}
			else if(out){
				char* tmp_out = strchr(token, '>');
				output = tmp_out+1;
				strtrim(output);
				*tmp_out = '\0';
			}

			if(find_file(token)){
				char *argv [ARG_MAX_NUM];
				int i = 0;
				int flag = 0;;

				if(strchr(cmdline, '<')!=NULL) {in = 1; }
				if(strchr(cmdline, '>')!=NULL) {out = 1; }

				while(token!=NULL && token-cmd<strlen(cmdline)){
					if(i<ARG_MAX){
						argv[i] = token;
						
						if(strchr(argv[i], '<')!=NULL) {break; }
						if(strchr(argv[i], '>')!=NULL) {break; }
					}
					else{
						fprintf(stderr, "-myshell: %s: too many parameters\n", argv[0]);
						return 1;
					}
					/*As strtok is called in find_file, 
					  reset the string to the new start after the '\0' of token*/
					token = strtok(token+strlen(token)+1, SPACE);
					++i;
				}
				argv[i]=0;

				if(in&&out){
               		char* tmp_in = strchr(cmdline, '<');
               		input = tmp_in+1;
               		strtrim(input);
               		for(int i=0; i< strlen(input); ++i){
                  		if(isspace(input[i])) input[i]='\0';
                  		break;
               		}
               		char* tmp_out = strchr(cmdline, '>');
               		output = strtok(tmp_out+1, SPACE);
               		strtrim(output);
               		*tmp_in = '\0';
               		*tmp_out = '\0';
               		*(tmp_out-1) = '\0';
               	}
				else if(in){
					char* tmp_in = strchr(cmdline, '<');
					input = strtok(tmp_in+1, SPACE);
					for(int i=0; i< strlen(input); ++i){
						if(isspace(input[i])) input[i]='\0';
						break;
					}
					strtrim(input);
					*tmp_in = '\0';
					
				}
				else if(out){
					char* tmp_out = strchr(cmdline, '>');
					output = strtok(tmp_out+1, SPACE);
					strtrim(output);
					*tmp_out = '\0';
				}

				pid_t pid = fork();
				if(pid > 0){			
					wait(&child_status);
					return child_status;
				}
				else{
					    if (in){
        					int fd0 = open(input, O_RDONLY);
        					if(fd0==-1){
        						fprintf(stderr, "-myshell: %s: No such file or directory\n", input);
								exit(1);
        					}
       						dup2(fd0, STDIN_FILENO);
        					close(fd0);
    					}

    					if (out){
    						fflush(stdout);
        					int fd1 = open(output,O_WRONLY|O_CREAT|O_TRUNC,0644);
        					dup2(fd1, STDOUT_FILENO);
        					close(fd1);
    					}

					flag = execvp(argv[0], argv);
					
					if(flag<0) {
						fprintf(stderr, "-myshell: %s: %s\n", argv[0], strerror(errno));
						free(cmd);
						exit(1);
					}
					free(cmd);
					exit(0);
				} 
			}
			else {
				free(cmd);
				return 1;
			}
			
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
   else if(strcmp(filename, "./")>0){return 1;}
   else{
      fprintf(stderr, "-myshell: %s: command not found\n", filename);
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
