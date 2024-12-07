
//commands.c
#include "commands.h"
#include "jobs.h"

typedef struct cmd {
	char* cmd;
	char* args[MAX_ARGS];
	char* cmdFull;
	int numArgs;
} Command;

char path[1024];

// --------- commands managment functions ---------- //

// Function that parses command input and return a command struct 
 int parseCmd(char* line, Command* outCmd)
{	
	// ----------- Check inputs --------------- // 
	// Check the given line string
	if (!line) return INVALID_COMMAND;
	else {
		if (strlen(line) > MAX_LINE_SIZE) return INVALID_COMMAND;
	}
	// Check that outCmd pointer is not null
	if (!outCmd) return MEM_ALLOC_ERR;

	// ----------- Parsing command  --------------- //
	char* delimiters = " \t\n"; //parsing should be done by spaces, tabs or newlines
	char* cmd = strtok(line, delimiters); //read strtok documentation - parses string by delimiters
	
	//this means no tokens were found, most like since command is invalid
	if (!cmd) return INVALID_COMMAND; 

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

	// ----------- Returning command --------------- //
	// Allocate memory and copy command string to outCmd

	// Copy the whole user input (full command)
	outCmd->cmdFull = (char*)malloc(sizeof(char)*strlen(line) + 1);
	if (!(outCmd->cmdFull)) return MEM_ALLOC_ERR;
	strlcpy(outCmd->cmdFull, line);

	// Copy only the command name
	outCmd->cmd = (char*)malloc(sizeof(char)*strlen(args[0]) + 1);
	if (!outCmd->cmd) return MEM_ALLOC_ERR;
	strlcpy(outCmd->cmd, args[0], strlen(args[0]) + 1);
	
	// Set number of arguments of the user's command
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
	if (!cmd) return;
	free(cmd->cmd); // Free the command string
	free(cmd->cmdFull); // Free the full commnad string
    for (int i = 0; i < MAX_ARGS; i++) {
        if (cmd->args[i]) {
            free(cmd->args[i]); // Free each argument
        }
    }
}


