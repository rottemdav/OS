#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include <fstream>
#include <pthread.h>

class Log {

    std::ofstream log_file;
    pthread_mutex_t log_lock;

    public:
        // Constructor
        Log(const std::string& file_name) : log_file(file_name);

        // Destructor
        ~Log();

        // Write to log
        void write_to_log(const std::string& message);

}

#endif // LOG_H