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

        // Check allocation, if failed at any point, delete the all previous jobs 
        if (!jobsTable[i]){
            destroyTable(jobsTable); // deallocate table memory
            free(jobsTable); // free table pointer
            jobsTable = NULL;
            return NULL;
        }
        
        // otherwise, initialize the job
        jobsTable[i]->jobNum = i + 1;
        jobsTable[i]->isFree = true;
        jobsTable[i]->jobPid = -1;
        jobsTable[i]->startTime = time(NULL); // Sets current time
        jobsTable[i]->cmdString = NULL; 
        jobsTable[i]->isStopped = false;
    }
    return jobsTable;
}

/**
 * @brief updates the job table, deletes a job from the table if terminated or exited
 * 
 * @param jobsTable pointer to the jobs table
 * @return SUCCESS or FAILURE
 */
int checkJobs(Job** jobsTable){
    if (!jobsTable) return ERROR;
    else{
        // Iterate all the jobs
        for (int i = 0; i < NUM_JOBS; i++){
            if (jobsTable[i]->isFree) continue;

            // Check if current job ended (exited / terminated)
            int status;
            pid_t result = waitpid(jobsTable[i]->jobPid, &status, WNOHANG 
                                                               | WUNTRACED);

            if (result == 0) continue; // process still running
            else if (result == -1){

                if (errno == ECHILD){ // no such child process
                    if (deleteJobs(jobsTable[i]->jobNum, jobsTable) == ERROR){
                        return ERROR;
                    }
                } else {
                    fprintf(stderr, "\n");
                    perror("smash error: waitpid failed");
                }
            }
            else {
                // Process state has changed
                if (WIFEXITED(status) || WIFSIGNALED(status)){ 
                    // Process has terminated
                    if (deleteJobs(jobsTable[i]->jobNum, jobsTable) == ERROR){
                        return ERROR;
                    }
                } else if (WIFSTOPPED(status)) {
                    jobsTable[i]->isStopped = true;
                }
            }
        }
    }
    return SUCCESS;
}

/**
 * @brief adds a job to the table of jobs given the table has room
 * 
 * @param jobsTable pointer to the jobs table
 * @param jobPid pid of the job given by the system
 * @param cmd the WHOLE command string given by the user
 * @return SUCCESS or FAILURE 
 */
int addJob(Job** jobsTable, pid_t jobPid, char* cmd){
    
    // Check if command or job table are null
    if (!cmd || !jobsTable) return ERROR;
    
    if (checkJobs(jobsTable) == ERROR) return ERROR;
    
    int status;
    pid_t result = waitpid(jobPid, &status, WNOHANG | WUNTRACED);
    bool isStopped = (result > 0 && WIFSTOPPED(status));

    for (int i=0; i < NUM_JOBS; i++){
        if (!jobsTable[i]->isFree) continue;
        else { // Found minimal job id that is free
            jobsTable[i]->isFree = false;
            jobsTable[i]->jobPid = jobPid;
            jobsTable[i]->startTime = time(NULL); 
            jobsTable[i]->isStopped = isStopped;
            jobsTable[i]->cmdString = (char*)malloc(strlen(cmd)*sizeof(char)+1);
            if (!jobsTable[i]->cmdString) return ERROR;
            strncpy(jobsTable[i]->cmdString, cmd, strlen(cmd) + 1);
            jobsTable[i]->cmdString[strlen(cmd)] = '\0';
            
            break;
        }
    }
   // }
   
   return SUCCESS;
}

/**
 * @brief prints a single job
 * 
 * @param jobsTable pointer to the job table
 * @param idx job id
 */
void printJob(Job** jobsTable, int idx) {
    if (!jobsTable[idx]->isFree) {
        time_t elapsedTime = time(NULL) - jobsTable[idx]->startTime;

        if (elapsedTime < 0) elapsedTime = 0;

        printf("[%d] %s: %d %ld secs %s\n" ,
                    jobsTable[idx]->jobNum,
                    jobsTable[idx]->cmdString,
                    jobsTable[idx]->jobPid,
                    //(long)(difftime(jobsTable[idx]->startTime, time(NULL))),
                    (long)elapsedTime,
                    ((jobsTable[idx]->isStopped) ?  "(stopped)" : "" ));
    }
}

void continueJob(int jobNum, Job** jobsTable) {
    jobsTable[jobNum - 1]->isStopped = false;
}

/**
 * @brief Deletes a job from the job table, if there is one
 * 
 * @param jobNum job id given by the addJob() function
 * @param jobsTable job table with all of the current jobs
 * @return SUCCESS or FAILURE
 */
int deleteJobs(int jobNum, Job** jobsTable) {
    // If a job is free we shouldn't be able to delete it.
    if (jobsTable[jobNum - 1]->isFree) return ERROR;
    
    jobsTable[jobNum - 1]->isFree=true;
    jobsTable[jobNum - 1]->jobPid = -1;
    jobsTable[jobNum - 1]->cmdString = NULL;
    jobsTable[jobNum - 1]->startTime = time(NULL);
    jobsTable[jobNum - 1]->isStopped = false;
    return SUCCESS;
}

/**
 * @brief free all the memory allocated for the job table from the inside out,
 *        first the string (if exists) then the job itself.
 *        Doesn't free jobsTable because it's an outside pointer.
 * 
 * @param jobsTable pointer to the jobs table.
 */
void destroyTable(Job** jobsTable){
    if (jobsTable == NULL) return;

    for (int i = 0; i < NUM_JOBS; i++){
        if (jobsTable[i] != NULL){

            // deallocating the command string (if exists) and sets to NULL
            if (jobsTable[i]->cmdString != NULL) {
                free(jobsTable[i]->cmdString);
                jobsTable[i]->cmdString = NULL;
            } 
            // deallocating the pointer to the job and sets to NULL
            free(jobsTable[i]);
            jobsTable[i] = NULL;
        }
    }
}



