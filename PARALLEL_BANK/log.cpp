#include "log.hpp"

// Constructor
Log::Log(const std::string& file_name) {
    // Initialize mutex
    if (pthread_mutex_init(&log_lock, nullptr) != 0) {
        perror("Bank error: pthread_mutex_init failed");
    }

    // Open the file
    log_file.open(file_name, std::ios::out);

    // Check if the log file created successfully 
    if (!log_file.is_open()) {
        perror("Bank error: fopen failed");
    }

    // Close the file
    log_file.close(); 
    
}

// Destructor
Log::~Log() {
    // Destroy mutex
    if (pthread_mutex_destroy(&log_lock) != 0) {
        perror("Bank error: pthread_mutex_destroy failed");
    }
    
    // Close file
    log_file.close();
}

// Write to log
void Log::write_to_log(const std::string& message) {
    // Lock log
    if (pthread_mutex_lock(&log_lock) != 0) {
        perror("Bank error: pthread_mutex_lock failed");
    }
    
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
    if (pthread_mutex_unlock(&log_lock) != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
    }
}

// Print incorrect password message to log
void Log::print_inc_pass(const int atm_id, const int acc_id) {
    // Format message
    std::string failure = "Error " + std::to_string(atm_id) + 
                          " Your transaction failed - password for account id " + 
                          std::to_string(acc_id) + " is incorrect";
    write_to_log(failure);
}

// Print account does not exist message to log
void Log::print_no_acc(const int atm_id, const int acc_id) {
    // Format message
    std::string not_exist = "Error " + std::to_string(atm_id) + 
                            " Your transaction failed - account id " + 
                            std::to_string(acc_id) + " does not exist";
    write_to_log(not_exist);
}
