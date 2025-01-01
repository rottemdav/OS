#include "bank_account.h"

// Constructor
BankAccount::BankAccount(int id, int pwd, int balance)
    : acc_id(id), acc_pwd(pwd), acc_blc(balance) {}

// Copy Constructor
BankAccount::BankAccount(const BankAccount& other)
    : acc_id(other.acc_id), acc_pwd(other.acc_pwd), acc_blc(other.acc_blc) {

}


// Destructor
BankAccount::~BankAccount() {}

// Set a new balance
void BankAccount::set_balance(int new_blc){
    // lock? when is account allowed to write
    this->acc_blc = new_blc;
    // unlock?
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


