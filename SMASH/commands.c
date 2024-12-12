
//commands.c
#include "commands.h"
#include "jobs.h"

char prev_path[1024] ="";
//char curr_path[1024] = "";
extern pid_t fgProc;

// --------- commands managment functions ---------- //

int parseLine(char* line, compCmd** commandsArray, int* numCommands) {
	if (!line || !numCommands || !commandsArray) return INVALID_COMMAND;
	else {
		if (strlen(line) > MAX_LINE_SIZE) return INVALID_COMMAND;
	}
	
	// Memory allocation for each complex command object
	for (int i = 0; i < MAX_COMMANDS; i++) {
		commandsArray[i] = (compCmd*)malloc(sizeof(compCmd));
		if (commandsArray[i] == NULL) {
			freeCommandsArray(commandsArray, MAX_COMMANDS);
			return MEM_ALLOC_ERR;
		}
    	commandsArray[i]->line = NULL; // Initialize line to NULL
	}

	
	*numCommands = 0;
	int commandIndex = 0;
	
	char* start = line;
	char* end;
	const char* delim = ";&"; // delimiters

	while ((end = strpbrk(start, delim)) != NULL) {
		// strpbrk(start, delim) searh the string start for first apperance of delim
		// and sets end value to the that delimiter.
		
		if (*numCommands >= MAX_COMMANDS) {
			freeCommandsArray(commandsArray, MAX_COMMANDS);
			return COMMAND_FAILED;
		}

		char delimiter = *end; // Save the delimiter
		*end = '\0'; // Isolating the token with null-term char

		commandsArray[commandIndex]->line = 
									(char*)malloc(sizeof(char)*strlen(start) + 1);
		if (!commandsArray[commandIndex]->line){
			freeCommandsArray(commandsArray, MAX_COMMANDS);
			return MEM_ALLOC_ERR;
		}
		strncpy(commandsArray[commandIndex]->line, start, strlen(start));
		commandsArray[commandIndex]->line[strlen(start)] = '\0';

		// Set the type of the function (meaning how it connects to he following cmd)
		int type;
		
		if (commandIndex < (MAX_COMMANDS-1)){
			// 1-25 commands context classification
			if (delimiter == ';') type = NOT_COND_CMD;
			else if ((end + 1) != NULL){
				if (delimiter == '&' && *(end+1) == '&') type = COND_CMD;
			}
		} else { 
			// 25 command classification (won;t get here but verifies anyway)
			type = LAST;
		}
		commandsArray[commandIndex]->type = type;
		
		// finished adding a new command
		
		commandIndex++; // update the index will stop at 25
		(*numCommands)++; // update the counter wiill stop at 25 
		
		if (type == NOT_COND_CMD) start = end + 1; // Advance the start pointer.
		else start = end + 2; // Advance the start by 2
		 
	}

	// Handle the last token
	if (*start != '\0') {
		if (*numCommands >= MAX_COMMANDS) {
			freeCommandsArray(commandsArray, MAX_COMMANDS);
			return COMMAND_FAILED; // Too many commands
		}

		// Allocate memory for the last command
		commandsArray[*numCommands]->line = (char*)malloc(strlen(start) + 1);
		if (!commandsArray[*numCommands]->line) {
			freeCommandsArray(commandsArray, MAX_COMMANDS);
			return MEM_ALLOC_ERR; // Memory allocation failure
		}

		// Copy the last command into the allocated memory
		strncpy(commandsArray[*numCommands]->line, start,strlen(start) + 1);
		commandsArray[commandIndex]->line[strlen(start)] = '\0';
		// Set type of the last command
		commandsArray[*numCommands]->type = LAST;

		// Increment command count
		(*numCommands)++;
	}


	return COMMAND_SUCCESS;
	
}

