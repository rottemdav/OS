#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include <fstream>
#include <pthread.h>
//#include <format> c++11 doesnt contain this library - need to check if later version is permitted

class Log {

    std::ofstream log_file;
    pthread_mutex_t log_lock;

    public:
        // Constructor
        Log(const std::string& file_name) : log_file(file_name) {} ;

        // Destructor
        ~Log();

        // Write to log
        void write_to_log(const std::string& message);

        // Print <<incorrect password>> message to log
        void print_inc_pass(const int atm_id, const int acc_id);

        // Print <<account not exist>> message to log
        void print_no_acc(const int atm_id, const int acc_id);
};

#endif // LOG_H