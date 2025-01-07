#ifndef BANK_H
#define BANK_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "bank_account.hpp"
#include <pthread.h>

//class BankAccount;

// the class for the statuses that will be used for the rollback
class Status {
    std::vector<BankAccount> snapshot_list;
    int idx;
    bool is_full;

public:

    // C'tor
    Status(int idx_val): snapshot_list(), idx(idx_val), is_full(false);

}

// std::vector<BankAccount> bank
class Bank{
    std::vector<BankAccount> accounts_list;
    std::vector<Status> rollback_db;
    BankAccount* fees_account;
    ATM* atm_list_pointer; // a pointer to the vector object of atms
    pthread_mutex_t account_list_mutex; // Lock for account list
    pthread_mutex_t atm_list_mutex; // Lock for ATM list

public: 

    // C'tor
    Bank(): accounts_list(), rollback_db(), fees_account(), atm_list_pointer() {}

    // printing to the screen function
    // inside this function the bank checks if a request to close an ATM was submitted
    void screen_print() const;

    // get the wanted bank account
    BankAccount* get_account(int id) const;

    // checks if the account exists in the list --> 1 true : 0 false;
    int account_exists(int id) const;

    // checks if the atm exists in the list --> 1 true : 0 false;
    int atm_exists(int atm_id) const;

    // save the current status to the rollback_db, 0 for success -1 for failure.
    int save_status(Status* curr_status);

    // send collectors to collect the bank's fees
    int collect_fee() const;

    // sends a signal to start an ATM shutdown
    int close_atm(ATM*) const;

}


#endif // BANK_H