void freeCommandsArray(compCmd** commandsArray, int count) {
    if (commandsArray == NULL) {
        return;
    }

    for (int i = 0; i < count; i++) {
        if (commandsArray[i] != NULL) {
            if (commandsArray[i]->line != NULL) {
                free(commandsArray[i]->line);
                commandsArray[i]->line = NULL;
            }
            free(commandsArray[i]);
            commandsArray[i] = NULL;
        }
    }
}


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

	// Handle newline command
	if (strcmp(line, "\n") == 0) {
		return NEWLINE;
	}

	// Remove the newline from user input and replace with null-terminated
	int len = strlen(line);
	if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

	// Copy the whole user input (full command)
	outCmd->cmdFull = (char*)malloc(sizeof(char)*strlen(line) + 1);
	if (!(outCmd->cmdFull)) return MEM_ALLOC_ERR; 
	strncpy(outCmd->cmdFull, line, strlen(line));
	outCmd->cmdFull[strlen(line)] = '\0';

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

	// Copy only the command name
	outCmd->cmd = (char*)malloc(sizeof(char)*strlen(args[0]) + 1);
	if (!outCmd->cmd){
		free(outCmd->cmd);
		outCmd->cmd = NULL;
		return MEM_ALLOC_ERR;
	} 
	strncpy(outCmd->cmd, args[0], strlen(args[0]));
	outCmd->cmd[strlen(args[0])] = '\0';

	// Set number of arguments of the user's command
	outCmd->numArgs = numArgs;

	// Allocate memory and copy command arguments to outCmd
	for (int i = 0; i < MAX_ARGS; i++){
		// First argument is the command itself
		if (i == 0) {
			outCmd->args[0] = (char*)malloc(sizeof(char) * strlen(args[0]) + 1);
			if (!outCmd->args[0]) {
				freeCommand(outCmd);
				return MEM_ALLOC_ERR;
			}
			strncpy(outCmd->args[0], args[0], strlen(args[0]));
			outCmd->args[i][strlen(args[0])] = '\0';
		}
		else {
			if (i <= numArgs){
				outCmd->args[i] = (char*)malloc(sizeof(char) * strlen(args[i]) + 1);
				if (!outCmd->args[i]) {
					freeCommand(outCmd);
					return MEM_ALLOC_ERR;
				}
				strncpy(outCmd->args[i], args[i], strlen(args[i]));
				outCmd->args[i][strlen(args[i])] = '\0';
			} else {
				outCmd->args[i] = NULL;
			}
		}
	}

	return COMMAND_SUCCESS;
}


void freeCommand(Command* cmd){
	if (cmd == NULL) {
		return;
	} else {
		// free arguments
		for (int i = 0; i < MAX_ARGS; i++) {
			if (cmd->args[i] != NULL) {
				free(cmd->args[i]); // Free each argument
				cmd->args[i] = NULL;
			}
    	}

		if (cmd->cmd != NULL){
			free(cmd->cmd);
			cmd->cmd = NULL;
		}

		if (cmd->cmdFull != NULL){
			free(cmd->cmdFull);
			cmd->cmdFull = NULL;
		}
	}
	return;
}

bool isBuiltInCmd(Command* cmd){
	// Check if the command is one of the built-in commands
	if (strcmp(cmd->cmd, "cd") == 0       ||
		strcmp(cmd->cmd, "showpid") == 0  ||
		strcmp(cmd->cmd, "pwd") == 0	  ||
		strcmp(cmd->cmd, "jobs") == 0	  ||
		strcmp(cmd->cmd, "kill") == 0	  ||
		strcmp(cmd->cmd, "fg") == 0		  ||
		strcmp(cmd->cmd, "bg") == 0 	  ||
		strcmp(cmd->cmd, "quit") == 0	  ||
		strcmp(cmd->cmd, "diff") == 0){ 
		return true; 
	} 
	
	return false;
}

int chooseBuiltIn(Command* cmd, Job** jobsTable){
	// Check input
	if (!cmd || !(cmd->cmd) || !jobsTable) return COMMAND_FAILED;

	// Chose between built-in commands 
	if (strcmp(cmd->cmd, "cd") == 0){
		return handleCd(cmd);
	}
	else if (strcmp(cmd->cmd, "showpid") == 0){
		return handleShowPid(cmd);
	}
	else if (strcmp(cmd->cmd, "pwd") == 0){
		return handlePwd(cmd);
	}
	else if (strcmp(cmd->cmd, "jobs") == 0){
		return handleJobs(cmd, jobsTable);
	}
	else if (strcmp(cmd->cmd, "kill") == 0){
		return handleKill(cmd, jobsTable);
	}
	else if (strcmp(cmd->cmd, "fg") == 0){
		return handleFg(cmd, jobsTable);
	}
	else if (strcmp(cmd->cmd, "bg") == 0){
		return handleBg(cmd, jobsTable);
	}
	else if (strcmp(cmd->cmd, "quit") == 0){
		return handleQuit(cmd, jobsTable);
	}
	else if (strcmp(cmd->cmd, "diff") == 0){
		return handleDiff(cmd);
	}

	return COMMAND_FAILED;
}

