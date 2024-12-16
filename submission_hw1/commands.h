#ifndef __COMMANDS_H__
#define __COMMANDS_H__
/*=============================================================================
* includes, defines, usings
=============================================================================*/
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h> //for NULL
#include <sys/stat.h>
#include <string.h>
#include "signals.h"
#include "jobs.h"

#define MAX_LINE_SIZE 80
#define MAX_ARGS 20
#define MAX_COMMANDS 26

/*=============================================================================
* error definitions
=============================================================================*/
enum cmdType {
	COND_CMD = 0,
	NOT_COND_CMD = 1,
	LAST = 2,
};

enum cmdStatus {
	COMMAND_SUCCESS = 0,
	INVALID_COMMAND = 1,
	MEM_ALLOC_ERR = 2,
	COMMAND_FAILED = 3,
	NEWLINE = 4,
	QUIT_CMD = 5,
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

typedef struct compCmd {
	char* line; // String
	int type; // Type of dependency on the previous function
} compCmd;

/**
 * @brief Parses a whole line and return array of commands strings if necessary
 * 
 * @param line pointer to user line input
 * @param commandsArray pointer to array of commands. Will be set as NULL on calling.
 * @param numCommands number of commands parsed out. input expected to be 0
 * @return cmdStatus
 */
int parseLine(char* line, compCmd** commandsArray, int* numCommands);

/**
 * @brief Deallocate the memory of the commands array
 * 
 * @param commandsArray pointer to commands array
 * @param count MAX_COMMANDS
 */
void freeCommandsArray(compCmd** commandsArray, int count);

int parseCmd(char* line, Command* outCmd);

void freeCommand(Command* cmd);

// Will get a command after parsing and handle according to needs
int handleCmd(Command* cmd, Job** jobsTable);

int chooseBuiltIn(Command* cmd, Job** jobsTable);

int handleShowPid(Command* cmd);

int handlePwd(Command* cmd);

int handleCd(Command* cmd);

int handleJobs(Command* cmd, Job** jobsTable);

int handleKill(Command* cmd, Job** jobsTable);

int handleFg(Command* cmd, Job** jobsTable);

int handleBg(Command* cmd, Job** jobTable);

int handleQuit(Command* cmd, Job** jobsTable);

int handleDiff(Command* cmd);

int handleExternal(Command* cmd, Job** jobsTable);

bool isBuiltInCmd(Command* cmd);

int maxJobNum(Job** jobsTable);

#endif //__COMMANDS_H__