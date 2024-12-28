#include "bank_account.h"

// Constructor
BankAccount::BankAccount(int id, int pwd, int balance)
    : accId(id), accPwd(pwd), accBlc(balance) {}

// Destructor
BankAccount::~BankAccount() {}

// Set a new balance
void BankAccount::setBalance(int newBlc){
    // lock? when is account allowed to write
    accBlc = newBlc;
    // unlock?
}

// Get current balance
int BankAccount::getBalance() const {
    // is he allowed to read (no one writing to him) ??
    return accBlc;
}

// Verify account password
bool BankAccount::verifyPwd(int pwdGiven){
    return (accPwd == pwdGiven);
}


