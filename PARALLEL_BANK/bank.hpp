#ifndef BANK_H
#define BANK_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include <random>

#include "bank_account.hpp"
#include "atm.hpp" 


// the class for the statuses that will be used for the rollback
class Status {
    std::vector<BankAccount> snapshot_list;
    int idx;
    int counter;

public:

    // C'tor
    Status(int idx_val, int counter): snapshot_list(), idx(idx_val), counter(counter+1) {};

};

// std::vector<BankAccount> bank
class Bank{
private:
    std::vector<BankAccount> accounts_list;
    std::vector<Status> rollback_db;
    BankAccount* fees_account;
    ATM* atm_list_pointer; // a pointer to the vector object of atms
    Log* log_ptr;
    // vector of some sort to check rollback requsts 
    
    // Locks for lists
    MultiLock account_list_lock;
    MultiLock atm_list_lock;

public: 

    // C'tor
    Bank(Log* log); 

    // D'tor
    ~Bank() {}
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

    // update the bank's fees account
    void update_fees_account();

    // sends a signal to start an ATM shutdown
    int close_atm(ATM*) const;


};


#endif // BANK_H