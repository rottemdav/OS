// signals.c

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

    printf("smash: caught CTRL+C\n");
    
    // Check if the foreground process group isn't the shell
    if (fgProc != getpid()) {
        kill(-fgProc, SIGINT);
        //tcsetpgrp(STDIN_FILENO, getpgrp()); // Restore control to the shell
        printf("smash: process %d was killed\n", fgProc);
    }
}

// Signal handler for SIGSTP (CTRL+Z) signal stop
void sigtstpHandler(int sig){

    if (fgProc == getpid()){
        receivedSignal = 1; // Raise flag only if is trying to stop smash
    }

    printf("smash: caught CTRL+Z\n");
    
    // Check if the foreground process group isn't the shell
    if (fgProc != getpid()) {
        kill(-fgProc, SIGTSTP);
        printf("smash: process %d was stopped\n", fgProc);
    }
}

void installSignalHandlers() {
    struct sigaction sa_int, sa_stp;
    
    // Install SIGINT handler
    memset(&sa_int, 0, sizeof(struct sigaction)); // Zero out the structure
    sa_int.sa_handler = sigintHandler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0; // No special flags

    //sa_int.sa_flags = SA_RESTART; // Restart interrupted system calls
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
         perror("smash error: sigaction failed\n");
         exit(EXIT_FAILURE);
    }

    // Install SIGSTP handler
    memset(&sa_stp, 0, sizeof(struct sigaction)); // Zero out the structure
    sa_stp.sa_handler = sigtstpHandler;
    sigemptyset(&sa_stp.sa_mask);
    sa_stp.sa_flags = 0; // No special flags

    //sa_stp.sa_flags = SA_RESTART; // Restart interrupted system calls
    if (sigaction(SIGTSTP, &sa_stp, NULL) == -1){
        perror("smash error: sigaction failed\n");
        exit(EXIT_FAILURE);
    }
}