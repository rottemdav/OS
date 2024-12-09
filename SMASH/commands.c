
//commands.c
#include "commands.h"
#include "jobs.h"

char path[1024] ="";

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
	if (!cmd) {
		if (outCmd->cmdFull){
			free(outCmd->cmdFull);
			outCmd->cmdFull = NULL;
		}
		return INVALID_COMMAND; 
	}

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
		free(outCmd->cmdFull);
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
	if (!cmd) return;

	if (cmd->cmd){
		free(cmd->cmd);
		cmd->cmd = NULL;
	}

	if (cmd->cmdFull){
		free(cmd->cmdFull);
		cmd->cmdFull = NULL;
	}
	
    for (int i = 0; i < MAX_ARGS; i++) {
        if (cmd->args[i]) {
            free(cmd->args[i]); // Free each argument
			cmd->args[i] = NULL;
        }
    }
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
				int cmdStatus = handleExternal(cmd, jobsTable);
				if (cmdStatus != COMMAND_SUCCESS) exit(1);

				exit(0);
			} else {
				// parent process
				addJob(jobsTable, pid, cmd->cmdFull);
				int status;
				
				// wait for child process to finish
				if (waitpid(pid, &status, 0) < 0){
					fprintf(stderr, "\n");
					perror("smash error: waitpid failed");
				}
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

			if (cmdStatus != COMMAND_SUCCESS) exit(1);

			exit(0);
		} else {
			// parent process
			addJob(jobsTable, pid, cmd->cmdFull);
		}
	}
	
	// Will free the command string and args, will still require main to free pointer
	freeCommand(cmd); 
	return COMMAND_SUCCESS;
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
	if ( cmd->args[1] ) {
		printf("smash error: pwd: expceted 0 arguments\n");
		return INVALID_COMMAND;
	}

	if (getcwd(path, sizeof(path)) != NULL) {
		printf("%s\n", path);
		return COMMAND_SUCCESS;
	} else {
		return COMMAND_FAILED;
	}
}

int handleCd(Command* cmd) {
    // more than 1 argument check
	if (!cmd->cmd || cmd->numArgs>1 || cmd->numArgs == 0) {
        printf("smash error: cd: expected 1 arguments\n");
		return INVALID_COMMAND;
	}

	char curr_path[1024];
    if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
        return INVALID_COMMAND;
	}
    // previous path check - return invalid if there's no prev path
	if (strcmp(cmd->args[1], "-") == 0) {
        if (strlen(path) == 0) { // the argument is "-" and there's no previous path			
            printf("smash error: old pwd not set\n");
            return INVALID_COMMAND;
		}
		
		// Switch to previous path
		if (chdir(path) == -1){
			return COMMAND_FAILED;
		}
		strncpy(path, curr_path, sizeof(path));
		return COMMAND_SUCCESS;
		// } else {
        //     if (chdir(path) == -1) {
		// 		return COMMAND_FAILED;
        // 	}
		// 	if (strcpy(path, curr_path) == 0) return COMMAND_SUCCESS;
		// 	return COMMAND_FAILED;
    	// }
	}

    // parent directory command
    if (strcmp(cmd->args[1], "..") == 0) {
            if(chdir("..") == -1) {; // for now the function doesn't take care of a case when the parent directory is not accesible
				return COMMAND_FAILED;
		    }
			if (strcpy(path, curr_path) == 0) return COMMAND_SUCCESS;
			return COMMAND_FAILED;
	}
    
    // none of the above - check for valid path and then switch to it
    if ((strchr(cmd->args[1], '/')) == NULL) {
        printf("smash error: cd: target directory does not exist\n");
        return INVALID_COMMAND;
    } else {
        if (chdir(cmd->args[1]) == -1) {
			printf("smash error: cd: target directory does not exist\n");
        	return COMMAND_FAILED; 
        }
		strcpy(path, curr_path);
        return COMMAND_SUCCESS;
    }
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

/**
 * @brief will return a stopped command to run in background.
 *		  Input: bg [job id] - continue job with job id in background
 * 		         bg - continue job with maximum job id in background
 * @param cmd pointer to command object by user
 * @param jobsTable pointer to job table 
 * @return 
 */
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
			printf("%s: %d\n", jobsTable[jobId - 1]->cmdString,
							   jobsTable[jobId - 1]->jobPid);
			
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
					printf("%s: %d\n", jobsTable[i]->cmdString,
							   		   jobsTable[i]->jobPid);
					
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
		printf("smash error: bg: there are no stopped jobs to resume\n");
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
	if(!cmd || cmd->numArgs <= 0 ) {
		printf("smash error: external: invalid command\n");
		return INVALID_COMMAND;
	} 
	// the program we're trying to executre doesn't exist in the current path
	// if (!fopen(cmd->args[0], "r")) { 
	if (access(cmd->cmd, X_OK) != 0){
		//perror("\nsmash error: external: cannot find program"); 
		printf("smash error: external: cannot find program\n");
	} else {
		
		if(execvp(cmd->args[0],cmd->args) == -1) {
			printf("smash error: external: invalid command\n");
			return COMMAND_FAILED;
		}
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
