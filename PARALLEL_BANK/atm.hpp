#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <string>
#include <pthread.h>

#include "bank.hpp"
#include "log.hpp"
#include "bank_account.hpp"
#include "vip_th.hpp"

using namespace std;

class Bank;
class Log;
class VipQueue;


enum Operation_status{
    FAILURE = 0,
    SUCCESS = 1
};

// ATM class
class ATM {
    Bank* bankptr;    // Pointer to the bank object
    VipQueue* vipptr; //Pointer to the vip queue object
    string path;      // Path to the ATM operations file
    int atm_id;       // ATM ID
    bool is_active;   // ATM status
    Log* log_ptr;  // Pointer to the log object
    bool close_req;   // true - a signal to close was sent
    
    pthread_cond_t close_sig; // cv to signal the bank that atm closed
    pthread_mutex_t close_mutex; // for the self closing operation

    public:
        // Constructor
        ATM(Bank* bank,VipQueue* vip_queue,const string& dir, int id, bool is_active, Log* log, bool close_req)
            : bankptr(bank),vipptr(vip_queue), path(dir), atm_id(id), is_active(is_active), log_ptr(log), close_req(close_req)
        { 
            pthread_cond_init(&close_sig, nullptr);
            pthread_mutex_init(&close_mutex, nullptr);
        }

        // Destructor
        ~ATM() { 
            pthread_cond_destroy(&close_sig); 
            pthread_mutex_destroy(&close_mutex); 
        }


        // Read the ATM operations file and parse commands
        void read_file();
        
        // Parse the command
        Cmd parse_cmd(std::string command_line);

        // create the thread that will be tied to the atm
        static void* thread_entry(void* obj);

        // Open bank account
        int O(int id, int pwd, int init_amount, bool is_per);
        
        // Deposit money into account
        int D(int id, int pwd, int amount, bool is_per);
        
        // Withdraw money from account
        int W(int id, int pwd, int amount, bool is_per);
        
        // Get account balance
        int B(int id, int pwd, bool is_per);
        
        // Close bank account
        int Q(int id, int pwd, bool is_per);
        
        // Transfer money between bank accounts
        int T(int source_id, int pwd, int target_id, int amount, bool is_per);

        // Close ATM
        int C(int target_atm_id, bool is_per);

        // Rollback to the status {iterations} back
        int R(int iteration, bool is_per);


        void exe_cmd(Cmd cmd);

        //getter
        int get_id() const { return atm_id; }
        bool get_close_req() const {return close_req;}
        bool get_is_active() const {return is_active;}
        pthread_cond_t* get_close_sig() {return &close_sig;}

        //setter
        void set_close_req(bool value) { close_req = value;}
        void set_is_active(bool value) { is_active = value;}
};



#endif // ATM_H