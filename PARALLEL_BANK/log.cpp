#include "log.hpp"

Log::Log(const std::string& file_name) : log_file(file_name) {
    // Initialize mutex
    pthread_mutex_init(&log_lock, nullptr);

    // Open the file
    log_file.open(file_name, std::ios::out);

    // Check if the log file created successfully 
    if (!log_file.is_open()) {
        perror("Bank error: fopen failed");
    }

    // Close the file
    log_file.close(); 
}

Log::~Log() {
    // Destroy mutex
    pthread_mutex_destroy(&log_lock);
    
    // Close file
    log_file.close();
}

void write_to_log(const std::string& message) {
    // Lock log
    pthread_mutex_lock(&log_lock);
    
    // Open log file 
    std::ofstream log_file("log.txt", std::ios::out | std::ios::app);
    
    // Check if the log file created successfully 
    if (!log_file.is_open()) {
        perror("Bank error: fopen failed");
    }

    // Write to log
    log_file << message << std::endl;
    
    // Close log file
    log_file.close();

    // Unlock log
    pthread_mutex_unlock(&log_lock);
}

void print_inc_pass(const int atm_id, const int acc_id){
    // Format message
    std::string failure = std::format("Error {}: Your transaction failed -
        password for account id {} is incorrect", atm_id, id);   
    
    // Write to log
    log->write_to_log(failure); 
}

void print_no_acc(const int atm_id, const int acc_id){
    // Format message
    std::string not_exist = std::format("Error {}: Your transaction failed -
                    account id {} does not exist", atm_id, id);
    
    // Write to log
    log->write_to_log(not_exist);
}