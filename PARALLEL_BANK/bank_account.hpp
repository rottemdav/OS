#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H

#include <iostream>
#include <pthread.h>
#include "read_write.hpp"

class BankAccount {
    private:
    int acc_id;  // Account ID
    int acc_pwd; // Account Password
    int acc_blc; // Account Balance
    
    int acc_readers; // Number of active readers
    MultiLock account_lock;


    public:
        // Constructor
        BankAccount(int id, int pwd, int balance);
           

        // Copy Constructor
        BankAccount(const BankAccount& other);
        
        // Destructor
        ~BankAccount();

        // Set new balance
        void set_balance(int new_blc);

        // Get current balance
        int get_balance() const;

        // Get account ID
        int get_id() const;

        // Verify account password
        bool verify_pwd(int pwd_given);

};

#endif // BANK_ACCOUNT_H