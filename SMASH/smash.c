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

/*=============================================================================
* main function
=============================================================================*/
int main(int argc, char* argv[])
{
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
			Command* newCmd = (Command*)malloc(sizeof(Command));
			if (!newCmd) {
				perror("smash error: memory allocation unsuccessfull");
				return 0;
			}

			// logic
			
			
			if (newCmd != NULL){
				freeCommand(newCmd);
				free(newCmd);
				newCmd = NULL;
			}
		}

	
		int parseStat = parseCmd(_cmd, newCmd);
		//parsing new command
		if (parseStat != 0) {
			//printf("Status: %d\n",parseStat);
			if (newCmd != NULL){
				free(newCmd);
				newCmd = NULL;
			}
			continue;
		}
		
		//execute command
		handleCmd(newCmd, jobsTable);

		//initialize buffers for next command
		_line[0] = '\0';
		_cmd[0] = '\0';
		
		if (newCmd != NULL){
				freeCommand(newCmd);
				free(newCmd);
				newCmd = NULL;
		}
	}
	
	//the program should reach here only if error inside the while loop occured
	if (jobsTable){
		destroyTable(jobsTable);
		free(jobsTable);
		jobsTable = NULL;
	}
	return 0;
}