int handleCmd(Command* cmd, Job** jobsTable){
	
	// Check inputs
	if (!cmd || !jobsTable) return INVALID_COMMAND;
	
	// Update job table
	checkJobs(jobsTable);
	
	bool isBuiltIn = isBuiltInCmd(cmd);

	// Check if command should run in background, if so remove the % argument
	bool isBg = false;
	if (cmd->numArgs > 0 && strcmp(cmd->args[cmd->numArgs], "%") == 0){
		isBg = true;
		free(cmd->args[cmd->numArgs]);
		cmd->args[cmd->numArgs] = NULL;
		cmd->numArgs--;
	}

	int retVal = COMMAND_FAILED;

	// Check if command is one of the built-in commands
	
	/* Scenariouus by order:
		1. If not in background and built-in -> run built-in and return
		2. If not in background and external -> fork:
									child: runs external
									parent: waits for child to finish and return
		3. If in background and built-in -> fork:
									child: runs built-in
									parent: add to job table and wait, return at exit
		4. If in background and external -> fork
									child: runs external
									parent: add to job table and return 
	*/
	if (!isBg){ // not in background
		
		if (isBuiltIn) { // build-in commaand
			chooseBuiltIn(cmd, jobsTable);
		} else { // external command
			pid_t pid = fork();

			if (pid < 0){
				fprintf(stderr, "\n");
				perror("smash error: fork failed");
				exit(1);
			} else if (pid == 0){
				// child process
				setpgrp(); // Ensures that signals won't reach unless used kill
				
				// If the external command wasn't executed properly, it will use
				// exit(1), otherwise the execvp will exit
				if ((retVal = handleExternal(cmd, jobsTable)) != COMMAND_SUCCESS){
					exit(1);
				}
			} else {
				// parent process
				//setpgid(pid, pid); // Ensures the child process is in its own process group
				fgProc = pid;

				int status;
				// Wait for the child process to terminate or stop
				if (waitpid(pid, &status, WUNTRACED) == -1){
					perror("smash error: waitpid failed");
				}
				
				// Restore shell as foreground process
				fgProc = getpid();

				// Add process to the job table in case it was stopped
				if (WIFSTOPPED(status)) addJob(jobsTable, pid, cmd->cmdFull);
			}
		}
	} else { // --- in backrground ---
		pid_t pid = fork();

		if (pid < 0){
			fprintf(stderr, "\n");
			perror("smash error: fork failed");
			exit(1);
		} else if (pid == 0){
			// child process
			setpgrp(); // Ensures that signals won't reach unless used kill

			int cmdStatus;
			if (isBuiltIn){
				cmdStatus = chooseBuiltIn(cmd, jobsTable);
			} else {
				cmdStatus = handleExternal(cmd, jobsTable);
			}
			retVal = cmdStatus;
			if (cmdStatus != COMMAND_SUCCESS) exit(1);

		} else {
			setpgid(pid, pid);
			// parent process
			addJob(jobsTable, pid, cmd->cmdFull);
		}
	}
	
	// Will free the command string and args, will still require main to free pointer
	freeCommand(cmd); 
	return retVal;
}

// ------------- buil-in command implementation functions ----------- //

int handleShowPid(Command* cmd) {
	//check for extra arguments
	if ( cmd->args[1] ) {
		printf("smash error: showpid: expected 0 arguments\n");
		return INVALID_COMMAND;
	}
	pid_t pid = getpid();
	printf("smash pid is %d\n", pid);
	return COMMAND_SUCCESS;
}

