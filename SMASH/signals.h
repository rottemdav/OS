#ifndef __SIGNALS_H__
#define __SIGNALS_H__

/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <signal.h>

extern pid_t fgProc;
extern int receivedSignal;

/*=============================================================================
* global functions
=============================================================================*/

// Function to install signal handlers
void installSignalHandlers();

// Signal handlers
void sigintHandler(int sig);
void sigtstpHandler(int sig);


#endif //__SIGNALS_H__