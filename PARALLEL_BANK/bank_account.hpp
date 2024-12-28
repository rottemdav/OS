#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H

#include <iostream>

class BankAccount {
    private:
    int accId;  // Account ID
    int accPwd; // Account Password
    int accBlc; // Account Balance

    public:
        // Constructor
        BankAccount(int id, int pwd, int balance);

        // Destructor
        ~BankAccount();

        // Set new balance
        void setBalance(int newBlc);

        // Get current balance
        int getBalance() const;

        // Verify account password
        bool verifyPwd(int pwdGiven);

};

#endif // BANK_ACCOUNT_H