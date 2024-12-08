#include "jobs.h"
#include <stdlib.h> //for NULL

Job** createTable() {
    // Initializing array of job pointers
    Job** jobsTable = (Job**)malloc(NUM_JOBS * sizeof(Job*));
    
    // Checking memeory allocation
    if (!jobsTable) return NULL;

    // Set all jobs as free and not stopped
    for (int i = 0; i < NUM_JOBS; i++){
        
        // Allocate memory for each job in array
        jobsTable[i] = (Job*)malloc(sizeof(Job));

        // Check allocation, if failed at any point, delete the table
        if (!jobsTable[i]){
            destroyTable(jobsTable);
            return NULL;
        }
        
        jobsTable[i]->jobNum = i + 1;
        jobsTable[i]->isFree = true;
        jobsTable[i]->jobPid = -1;
        jobsTable[i]->startTime = time(NULL); // Sets current time
        jobsTable[i]->cmdName = NULL; 
        jobsTable[i]->isStopped = false;
    }
    return jobsTable;
}

int checkJobs(Job** jobsTable){
    if (!jobsTable) return ERROR;
    else{
        for (int i = 0; i < NUM_JOBS; i++){
            if (jobsTable[i]->isFree) continue;

            // Check if current job ended (exited / terminated)
            int status;
            pid_t result = waitpid(jobsTable[i]->jobPid, &status, WUNTRACED 
                                                               | WNOHANG);

            if (result == 0) continue;           // still running
            else if (result == -1) return ERROR; // invalid PID
            else {
                if (WIFSTOPPED(status)) continue; // process stopped
                if (WIFEXITED(status) || WIFSIGNALED(status)){ // exited / terminated
                    if (deleteJob(jobsTable[i]->jobNum, jobsTable) == ERROR){
                        return ERROR;
                    }
                } 
            }
        }
    }

    return SUCCESS;
}

// Assumption: pid is legal upon using function
int addJob(Job** jobsTable, pid_t jobPid, char* cmd){
    
    // Check if command or job table are null
    if (!cmd || !jobsTable) return ERROR;
    
    if (checkJobs(jobsTable) == ERROR) return ERROR;
    
    // int status;
    // pid_t result = waitpid(jobPid, &status, WNOHANG | WUNTRACED);
    // bool isStopped = (result > 0 && WIFSTOPPED(status));

    // // <----------- what if is this? ------>
    // if (result <= 0 || !isStopped){
    //     return ERROR;
    // }
    // else {
    for (int i=0; i < NUM_JOBS; i++){
        if (!jobsTable[i]->isFree) continue;
        else { // Found minimal job id that is free
            jobsTable[i]->isFree = false;
            jobsTable[i]->jobPid = jobPid;
            jobsTable[i]->startTime = time(NULL); 
            jobsTable[i]->isStopped = isStopped;
            jobsTable[i]->cmdName = (char*)malloc(strlen(cmd)*sizeof(char)+1);
            if (!jobsTable[i]->cmdName) return ERROR;
            strncpy(jobsTable[i]->cmdName, cmd, strlen(cmd) + 1);
            jobsTable[i]->cmdName[strlen(cmd)] = '\0';
            
            break;
        }
    }
   // }
   
   return SUCCESS;
}

void printJob(Job** jobsTable, int idx) {
    //for (int i = 0; i < NUM_JOBS; i++) {
    if (!jobsTable[idx]->isFree) {
        printf("[%d] %s: %d %ld %s" ,
                    jobsTable[idx]->jobNum,
                    jobsTable[idx]->cmdName,
                    jobsTable[idx]->jobPid,
                    (long)(difftime(jobsTable[idx]->startTime, time(NULL))),
                    ((jobsTable[idx]->isStopped) ?  "Stopped" : "" ));
    }
    //}
}

void continueJob(int jobNum, Job** jobsTable) {
    jobsTable[jobNum - 1]->isStopped = false;
}

int deleteJobs(int jobNum, Job** jobsTable) {
    if (!jobsTable[jobNum - 1]->isFree) {
        return ERROR;
    }
    jobsTable[jobNum - 1]->isFree=true;
    jobsTable[jobNum - 1]->jobPid = -1;
    jobsTable[jobNum - 1]->cmdName = NULL;
    jobsTable[jobNum - 1]->startTime = time(NULL);
    jobsTable[jobNum - 1]->isStopped = false;
    return SUCCESS;
}

// all jobs are terminated
void destroyTable(Job** jobsTable){
    if (!jobsTable) return;

    for (int i = 0; i < NUM_JOBS; i++){
        if (jobsTable[i]){
            if (jobsTable[i]->cmdName) free(jobsTable[i]->cmdName);
            free(jobsTable[i]);
        }
        free(jobsTable);
    }
}



