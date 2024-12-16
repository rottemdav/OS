// signals.c
#define  _POSIX_C_SOURCE  200809L

#include "signals.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>

extern pid_t fgProc;
extern int receivedSignal;

// Signal handler for SIGINT (CTRL+C) signal interrupt
void sigintHandler(int sig) {
    
    if (fgProc == getpid()){
        receivedSignal = 1; // Raise flag only if trying to interrupt smash
    }

    const char *ctrlCMsg = "\nsmash: caught CTRL+C\n";
    write(STDOUT_FILENO, ctrlCMsg, strlen(ctrlCMsg));;
    
    // Check if the foreground process group isn't the shell
    if (fgProc != getpid()) {
        kill(-fgProc, SIGINT);
        char buffer[100];
        int len = snprintf(buffer, sizeof(buffer), "smash: process %d was killed\n", fgProc);
        write(STDOUT_FILENO, buffer, len);
    }
}

// Signal handler for SIGSTP (CTRL+Z) signal stop
void sigtstpHandler(int sig){

    if (fgProc == getpid()){
        receivedSignal = 1; // Raise flag only if is trying to stop smash
    }

    const char *ctrlZMsg = "\nsmash: caught CTRL+Z\n";
    write(STDOUT_FILENO, ctrlZMsg, strlen(ctrlZMsg));
    
    // Check if the foreground process group isn't the shell
    if (fgProc != getpid()) {
        kill(-fgProc, SIGTSTP);
        char buffer[100];
        int len = snprintf(buffer, sizeof(buffer), "smash: process %d was stopped\n", fgProc);
        write(STDOUT_FILENO, buffer, len);
    }
}

void installSignalHandlers() {
    struct sigaction sa_int, sa_stp;
    
    // Install SIGINT handler
    memset(&sa_int, 0, sizeof(struct sigaction)); // Zero out the structure
    sa_int.sa_handler = sigintHandler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0; // No special flags

    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
         perror("smash error: sigaction failed\n");
         exit(EXIT_FAILURE);
    }

    // Install SIGSTP handler
    memset(&sa_stp, 0, sizeof(struct sigaction)); // Zero out the structure
    sa_stp.sa_handler = sigtstpHandler;
    sigemptyset(&sa_stp.sa_mask);
    sa_stp.sa_flags = 0; // No special flags

    if (sigaction(SIGTSTP, &sa_stp, NULL) == -1){
        perror("smash error: sigaction failed\n");
        exit(EXIT_FAILURE);
    }
}
