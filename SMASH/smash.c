//smash.c

/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h>

#include "commands.h"
#include "signals.h" 

/*=============================================================================
* classes/structs declarations
=============================================================================*/

/*=============================================================================
* global variables & data structures
=============================================================================*/
char _line[MAX_LINE_SIZE];

// foreground process initialization 
pid_t fgProc = 0;

/*=============================================================================
* main function
=============================================================================*/
int main(int argc, char* argv[])
{
	fgProc = getpid();

	char _cmd[MAX_LINE_SIZE];
	Job** jobsTable = createTable();
	if (!jobsTable) return 1;

	installSignalHandlers();
	
	while(1)
	{
		printf("smash > ");
		fgets(_line, MAX_LINE_SIZE, stdin);
		//copies the lines written in the shell to the cmd variable
		strcpy(_cmd, _line);
		//buffering
		_cmd[strlen(_line) + 1] = '\0';

		compCmd** newCompCmdArray = (compCmd**)malloc(MAX_COMMANDS*sizeof(compCmd*));
;		if (!newCompCmdArray) {
			perror("smash error: memory allocation unsuccessfull");
			return 0;
		}
		for (int i = 0; i < MAX_COMMANDS; i++) {
    		newCompCmdArray[i] = NULL;	
		}	
		int numOfCommands = 0;

		// Check for failures in complex parse line function
		int parseLineStat = parseLine(_cmd, newCompCmdArray, &numOfCommands);
		if (parseLineStat == MEM_ALLOC_ERR){
			if (!newCompCmdArray){
				free(newCompCmdArray);
				newCompCmdArray = NULL;
			}
			perror("smash error: memory allocation unsuccessfull");
			return 0;
		} else if (parseLineStat == COMMAND_FAILED) continue; // freed all memory 

		// Arbitrary initializations 
		int statCmd = COMMAND_SUCCESS;

		// Iterate through the commands
		for (int i = 0; i < numOfCommands; i++){
			// Verify command even exist
			if (newCompCmdArray[i] == NULL){
				break;
			}
			//printf("Command[%d]: %s, type[%d] %d\n",i+1, newCompCmdArray[i]->line, i-1,(i > 0) ? newCompCmdArray[i-1]->type : 0);
			// Memory allocation for single command
			Command* newCmd = (Command*)malloc(sizeof(Command));
			if (!newCmd) {
				perror("smash error: memory allocation unsuccessfull");
				return 0;
			}

			// Check if command legal and parses command into newCmd
			int isCommandValid = parseCmd(newCompCmdArray[i]->line, newCmd);
			if (isCommandValid != COMMAND_SUCCESS) {
				if (newCmd != NULL){ // free memory of command in order to move on
					free(newCmd);
					newCmd = NULL;
				}
				statCmd = COMMAND_FAILED; // "remember" this command failed
				continue;
			} else {
				if (i == 0){
					// Execute command and save it's status
					statCmd = handleCmd(newCmd, jobsTable);
				} else {
					if ((newCompCmdArray[i-1]->type == COND_CMD) && 
														(statCmd == COMMAND_SUCCESS))
					{
						statCmd = handleCmd(newCmd, jobsTable);

					} else if ((newCompCmdArray[i-1]->type == COND_CMD) &&
														(statCmd != COMMAND_SUCCESS))
					{
						if (newCmd != NULL){
							freeCommand(newCmd);
							free(newCmd);
							newCmd = NULL;
						}
						break; // no need to execute command because the command
								  // before didn't executed properly
					} else {
						statCmd = handleCmd(newCmd, jobsTable);
					}
				}
				//printf("Status of [%d]: %d\n", i+1, statCmd); // check command return values print
				// free current command memory
				if (newCmd != NULL){
					freeCommand(newCmd);
					free(newCmd);
					newCmd = NULL;
				}
			}
		} // end of for loop (iterated through all the commands in line)

		// free command array memory
		if (newCompCmdArray != NULL){
			freeCommandsArray(newCompCmdArray,MAX_COMMANDS);
			free(newCompCmdArray);
			newCompCmdArray = NULL;
		}
		
		//initialize buffers for next command (IS NEEDED??)
		_line[0] = '\0';
		_cmd[0] = '\0';

	}
	
	//the program should reach here only if error inside the while loop occured
	if (jobsTable){
		destroyTable(jobsTable);
		free(jobsTable);
		jobsTable = NULL;
	}
	return 0;
}
