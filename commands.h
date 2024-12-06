#ifndef __COMMANDS_H__
#define __COMMANDS_H__
/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h> //for NULL
#include "signals.h"
#include "jobs.h"

#define MAX_LINE_SIZE 80
#define MAX_ARGS 20

/*=============================================================================
* error definitions
=============================================================================*/
enum ParsingStatus
{
	SUCCESS = 0,
	INVALID_COMMAND = 1,
	MEM_ALLOC_ERR = 2,
	
	//feel free to add more values here
};

/*=============================================================================
* global functions
=============================================================================*/
typedef struct cmd Command;

// Will parse the command into sections
int parseCommand(char* line, Command* outCmd);

void freeCommand(Command* cmd);

// Will get a command after parsing and handle according to needs
int handleCmd(Command* cmd, Job** jobTable);

int handleShowPid(Command* cmd);

int handlePwd(Command* cmd);

int handleCd(Command* cmd);

void handleJobs(Command* cmd);

void handleKill(Command* cmd);

void handleFg(Command* cmd);

void handleBg(Command* cmd);

void handleQuit(Command* cmd);

void handleDiff(Command* cmd);

void handleExternal(Command* cmd);

#endif //__COMMANDS_H__