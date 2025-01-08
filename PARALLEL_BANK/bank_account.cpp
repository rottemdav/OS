#include "bank_account.hpp"

// Constructor
BankAccount::BankAccount(int id, int pwd, int balance)
    : acc_id(id), acc_pwd(pwd), acc_blc(balance) {
        // Initialize mutex
        pthread_mutex_init(&read_lock, nullptr);
        pthread_mutex_init(&writer_lock, nullptr);
    }

// Copy Constructor
BankAccount::BankAccount(const BankAccount& other)
    : acc_id(other.acc_id), acc_pwd(other.acc_pwd), acc_blc(other.acc_blc) {
        // Initialize mutex
        pthread_mutex_init(&read_lock, nullptr);
        pthread_mutex_init(&writer_lock, nullptr);
}

// Destructor
BankAccount::~BankAccount() {
    // Destroy mutex
    pthread_mutex_destroy(&read_lock);
    pthread_mutex_destroy(&writer_lock);
}

// lock bank account
void BankAccount::enter_read(){
    pthread_mutex_lock(&read_lock);
    active_readers++;
    if (active_readers == 1) // If first writer
        pthread_mutex_lock(&writer_lock); // Wait for writer to finish
    
    // unlock read lock to allow other readers
    pthread_mutex_unlock(&read_lock);
}

void BankAccount::exit_read(){
    pthread_mutex_lock(&read_lock);
    active_readers--;
    if (active_readers == 0) // If last reader
        pthread_mutex_unlock(&writer_lock); // Allow writer to write
    pthread_mutex_unlock(&read_lock); // Unlock read lock
}

void BankAccount::enter_write(){
    pthread_mutex_unlock(&writer_lock);
}

void BankAccount::exit_write(){
    pthread_mutex_unlock(&writer_lock);
}

// Set a new balance
void BankAccount::set_balance(int new_blc){
    enter_write();
    acc_blc = new_blc;
    exit_write();
}

// Get current balance
int BankAccount::get_balance() const {
    enter_read();
    int balance = acc_blc;
    exit_read();
    return acc_blc;
}

// Get account ID
int BankAccount::get_id() const {
    return acc_id;
}

// Verify account password
bool BankAccount::verify_pwd(int pwd_given){
    return (acc_pwd == pwd_given);
}


