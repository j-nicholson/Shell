#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>

#define MAX_LINE 80

// Read args
char *read_input(void)
{
	char *line = NULL;
	size_t buf = 0;
	getline(&line, &buf, stdin);
	return line;
}

// Parse args
char **parse_args(char *args)
{
	int buff;
	int i = 0;
	char **parsers;

	buff = MAX_LINE;

	parsers = malloc(buff * (sizeof(char*) + sizeof(char*)));
	parsers[0] = strtok(args, " \n");
	while(parsers[i++] != NULL){
		parsers[i]= strtok(0," \n");
	}

	parsers[buff * (sizeof(parsers)+sizeof(char*))] = (char*)NULL;
	return parsers;
}

// Execute args
int execute_args(char **args){
	pid_t pid;
	pid_t ppid;
	int child_status;
	int i = 0;
	int count = 0;
	char* last;

	if(strcmp(args[0], "exit") == 0) {
		printf("Done\n");
		exit(EXIT_SUCCESS);
	}
	else if(strcmp(args[0], "cd") == 0) {
		if(args[1] == NULL){
			if(chdir(getenv("HOME")) != 0){
				perror("Can't process cd to home directory");
			}
		}
		else if(chdir(args[1]) != 0){
			perror("Can't process cd");
		}
	}
	else{
		while(args[i++] != NULL){
			++count;
		}
		last = args[count - 1];

		if(strcmp(last, "&") == 0)
		{
			args[count - 1] = NULL;
			pid = fork();
			if(pid < 0) {
				fprintf(stderr, "Command can not be invoked, ERROR: %d", errno);
			}
			else if(pid == 0) {
				setpgid(0, 0);
				if(execvp(args[0], args) == -1) {
					perror("Can't process command");
				}
				exit(EXIT_FAILURE);
			}
		}
		else{
			pid = fork();
			if(pid < 0) {
				fprintf(stderr, "Command can not be invoked, ERROR: %d", errno);
			}
			else if(pid == 0) {

				if(execvp(args[0], args) == -1) {
					perror("Can't process command");
				}
				exit(EXIT_FAILURE);
			}
			
			do {
				ppid = wait(&child_status);
			}while(ppid != pid);
		}
	}
	return 0;
}

int main(void){

	int should_run = 1;
	char **args;
	char *input;

	while(should_run) {
		printf("cmd>");

		// Read input
		input = read_input();
		// Parse arguments from input
		args = parse_args(input);
		// Execute parsed arguments
		execute_args(args);

		fflush(stdout);
		free(input);
		free(args);
	}
	return 0;
}
