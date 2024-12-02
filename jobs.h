#ifndef __JOBS_H__
#define __JOBS_H__
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

/*=============================================================================
* includes, defines, usings
=============================================================================*/

enum status {
    ERROR = -1,
    SUCCESS = 0
} status;

// definition of the data structure that will hold the jobs 
struct void* Job;

/*=============================================================================
* global functions
=============================================================================*/

// allocate memory and initialize the table that will save the jobs' information
Job* createTable();

// Check that all jobs are up-to-date and working properly
void checkJobs(Job** jobTable);

// deallocate the memory used for the table
void destroyTable(Job* jobTable);

// add a new process that was switched to bg or stopped - as a new job
void addJob(pid_t jobPid, char* cmd);

// prints the table content
void printJobs(Jobs** jobsTable);

// takes a stopped job and remove it to the background
void continueJob(int jobNum);

// the process returning to the fg and needs to be deleted from the table
int deleteJob(int jobNum, Job** jobsTable); 

#endif //__JOBS_H__