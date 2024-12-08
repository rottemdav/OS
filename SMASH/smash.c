//smash.c

/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h>
#include <string.h>

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
		Command* newCmd = (Command*)malloc(sizeof(Command));
		if (!newCmd) {
			perror("\nsmash error: memory allocation unsuccessfull");
			return 0;
		}

		//buffering
		_cmd[strlen(_line) + 1] = '\0';

		//parsing new command
		if (parseCmd(_cmd, newCmd) != 0 ) {
			free(newCmd);
			continue;
		}
		
		//execute command
		handleCmd(newCmd, jobsTable);

		//initialize buffers for next command
		_line[0] = '\0';
		_cmd[0] = '\0';

		free(newCmd);
	}
	
	//the program should reach here only if error inside the while loop occured
	destroyTable(jobsTable);
	return 0;
}
