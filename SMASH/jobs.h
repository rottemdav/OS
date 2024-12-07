#ifndef __JOBS_H__
#define __JOBS_H__
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

/*=============================================================================
* includes, defines, usings
=============================================================================*/
#define NUM_JOBS 100

enum status {
    ERROR = -1,
    SUCCESS = 0
} status;

// definition of the data structure that will hold the jobs 
typedef struct job Job;

/*=============================================================================
* global functions
=============================================================================*/

// allocate memory and initialize the table that will save the jobs' information
Job** createTable();

// Check that all jobs are up-to-date and working properly
status checkJobs(Job** jobsTable);

// deallocate the memory used for the table
void destroyTable(Job** jobsTable);

// add a new process that was switched to bg or stopped - as a new job
status addJob(Job** jobsTable; pid_t jobPid, char* cmd);

// prints the table content
void printJobs(Jobs** jobsTable);

// takes a stopped job and remove it to the background
void continueJob(int jobNum, Jobs** jobsTable);

// the process returning to the fg and needs to be deleted from the table
status deleteJob(int jobNum, Job** jobsTable); 

#endif //__JOBS_H__