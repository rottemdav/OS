//commands.c
#include "commands.h"

typedef struct cmd {
	char* cmd;
	char* args[MAX_ARGS];
	int numArgs;
} Command;

char path[1024] = NULL;

// --------- commands managment functions ---------- //

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

	// Allocate memory and copy command string to outCmd
	outCmd->cmd = (char*)malloc(sizeof(char)*strlen(args[0]) + 1);
	if (!outCmd->cmd) return MEM_ALLOC_ERR;
	strlcpy(outCmd->cmd, args[0], strlen(args[0]) + 1);
	
	outCmd->numArgs = numArgs;

	// Allocate memory and copy command arguments to outCmd
	for (int i = 0; i < MAX_ARGS; i++){
		if (i == 0 || i >= numArgs) outCmd->args[i] = NULL;
		else {
			outCmd->args[i] = (char*)malloc(sizeof(char) * strlen(args[i]) + 1);
			if (!outCmd->args[i]) {
				freeCommand(outCmd);
				return MEM_ALLOC_ERR;
			}
			strlcpy(outCmd->args[i], args[i], strlen(args[i]) + 1);
		}
	}

	return SUCCESS;
}

void freeCommand(Command* cmd){
	free(cmd->cmd); // Free the command string
    for (int i = 0; i < MAX_ARGS; i++) {
        if (cmd->args[i]) {
            free(cmd->args[i]); // Free each argument
        }
    }
}

// ------------- buil-in command implementation functions ----------- //

int handleShowPid(Command* cmd) {
	//check for extra arguments
	if ( !cmd->args[1] ) {
		printf("smash error: showpid: expceted 0 arguments\n");
		return INVALID_COMMAND;
	}
	pid_t pid = getpid();
	printf("smash pid is %d\n", pid);

}

int handlePwd(Command* cmd) {
	//check for extra arguemtns 
	if ( !cmd->args[1] ) {
	printf("smash error: pwd: expceted 0 arguments\n");
	return INVALID_COMMAND;
	}

	if (getcwd(path, sizeof(path)) != NULL) {
		printf("%s\n", path);
		return SUCCESS;
	}
}

// 
int handleCmd(Command* cmd, Job** jobTable){
	
	if (!cmd || !jobTable) return INVALID_COMMAND;
	checkJobs(jobsTable);
	
	bool isBg = false;

	// Check if run in background and add to job table
	if (cmd->numArgs > 0 && strcmp(cmd->args[cmd->numArgs - 1], "%") == 0){
		isBg = true;
	}

	pid_t pid = fork();
	
	if (pid < 0) {
		perror("smash error: fork failed");
		exit(EXIT_FAILURE);
	} else if (pid == 0){
		
		if (isBg){
			// Detach from parent terminal
			setsid();
		}

		if (strcmp(cmd->cmd, "cd") == 0){
			handleCd(cmd);
		}
		else if (strcmp(cmd->cmd, "showpid") == 0){
			handleShowPid(cmd);
		}
		else if (strcmp(cmd->cmd, "pwd") == 0){
			handlePwd(cmd);
		}
		else if (strcmp(cmd->cmd, "jobs") == 0){
			handleJobs(cmd);
		}
		else if (strcmp(cmd->cmd, "kill") == 0){
			handleKill(cmd);
		}
		else if (strcmp(cmd->cmd, "fg") == 0){
			handleFg(cmd);
		}
		else if (strcmp(cmd->cmd, "bg") == 0){
			handleBg(cmd);
		}
		else if (strcmp(cmd->cmd, "quit") == 0){
			handleQuit(cmd);
		}
		else if (strcmp(cmd->cmd, "diff") == 0){
			handleDiff(cmd);
		}
		else {
			handleExternal(cmd);
		}

	} else {
		if (isBg) {
			addJob(jobTable, pid, cmd->cmd);
		} else {
			int status;
			waitpid(pid, &status, 0);
		}
	} 
	
	
	


}

int handleCd(Command* cmd) {
    // more than 1 argument check
	if (!cmd->cmd || cmd.numArgs>1) {
        printf("smash error: cd: old pwd not set")
		return INVALID_COMMAND;
	}

	char curr_path[1024];
    if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
        return INVALID_COMMAND;
    }

    // previous path check - return invalid if there's no prev path
	if (strcmp(cmd->args[1],"-") == 0) {
        if (strlen(path) == 0) { // the argument is "-" and there's no previous path			
            printf("smash error: old pwd not set")
            return INVALID_COMMAND;
        } else {
            if (chdir(curr_path) == 0) {
                strcpy(path, curr_path);
                return SUCCESS;
        }
    }

    // parent directory command
    if (strcmp(cmd->args[1],"..") == 0) {
            chdir(".."); // for now the function doesn't take care of a case when the parent directory is not accesible
        }
    }

    // none of the above - check for valid path and then switch to it
    if ((strchr(cmd.args[1], "/")) == NULL) {
        printf("smash error: cd: target directory does not exist");
        return INVALID_COMMAND;
    } else {
        if (chdir(cmd->args[1]) == 0) {
            strcpy(path, curr_path);
            return SUCCESS;
        }
    }
}

	// ------------- example from the original commands.c file ------------// 

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