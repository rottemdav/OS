//commands.c
#include "commands.h"

typedef struct cmd {
	char* cmd;
	char* args[MAX_ARGS];
} Command;

// Function that parses command input and return a command struct 
 int parseCmd(char* line, Command* outCmd)
{
	char* delimiters = " \t\n"; //parsing should be done by spaces, tabs or newlines
	char* cmd = strtok(line, delimiters); //read strtok documentation - parses string by delimiters
	if(!cmd)
		return INVALID_COMMAND; //this means no tokens were found, most like since command is invalid
	
	char* args[MAX_ARGS];
	int numArgs = 0;
	args[0] = cmd; //first token before spaces/tabs/newlines should be command name
	for(int i = 1; i < MAX_ARGS; i++)
	{
		args[i] = strtok(NULL, delimiters); //first arg NULL -> keep tokenizing from previous call
		if(!args[i])
			break;
		numArgs++;
	}

	// // Initialize a new command
	// Command* newCmd = (Command*)malloc(sizeof(Command));
	// if (!newCmd) return NULL;
	
	outCmd->cmd = (char*)malloc(sizeof(char)*strlen(args[0])+1);
	if (!newCmd->cmd) return NULL;
	strlcpy()
	for (int i = 1; i < numArgs; i++){
		
	}

}


//example function for getting/setting return value of a process.
#include <unistd.h>
#include <sys/wait.h>
int processReturnValueExample()
{
	pid_t pid = fork();
	if(pid < 0)
	{
		perror("fork fail");
		exit(1);
	}
	else if(pid == 0) //child code
	{
		//do some work here - for example, execute an external command
		char* cmd = "/bin/ls";
		char* args[] = {"ls", "-l", NULL};
		execvp(cmd, args);
		//execvp never returns unless upon error
		perror("execvp fail");
		exit(1); //set nonzero exit code for father to read
	}
	else //father code
	{
		int status;
		waitpid(pid, &status, 0); //wait for child to finish and read exit code into status
		if(WIFEXITED(status)) //WIFEXITED determines if a child exited with exit()
		{
			int exitStatus = WEXITSTATUS(status);
			if(!exitStatus)
			{
				//exit status != 0, handle error	
			}
			else
			{
				//exit status == 0, handle success
			}
		}
	}

	return 0;
}