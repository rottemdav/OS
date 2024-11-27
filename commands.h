#ifndef __COMMANDS_H__
#define __COMMANDS_H__
/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h> //for NULL

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

#endif //__COMMANDS_H__