int handlePwd(Command* cmd) {
	//check for extra arguemtns 
	char curr_path[1024];
	if ( cmd->args[1] ) {
		printf("smash error: pwd: expceted 0 arguments\n");
		return INVALID_COMMAND;
	}

	if (getcwd(curr_path, sizeof(curr_path)) != NULL) {
		printf("%s\n", curr_path);
		return COMMAND_SUCCESS;
	} else {
		return COMMAND_FAILED;
	}
}

int handleCd(Command* cmd) {
	char curr_path[1024];
	char temp[1024];            
    // more than 1 argument check
	if (!cmd->cmd || cmd->numArgs>1 || cmd->numArgs == 0) {
        printf("smash error: cd: expected 1 arguments\n");
		return INVALID_COMMAND;
	}

	//printf("cmd->args[0]: %s", cmd->args[0]);
    // previous path check - return invalid if there's no prev path
	if (strcmp(cmd->args[1],"-") == 0) {

        if (strlen(prev_path) == 0) { // the argument is "-" and there's no previous path			
            printf("smash error: old pwd not set\n");
            return INVALID_COMMAND;
        } 

		if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
		return INVALID_COMMAND;
		}

		//move to the previous path
		if (chdir(prev_path) == -1) {
			printf("fail1");
			return COMMAND_FAILED;
		}
		
		strcpy(temp, curr_path);

		if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
		return INVALID_COMMAND;
		}

		strcpy(prev_path, temp);
		
		return COMMAND_SUCCESS;
		
	}

    // parent directory command
    if (strcmp(cmd->args[1], "..") == 0) {

		if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
        			return INVALID_COMMAND;
			}

		strcpy (prev_path, curr_path);

        if(chdir("..") == -1) {; // for now the function doesn't take care of a case when the parent directory is not accesible
			return COMMAND_FAILED;
		}

		if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
				return INVALID_COMMAND;
		}
		return COMMAND_SUCCESS;
	}

    // none of the above - check for valid path and then switch to it
	if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
		return INVALID_COMMAND;
	}

	strcpy(prev_path, curr_path);

	if (chdir(cmd->args[1]) == -1) {
		printf("smash error: cd: target directory does not exist\n");
		//printf("hi \n");
		return COMMAND_FAILED; 
	}

	if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
		return INVALID_COMMAND;
	}

	return COMMAND_SUCCESS;
	}

int handleJobs(Command* cmd, Job** jobsTable) {
	if (cmd->numArgs>0) {
		printf("smash error: jobs: expected 0 arguments\n");
		return INVALID_COMMAND;
	}
	if (jobsTable) {
		for (int i = 0; i < NUM_JOBS; i++) {
			if (!jobsTable[i]->isFree) printJob(jobsTable, i);
		}
	}
	return COMMAND_SUCCESS;
}

int handleKill(Command* cmd, Job** jobsTable) {
	if (!cmd || !(cmd->cmd) || !(cmd->args[1]) || !(cmd->args[2])){
		printf("smash error: kill: invalid arguments\n");
		return COMMAND_FAILED;
	}

	// Check arguments
	if (cmd->numArgs != 2) { // check amount of arguments (command, signum, id)
		printf("smash error: kill: invalid arguments\n");
		return COMMAND_FAILED;
	}
	// parse signal num
	char* delim = "-";
	char* token;
	token = strtok(cmd->args[1], delim);
	
	if (token == NULL){
		printf("smash error: kill: invalid arguments\n");
		return COMMAND_FAILED;
	}

	int signum = atoi(token);
	if (signum < 1 || signum > 31) { // check if signal num is legal (1-31)
		printf("smash error: kill: invalid arguments\n");
		return COMMAND_FAILED;
	}

	// check if job exist
	int jobId = atoi(cmd->args[2]);

	if (jobId < 1 || jobId > NUM_JOBS){ // Check if job argument is valid
		printf("smash error: kill: invalid arguments\n");
		return COMMAND_FAILED;
	}

	// send signal if job exist on job table
	if (jobsTable[jobId - 1]->isFree) {
		//perror("\njob id %d does not exist", jobId);
		fprintf(stderr, "smash error: job id %d does not exist\n", jobId);
		return COMMAND_FAILED;
	} else {
		if  (kill(jobsTable[jobId - 1]->jobPid, signum) == -1) {
			fprintf(stderr, "\n");
			perror("smash error: kill failed");
			return COMMAND_FAILED;
		}
	}
	printf("signal %d was sent to pid %d\n",signum ,jobsTable[jobId - 1]->jobPid); 
	return COMMAND_SUCCESS;
}

