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
enum cmdStatus
{
	SUCCESS = 0,
	INVALID_COMMAND = 1,
	MEM_ALLOC_ERR = 2,
	COMMAND_FAILED = 3,
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

int handleJobs(Command* cmd, Job** jobTable);

int handleKill(Command* cmd, Job** jobTable);

int handleFg(Command* cmd, Job** jobsTable);

int handleBg(Command* cmd, Job** jobTable);

int handleQuit(Command* cmd, Job** jobsTable);

int handleDiff(Command* cmd);

int handleExternal(Command* cmd);

#endif //__COMMANDS_H__