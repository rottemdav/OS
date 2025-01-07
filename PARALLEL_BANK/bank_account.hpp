#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H

#include <iostream>
#include <pthread.h>


class BankAccount {
    private:
    int acc_id;  // Account ID
    int acc_pwd; // Account Password
    int acc_blc; // Account Balance
    
    int active_readers; // Number of active readers
    pthread_mutex_t read_lock; // Account read lock
    pthread_mutex_t writer_lock; // Account write lock


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

        // Get account ID
        int get_id() const {};

        // Verify account password
        bool verify_pwd(int pwd_given);

        // Enter read mode
        void enter_read();

        // Exit read mode
        void exit_read();

        // Enter write mode
        void enter_write();

        // Exit write mode
        void exit_write();



};

#endif // BANK_ACCOUNT_H