int handleCmd(Command* cmd, Job** jobsTable){
	
	if (!cmd || !jobsTable) return INVALID_COMMAND;
	checkJobs(jobsTable);
	
	bool isBg = false;
	
	// Check if run in background and add to job table
	if (cmd->numArgs > 0 && strcmp(cmd->args[cmd->numArgs - 1], "%") == 0){
		isBg = true;
	}

	pid_t pid = fork();
	
	if (pid < 0) {
		perror("\nsmash error: fork failed");
		exit(EXIT_FAILURE);
	} else if (pid == 0){
		// Return status to check if the desired command was succesfull or not
		int status; 

		if (isBg){
			// Create a new process group for the background process
			setpgrp();
		} else {
			// Ensure the foreground process is in the same group as the parent
        	setpgid(0, getppid());
		}

		if (strcmp(cmd->cmd, "cd") == 0){
			status = handleCd(cmd);
		}
		else if (strcmp(cmd->cmd, "showpid") == 0){
			status = handleShowPid(cmd);
		}
		else if (strcmp(cmd->cmd, "pwd") == 0){
			status = handlePwd(cmd);
		}
		else if (strcmp(cmd->cmd, "jobs") == 0){
			status = handleJobs(cmd);
		}
		else if (strcmp(cmd->cmd, "kill") == 0){
			status = handleKill(cmd, jobsTable);
		}
		else if (strcmp(cmd->cmd, "fg") == 0){
			status = handleFg(cmd);
		}
		else if (strcmp(cmd->cmd, "bg") == 0){
			status = handleBg(cmd, jobsTable);
		}
		else if (strcmp(cmd->cmd, "quit") == 0){
			status = handleQuit(cmd);
		}
		else if (strcmp(cmd->cmd, "diff") == 0){
			status = handleDiff(cmd);
		}
		else {
			status = handleExternal(cmd);
		}

		if (status != SUCCESS){
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);

	} else {
		if (isBg) {
			addJob(jobsTable, pid, cmd->cmd);
		} else {
			// Foreground process: Set terminal control to the child process
        	tcsetpgrp(STDIN_FILENO, pid);

			int status;
			pid_t fg_pid = waitpid(pid, &status, WUNTRACED);

			if (WIFSTOPPED(status)) addJob(jobsTable, fg_pid, cmd->cmd);

			// Restore terminal control to the shell
			tcsetpgrp(STDIN_FILENO, getpid());
		}
	}
	
	// Will free the command string and args, will still require main to free pointer
	freeCommand(cmd); 
	return SUCCESS;
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

int handleCd(Command* cmd) {
    // more than 1 argument check
	if (!cmd->cmd || cmd->numArgs>1) {
        printf("smash error: cd: old pwd not set");
		return INVALID_COMMAND;
	}

	char curr_path[1024];
    if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
        return INVALID_COMMAND;
    }

    // previous path check - return invalid if there's no prev path
	if (strcmp(cmd->args[1],"-") == 0) {
        if (strlen(path) == 0) { // the argument is "-" and there's no previous path			
            printf("smash error: old pwd not set");
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
    if ((strchr(cmd->args[1], "/")) == NULL) {
        printf("smash error: cd: target directory does not exist");
        return INVALID_COMMAND;
    } else {
        if (chdir(cmd->args[1]) == 0) {
            strcpy(path, curr_path);
            return SUCCESS;
        }
    }
}	

int handleJobs(Command* cmd, Job** jobTable) {
	if (cmd->numArgs>0) {
		perror("\nsmash error: jobs: expected 0 arguments");
	}
	for (int i = 0; i < NUM_JOBS; i++) {
		printJob(jobTable, i);

	}
}

int handleKill(Command* cmd, Job** jobsTable) {
	if (!cmd || !(cmd->cmd) || !(cmd->args[1]) || !(cmd->args[2])){
		perror("\nsmash error: kill: invalid arguments");
		return COMMAND_FAILED;
	}

	// Check arguments
	if (cmd->numArgs != 3) { // check amount of arguments (command, signum, id)
		perror("\nsmash error: kill: invalid arguments");
		return COMMAND_FAILED;
	}
	// parse signal num
	char* delim = "-";
	char* token;
	token = strtok(cmd->args[1], delim);
	
	if (token == NULL){
		perror("\nsmash error: kill: invalid arguments");
		return COMMAND_FAILED;
	}

	int signum = atoi(token);
	if (signum < 1 || signum > 31) { // check if signal num is legal (1-31)
		perror("\nsmash error: kill: invalid arguments");
		return COMMAND_FAILED;
	}

	// check if job exist
	int jobId = atoi(cmd->args[2]);

	if (jobId < 1 || jobId > NUM_JOBS){ // Check if job argument is valid
		perror("\nsmash error: kill: invalid arguments");
		return COMMAND_FAILED;
	}

	// send signal if job exist on job table
	if (jobsTable[jobId - 1]->isFree) {
		perror("job id %d does not exist", jobId);
		return COMMAND_FAILED;
	} else {
		if  (kill(jobsTable[jobId - 1]->jobPid, signum) == -1) {
			perror("\nsmash error: kill failed");
			return COMMAND_FAILED;
		}
	}
	printf("\nsignal %d was sent to pid %d",signum ,jobsTable[jobId - 1]->jobPid); // check if this correct
	return SUCCESS;
}

int handleFg(Command* cmd, Job** jobsTable) {
	if (!jobsTable && !cmd->args[1]) {
		perror("\n smash error: fg: jobs list is empty");
	}
	int jobId = cmd->args[1];
	if (cmd->numArgs == 0) {
		//implement find_max_job function
		jobId = maxJob(jobsTable); //check this correctness
	} 
	//print to stdout the cmd and pid
	printf("%s %d", jobsTable[jobId]->cmdName, jobsTable[jobId]->jobNum);

	//send SIGCONT to the process to activate it again
	if (kill(jobsTable[jobId]->jobPid,SIGCONT) == -1 ) {
		perror("SIGCONT failed");
		return COMMAND_FAILED;
	}

	//remove job from jobsTable
	deleteJob(jobId, jobsTable);

	//smash waits for the process to finish
	int status;
	if (waitpid(jobsTable[jobId]->jobPid, &status, 0) == -1 ) {
		perror("Waiting error");
		return COMMAND_FAILED;
	}

	
}

int handleBg(Command* cmd, Job** jobTable){
	// check arguments
	if (!cmd || !jobTable || cmd->numArgs > 2) {
		perror("\nsmash error: kill: invalid arguments");
		return COMMAND_FAILED;
	}

	if (cmd->args[1] != NULL){ // The user wants to stop a specific job
		int jobId = atoi(cmd->args[1]);
		if (jobId < 1 || jobId > NUM_JOBS){ // Check if job argument is valid
			perror("\nsmash error: kill: invalid arguments");
			return COMMAND_FAILED;
		}

		// check if job exists
		if (jobTable[jobId - 1]->isFree) {
			perror("\nsmash error: bg: job id %d does not exist", jobId);
			return COMMAND_FAILED;
		} else {
			// check if job is stopped
			if (!(jobTable[jobId - 1]->isStopped)) {
				perror("\nsmash error: bg: job id %d is already in background", jobId);
				return COMMAND_FAILED;
			}
			
			// print the command and pid 
			printf("\n%s: %d", cmd->cmdFull, jobTable[jobId - 1]->jobPid);
			
			// set as not stopped in table
			continueJob(jobId, jobTable);

			// send signal to continue
			if (kill(jobTable[jobId - 1]->jobPid, SIGCONT) == -1){
				perror("\nsmash error: kill failed");
				return COMMAND_FAILED;
			}
			
			return SUCCESS;
		}
	} else {
		for (int i = NUM_JOBS - 1; i >= 0; i--){ // Check for jobs from max
			if (jobTable[i]->isFree) continue;
			else {
				if (jobTable[i]->isStopped){ // if job is stopped
					
					// print command nad pid
					printf("\n%s: %d", cmd->cmdFull, jobTable[i]->jobPid);
					
					// set as not stopped in table (i+1 because we send jobId (1-100))
					continueJob(i+1, jobTable);
					
					// send signal
					if (kill(jobTable[i]->jobPid, SIGCONT) == -1){
						perror("\nsmash error: kill failed");
						return COMMAND_FAILED;
					}

					return SUCCESS;
				}
			} 
		}
		perror("\nsmash error: bg: there are no stopped jobs to resume");
		return SUCCESS;
	}
}

int handleQuit(Command* cmd, Job** jobsTable) {
	// if kill exists: kill all the jobs in ascending order

	//for each job in the table:
	//print job id and its command

	//send SIGTERM with message

	//if the process killed *up to* 5 secs - print done

	// if the process is not killed within 5 secs - send SIGKILL with message

	// ~!!!~ the prints for each job are in the same line

	
}


int handleDiff(Command* cmd){
	// Check arguments

	// Check if path exist

	// Check if file exists in path

}	// ------------- example from the original commands.c file ------------// 

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
