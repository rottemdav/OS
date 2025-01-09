#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <string>

#include "bank.hpp"
#include "log.hpp"
#include"bank_account.hpp"

using namespace std;

// ATM class
class ATM {
    Bank* bankptr;    // Pointer to the bank object
    string path;      // Path to the ATM operations file
    int atm_id;       // ATM ID
    bool is_active;   // ATM status
    Log* log_ptr;  // Pointer to the log object
    bool close_req;   // true - a signal to close was sent
    pthread_cond_t close_sig; // cv to signal the bank that atm closed

    public:
        // Constructor
        ATM(Bank* bank, string dir, int id, Log* log)
            : bankptr(bank),path(dir), atm_id(id), log_ptr(log) {};

        // Destructor
        ~ATM() {};

        // Read the ATM operations file and parse commands
        void read_file();
        
        // Parse the command
        void parse_command(string command);

        // create the thread that will be tied to the atm
        static void* thread_entry(void* obj);

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