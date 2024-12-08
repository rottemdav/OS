#ifndef __COMMANDS_H__
#define __COMMANDS_H__
/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h> //for NULL
#include <sys/stat.h>
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
typedef struct cmd {
	char* cmd;
	char* args[MAX_ARGS];
	char* cmdFull;
	int numArgs;
} Command;

// Will parse the command into sections
int parseCmd(char* line, Command* outCmd);

void freeCommand(Command* cmd);

// Will get a command after parsing and handle according to needs
int handleCmd(Command* cmd, Job** jobTable);

int chooseBuiltIn(Command* cmd, Job** jobsTable);

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

bool isBuiltInCmd(Command* cmd);

int maxJobNum(Job** jobsTable);

#endif //__COMMANDS_H__