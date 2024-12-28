#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <string>

#include"bank_account.h"

using namespace std;

// ATM class
class ATM {
    Bank* bankptr;   // Pointer to the bank object
    string path;     // Path to the ATM operations file
    int atm_id;       // ATM ID
    bool is_active;   // ATM status
    public:
        // Constructor
        ATM(Bank* bank, string dir, int id)
            : bankptr(bank),path(dir), atm_id(id) {};

        // Destructor
        ~ATM() {};

        // Open bank account
        void O(int id, int pwd, int init_amount);
        
        // Deposit money into account
        void D(int id, int pwd, int amount);
        
        // Withdraw money from account
        void W(int id, int pwd, int amount);
        
        // Get account balance
        void B(int id, int pwd);
        
        // Close bank account
        void Q(int id, int pwd);
        
        // Transfer money between bank accounts
        void T(int source_id, int pwd, int target_id, int amount);

        // Close ATM
        void C(int target_atm_id);

        // Rollback to the status {iterations} back
        void R(int iteration);
};

#endif // ATM_H