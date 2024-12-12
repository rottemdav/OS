// signals.c
#include "signals.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

extern pid_t fgProc;

// Signal handler for SIGINT (CTRL+C) signal interrupt
void sigintHandler(int sig) {
    printf("smash: caught CTRL+C\n");

    // Get the current foreground process group
    //pid_t fg_pid = tcgetpgrp(STDIN_FILENO);
    
    // Check if the foreground process group isn't the shell
    if (fgProc != getpid()) {
        kill(-fgProc, SIGINT);
        //tcsetpgrp(STDIN_FILENO, getpgrp()); // Restore control to the shell
        printf("smash: process %d was killed\n", fgProc);
    }
}

// Signal handler for SIGSTP (CTRL+Z) signal stop
void sigtstpHandler(int sig){
    printf("smash: caught CTRL+Z\n");

    // Get the current foreground process group
    //pid_t fg_pgid = tcgetpgrp(STDIN_FILENO);
    
    // Check if the foreground process group isn't the shell
    if (fgProc != getpid()) {
        kill(-fgProc, SIGTSTP);
        //tcsetpgrp(STDIN_FILENO, getpgrp()); // Restore control to the shell
        printf("smash: process %d was stopped\n", fgProc);
    }
}

void installSignalHandlers() {
    struct sigaction sa;
    
    // Install SIGINT handler
    sa.sa_handler = sigintHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Restart interrupted system calls
    if (sigaction(SIGINT, &sa, NULL) == -1) {
         perror("smash error: kill failed\n");
         exit(EXIT_FAILURE);
    }

    // Install SIGSTP handler
    sa.sa_handler = sigtstpHandler;
    if (sigaction(SIGTSTP, &sa, NULL) == -1){
        perror("smash error: kill failed\n");
        exit(EXIT_FAILURE);
    }
}