#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H

#include <iostream>
#include <pthread.h>


class BankAccount {
    private:
    int acc_id;  // Account ID
    int acc_pwd; // Account Password
    int acc_blc; // Account Balance
    pthread_mutex_t acc_mutex; // Account Mutex

    public:
        // Constructor
        BankAccount(int id, int pwd, int balance)
            :acc_id(id), acc_pwd(pwd), acc_blc(balance) {};

        // Copy Constructor
        BankAccount(const BankAccount& other)
            :acc_id(other.acc_id), acc_pwd(other.acc_pwd), acc_blc(other.acc_blc) {};
        
        // Destructor
        ~BankAccount() {};

        // Set new balance
        void set_balance(int new_blc);

        // Get current balance
        int get_balance() const;

        // Verify account password
        bool verify_pwd(int pwd_given);

        // Lock bank account
        void lock_account();

        // Unlock bank account
        void unlock_account();

};

#endif // BANK_ACCOUNT_H