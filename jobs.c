#include "jobs.h"
#include <stdlib.h> //for NULL

#define NUM_JOBS 100

struct Job {
    int jobNum; // job id
    bool isFree; // is the 
    pid_t jobPid;
    char* cmdName;
    time_t startTime;
    bool isStoped;
} Job;

Job* createTable() {
    // Initializing array of job pointers
    Job** jobsTable = (Job**)malloc(NUM_JOBS * sizeof(Job*));
    
    // Checking memeory allocation
    if (!jobsTable) return NULL;

    // Set all jobs as free and not stopped
    for (int i = 0; i < NUM_JOBS; i++){
        jobsTable[i].jobNum = i + 1;
        jobsTable[i].isFree = true;
        jobsTable[i].jobPid = -1;
        jobsTable[i].startTime = time(NULL); // Sets current time
        jobsTable[i].cmdName = NULL; 
        jobsTable[i].isStoped = false;
    }
    return jobsTable;
}

status checkJobs(Job** jobsTable){
    if (!jobsTable) return ERROR;
    else{
        for (int i = 0; i < NUM_JOBS; i++){
            if (jobsTable[i].isFree) continue;

            // Check if current job ended (exited / terminated)
            int status;
            pid_t result = waitpid(jobsTable[i].jobPid, &status, WUNTRACED 
                                                               | WNOHANG);

            if (result == 0) continue;           // still running
            else if (result == -1) return ERROR; // invalid PID
            else {
                if (WIFSTOPED(status)) continue; // process stopped
                if (WIFEXITED(status) || WIFSIGNALED(status)){ // exited / terminated
                    if (deleteJobs(jobsTable[i].jobNum, jobsTable) == ERROR){
                        return ERROR;
                    }
                } 
            }
        }
    }

    return SUCCESS;
}

// Assumption: pid is legal upon using function
status addJob(Job** jobsTable; pid_t jobPid, char* cmd){
    
    // Check if command or job table are null
    if (!cmd || !jobsTable) return ERROR;
    
    if (checkJobs(jobsTable) == ERROR) return ERROR;
    else {
        for (int i=0; i < NUM_JOBS; i++){
            if (!jobsTable[i].isFree) continue;
            else { // Found minimal job id that is free
                jobsTable[i].isFree = false;
                jobsTable[i].jobPid = jobPid;
                jobsTable[i].startTime = time(NULL); 
                jobsTable[i]->cmdName = (char*)malloc(strlen(cmd)*sizeof(char)+1);
                if (!jobsTable[i]->cmdName) return ERROR;
                strlcpy(jobsTable[i]->cmdName, cmd, strlen(cmd) + 1);
                
                break;
            }
        }
    }
   
   return SUCCESS;
}

void printJobs(Jobs** jobsTable) {
    for (int i = 0; i < NUM_JOBS; ) {
        if (!jobsTable[i].isFree) {
            printf("[%d] %s: %d %ld %s" ,
                     jobsTable[i].jobNum,
                     jobsTable[i]->cmdName,
                     jobsTable[i].jobPid,
                     (long)(timediff(jobsTable[i].startTime, time(NULL)),
                     (jobsTable[i].isStopped) ?  "Stopped"  : "" ));
        }
    }
}

int continueJob(int jobNum, Jobs** jobsTable) {
    jobsTable[jobNums]->isStoped = false;
}

int deleteJobs(int jobNum, Jobs** jobsTable) {
    if (!jobsTable[jobNum]->isFree) {
        return ERROR;
    }
    jobsTable[jobNum].isFree=true;
    jobsTable[jobNum].jobPis = -1;
    jobsTable[jobNum]->cmdName = NULL;
    jobsTable[jobNum].startTime = time(NULL);
    jobsTable[jobNum].isStopped = false;
    return SUCCESS;
}

// all jobs are terminated
void* destroyTable(Job* jobTable){

    for (int i=0; i < 100; i++){
        if (jobsTable[i] != NULL){
            if (jobsTable[i]->cmdName != NULL){
                free(jobsTable[i]->cmdName);
            }
        }
        free(jobsTable);
    }
}



