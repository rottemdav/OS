// signals.c
#define  _POSIX_C_SOURCE  200809L

#include "signals.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>

extern pid_t fgProc;
extern volatile sig_atomic_t sigintReceived;
extern volatile sig_atomic_t sigtstpReceived;

// Signal handler for SIGINT (CTRL+C) signal interrupt
void sigintHandler(int sig) {
    //ignore the signal of the foregound process is the smash
    printf("fgProc: %d, getpid(): %d", fgProc, getpid());
    const char *ctrlCMsg = "\nsmash: caught CTRL+C\n";
    write(STDOUT_FILENO, ctrlCMsg, strlen(ctrlCMsg));;

    //if (fgProc == getpid()) {
    //    return;
   // }
    
    // Check if the foreground process group isn't the shell
    if (fgProc != getpid()) {
        kill(-fgProc, SIGINT);
        char buffer[100];
        int len = snprintf(buffer, sizeof(buffer), "smash: process %d was killed\n", fgProc);
        write(STDOUT_FILENO, buffer, len);
    }
    sigintReceived = 1;
   
}

// Signal handler for SIGSTP (CTRL+Z) signal stop
void sigtstpHandler(int sig){
    //ignore the signal of the foregound process is the smash
    printf("fgProc: %d, getpid(): %d", fgProc, getpid());
    
    const char *ctrlCMsg = "\nsmash: caught CTRL+Z\n";
    write(STDOUT_FILENO, ctrlCMsg, strlen(ctrlCMsg));;
    
    if (fgProc == getpid()) {
        return;
    }
    
    // Check if the foreground process group isn't the shell
    if (fgProc != getpid()) {
        kill(-fgProc, SIGTSTP);
        char buffer[100];
        int len = snprintf(buffer, sizeof(buffer), "smash: process %d was killed\n", fgProc);
        write(STDOUT_FILENO, buffer, len);
    }
    sigtstpReceived = 1;
    //return;
}

void installSignalHandlers() {
    struct sigaction sa_int, sa_stp;
    
    // Install SIGINT handler
    sa_int.sa_handler = sigintHandler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART; // Restart interrupted system calls
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
         perror("smash error: kill failed\n");
         exit(EXIT_FAILURE);
    }

    // Install SIGSTP handler
    sa_stp.sa_handler = sigtstpHandler;
    sigemptyset(&sa_stp.sa_mask);
    sa_stp.sa_flags = SA_RESTART; // Restart interrupted system calls
    if (sigaction(SIGTSTP, &sa_stp, NULL) == -1){
        perror("smash error: kill failed\n");
        exit(EXIT_FAILURE);
    }
}


// saved prints
/*
printf("smash: process %d was stopped\n", fgProc);
//tcsetpgrp(STDIN_FILENO, getpgrp()); // Restore control to the shell
//char *msg = 
//write(STDOUT_FILENO, "try\n", 4);
//printf("smash: process %d was killed\n", fgProc);
*/