#ifndef __COMMANDS_H__
#define __COMMANDS_H__
/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h> //for NULL
#include <unistd.h> // -------- added by Rottem 06-12-24 ---------- //

#define MAX_LINE_SIZE 80
#define MAX_ARGS 20

/*=============================================================================
* error definitions
=============================================================================*/
enum ParsingError
{
	INVALID_COMMAND = 0,
	//feel free to add more values here
};

/*=============================================================================
* global functions
=============================================================================*/
int parseCommandExample(char* line);

// ------ added by Rottem 06-12-24 -------- //

int showPid(Command* cmd); // or int showPid(char* line, char* args[MAX_ARGS]);

int pwd(Command* cmd); // or int pwd(char* line, char * args[MAX_ARGS]);

#endif //__COMMANDS_H__