int handleFg(Command* cmd, Job** jobsTable) {
	if (!cmd || !jobsTable) return MEM_ALLOC_ERR;
	if (!cmd->args[1] && maxJobNum(jobsTable) == 0 ) {
		//perror("\nsmash error: fg: jobs list is empty");
		printf("smash error: fg: jobs list is empty\n");
		return INVALID_COMMAND;
	}
	int jobId = atoi(cmd->args[1]);
	if (cmd->numArgs == 0) {
		//implement find_max_job function
		jobId = maxJobNum(jobsTable); //check this correctness
	} 
	//print to stdout the cmd and pid
	printf("%s %d\n", jobsTable[jobId]->cmdString, jobsTable[jobId]->jobNum);

	//send SIGCONT to the process to activate it again
	if (kill(jobsTable[jobId]->jobPid,SIGCONT) == -1 ) {
		fprintf(stderr, "\n");
		perror("smash error: kill failed");
		return COMMAND_FAILED;
	}

	//remove job from jobsTable
	deleteJobs(jobId, jobsTable);

	//smash waits for the process to finish
	int status;
	if (waitpid(jobsTable[jobId]->jobPid, &status, 0) == -1 ) {
		fprintf(stderr, "\n");
		perror("smash error: waitpid failed");
		return COMMAND_FAILED;
	}

	return COMMAND_SUCCESS;
}

int handleBg(Command* cmd, Job** jobsTable){
	// check arguments
	if (!cmd || !jobsTable || cmd->numArgs > 1) {
		printf("smash error: kill: invalid arguments\n");
		return COMMAND_FAILED;
	}

	if (cmd->args[1] != NULL){ // The user wants to stop a specific job
		int jobId = atoi(cmd->args[1]);
		if (jobId < 1 || jobId > NUM_JOBS){ // Check if job argument is valid
			printf("smash error: kill: invalid arguments\n");
			return COMMAND_FAILED;
		}

		// check if job exists
		if (jobsTable[jobId - 1]->isFree) {
			printf("smash error: bg: job id %d does not exist\n", jobId);
			return COMMAND_FAILED;
		} else {
			// check if job is stopped
			if (!(jobsTable[jobId - 1]->isStopped)) {
				printf("smash error: bg: job id %d is already in background\n", jobId);
				return COMMAND_FAILED;
			}
			
			// print the command and pid 
			printf("\n%s: %d", cmd->cmdFull, jobsTable[jobId - 1]->jobPid);
			
			// set as not stopped in table
			continueJob(jobId, jobsTable);

			// send signal to continue
			if (kill(jobsTable[jobId - 1]->jobPid, SIGCONT) == -1){
				fprintf(stderr, "\n");
				perror("smash error: kill failed");
				return COMMAND_FAILED;
			}
			
			return COMMAND_SUCCESS;
		}
	} else {
		for (int i = NUM_JOBS - 1; i >= 0; i--){ // Check for jobs from max
			if (jobsTable[i]->isFree) continue;
			else {
				if (jobsTable[i]->isStopped){ // if job is stopped
					
					// print command nad pid
					printf("%s: %d\n", cmd->cmdFull, jobsTable[i]->jobPid);
					
					// set as not stopped in table (i+1 because we send jobId (1-100))
					continueJob(i+1, jobsTable);
					
					// send signal
					if (kill(jobsTable[i]->jobPid, SIGCONT) == -1){
						fprintf(stderr, "\n");
						perror("smash error: kill failed");
						return COMMAND_FAILED;
					}

					return COMMAND_SUCCESS;
				}
			} 
		}
		printf("\nsmash error: bg: there are no stopped jobs to resume");
		return COMMAND_SUCCESS;
	}
}

