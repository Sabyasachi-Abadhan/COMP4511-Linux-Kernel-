#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_CMDLINE_LEN 256

/* function prototypes go here... */

void show_prompt();
int get_cmd_line(char *cmdline);
void process_cmd(char *cmdline);


/* The main function implementation */
int main()
{
	char cmdline[MAX_CMDLINE_LEN];
	
	while (1) 
	{
		show_prompt();
		if ( get_cmd_line(cmdline) == -1 )
			continue; /* empty line handling */
		
		process_cmd(cmdline);
	}
	return 0;
}


void process_cmd(char *cmdline)
{
	// printf("%s\n", cmdline);
	const char* EXIT = "exit";
	const char* CHILD = "child";
	const char* SPACE = ' ';
	const char* COMMA = ':';
	int child_status;
	pid_t child_pid;

	char* token;
	char cmd[5] = "     ";
	int t;


	token = strtok(cmdline, SPACE);
	if(token!=NULL){
		/*Find a file*/
		char *paths = getenv("PATH");
		char* path;

		path = strtok(paths, COMMA);

		while(path!=NULL){

    		path = strtok(NULL, COMMA);
		} 

		free(paths);

		token = strtok(cmdline, SPACE);
		while(token!=NULL){

		}
	}
	

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

	free(cwd);
	free(dir);

	printf("[%s] myshell> ", dir);
}

int get_cmd_line(char *cmdline) 
{
    int i;
    int n;
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
    return 0;
}
