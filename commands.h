#ifndef __COMMANDS_H__
#define __COMMANDS_H__
/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h> //for NULL
#include "signals.h"

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
typedef struct cmd Command;

// Will parse the command into sections
int parseCommand(char* line, Command* outCmd);

// Will get a command after parsing and handle according to needs
void handleCmd(Command* command);

void handleShowPid();

void handlePwd();

void handleCd(char* path);

void handleJobs();

void handleKill(int sig, int jobId);

void handleFg(int jobId);

void handleBg(int jobId);

void handleQuit(bool kill);

void handleDiff(char* path1, char* path2);

void handleExternal(Command cmd);

#endif //__COMMANDS_H__