int handleQuit(Command* cmd, Job** jobsTable) {
// ~!!!~ the prints for each job are in the same line

// if kill exists: kill all the jobs in ascending order
//for each job in the table:
for ( int i = 0; i < NUM_JOBS; i++ ) {
	if (!jobsTable[i]->isFree) {
		if (cmd->numArgs == 1) {
			// kill job with SIGKILL
			if (kill(jobsTable[i]->jobPid, SIGKILL) == -1 ) {
				return COMMAND_FAILED;
				}
		} else if (strcmp(cmd->args[1],"kill") == 0 && cmd->numArgs > 1) {
			// print job id and its command
			printf("[%d] %s", jobsTable[i]->jobNum, jobsTable[i]->cmdString);

			// send SIGTERM with message
			if (kill(jobsTable[i]->jobPid, SIGTERM) == -1) {
				return COMMAND_FAILED;
				
			} else {
				printf("sending SIGTERM... ");
				
				// wait 5 secs
				sleep(5);
				
				if (kill(jobsTable[i]->jobPid,0) == 0 ){ // process still running after 5 secs
				// send SIGKILL if not terminated
					if(kill(jobsTable[i]->jobPid, SIGKILL) == -1) {
						return COMMAND_FAILED; 
					} else {
							printf("sending SIGKILL... done");
					}
				} else { // process terminated within 5 secs
					jobsTable[i]->isFree = true;
					printf("done\n");
				}
			}
		} else {
			fprintf(stderr, "\n");
			perror("smash error:quit: unexpected arguments");
			return COMMAND_FAILED;	
		}
	}
} 

// smash process waits for all of its childern to terminate.
int status;
int term_status = waitpid(-1, &status, 0);
if (term_status == -1 ) { //all childern process have been terminated
	destroyTable(jobsTable);
	exit(0);
	return SUCCESS;
	}
return INVALID_COMMAND;
}


int handleDiff(Command* cmd){
	// Check arguments
	if (!cmd) return INVALID_COMMAND;
	
	if (cmd->numArgs > 2 || !(cmd->args[1]) || !(cmd->args[2])) {
		printf("smash error: diff: expected 2 arguments\n");
	}
	
	// Check if path exist
	struct stat stat1, stat2;
	char* path1 = cmd->args[1];
	char* path2 = cmd->args[2];
	
	if (stat(path1, &stat1) != 0 || stat(path2, &stat2) != 0){
		printf("nsmash error: diff: expected valid paths for files\n");
		return COMMAND_FAILED;
	}

	// Check if both paths are files and not directories
	if (!S_ISREG(stat1.st_mode) || !S_ISREG(stat2.st_mode)) {
		printf("smash error: diff: paths are not files\n");
		return COMMAND_FAILED;
	}

	FILE *f1 = fopen(path1, "rb");
	FILE *f2 = fopen(path2, "rb");

	if (f1 == NULL || f2 == NULL) {
		if (f1) fclose(f1);
		if (f2) fclose(f2);
		fprintf(stderr, "\n");
		perror("smash error: fopen failed");
		return COMMAND_FAILED;
	}

	int c1, c2;
	while (1){
		c1 = fgetc(f1);
		c2 = fgetc(f2);

		// In the case two letters are different
		if (c1 != c2) {
			printf("1\n");
			break;
		}

		// In case we reached the end of file (successfully)
		if (c1 == EOF && c2 == EOF){
			printf("0\n");
			break;
		}
	}

	// Close the files
	fclose(f1);
	fclose(f2);

	return COMMAND_SUCCESS;
}	

int handleExternal(Command* cmd, Job** jobsTable) {
	if(!cmd || cmd->args[0] == NULL) {
		fprintf(stderr, "smash error: external: invalid command\n");
		return INVALID_COMMAND;
	} 

	// the program we're trying to executre doesn't exist in the current path
	if(execvp(cmd->args[0],cmd->args) == -1) {
		printf("smash error: external: invalid command\n");
		return COMMAND_FAILED;	
	}

	return COMMAND_SUCCESS;
}


int maxJobNum(Job** jobsTable) {
	if (!jobsTable) return MEM_ALLOC_ERR;
	int maxJobNum = 0;
	for (int i = 0; i < NUM_JOBS; i++) {
		if (!jobsTable[i]->isFree) {
			if (jobsTable[i]->jobNum > maxJobNum) {
				maxJobNum = jobsTable[i]->jobNum;
			}
		}
	}
	return maxJobNum;
}
