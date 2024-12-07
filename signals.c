// signals.c
#include "signals.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

// Signal handler for SIGINT (CTRL+C) signal interrupt
void sigintHandler(int sig) {
    printf("\nsmash: caught CTRL+C");
    // Get the current foreground process group
    pid_t fg_pid = tcgetpgrp(STDIN_FILENO);
    
    if (fg_pid != getpid()) {
        kill(-fg_pid, SIGINT);
        printf("\nsmash: process %d was killed", fg_pid);
    }
}

// Signal handler for SIGSTP (CTRL+Z)
void sigtstpHandler(int sig){
    printf("\nsmash: caught CTRL+Z");

    // Get the current foreground process group
    pid_t fg_pid = tcgetpgrp(STDIN_FILENO);
    
    if (fg_pid != getpid()) {
        kill(-fg_pid, SIGSTP);
        printf("\nsmash: process %d was stopped", fg_pid);
    }
}

void installSignalHandlers() {
    struct sigaction sa;
    
    // Install SIGINT handler
    sa.sa_handler = sigintHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Restart interrupted system calls
    if (sigaction(SIGINT, &sa, NULL) == -1) {
         perror("\nsmash error: kill failed");
         exit(EXIT_FAILURE);
    }

    // Install SIGSTP handler
    sa.sa_handler = sigtstpHandler;
    if (sigaction(SIGSTP, &sa, NULL) == -1){
        perror("\nsmash error: kill failed");
        exit(EXIT_FAILURE);
    }
}