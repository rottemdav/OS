#include "bank_account.hpp"

// Constructor
BankAccount::BankAccount(int id, int pwd int balance):
             acc_id(id), acc_pwd(pwd), acc_blc(balance),
             acc_readers(0), account_lock(acc_readers);

// Copy Constructor
BankAccount::BankAccount(const BankAccount& other)
    : acc_id(other.acc_id), acc_pwd(other.acc_pwd), acc_blc(other.acc_blc),
      acc_readers(other.acc_readers), account_lock(other.account_lock);

// Destructor
BankAccount::~BankAccount();

// Set a new balance - account expected to be locked to write upon enter
void BankAccount::set_balance(int new_blc){
    acc_blc = new_blc;
}

// Get current balance - account expected to be locked to read upon enter
int BankAccount::get_balance() const {
    return balance;
}

// Get account ID
int BankAccount::get_id() const {
    return acc_id;
}

// Verify account password
bool BankAccount::verify_pwd(int pwd_given){
    return (acc_pwd == pwd_given);
}


