#include "bank_account.h"

// Constructor
BankAccount::BankAccount(int id, int pwd, int balance)
    : acc_id(id), acc_pwd(pwd), acc_blc(balance) {
        // Initialize mutex
        pthread_mutex_init(&acc_mutex, NULL);
    }

// Copy Constructor
BankAccount::BankAccount(const BankAccount& other)
    : acc_id(other.acc_id), acc_pwd(other.acc_pwd), acc_blc(other.acc_blc) {
        // Initialize mutex
        pthread_mutex_init(&acc_mutex, NULL);
}

// Destructor
BankAccount::~BankAccount() {
    // Destroy mutex
    pthread_mutex_destroy(&acc_mutex);
}

// lock bank account
void BankAccount::lock_account(){
    pthread_mutex_lock(&acc_mutex);
}

// Unlock bank account
void BankAccount::unlock_account(){
    pthread_mutex_unlock(&acc_mutex);
}

// Set a new balance
void BankAccount::set_balance(int new_blc){
    this->lock_account();
    
    this->acc_blc = new_blc;
    
    this->unlock_account();
}

// Get current balance
int BankAccount::get_balance() const {
    // is he allowed to read (no one writing to him) ??
    // lock reader
    return this->acc_blc;
    // unlock reader
}

// Verify account password
bool BankAccount::verify_pwd(int pwd_given){
    return (acc_pwd == pwd_given);
}


