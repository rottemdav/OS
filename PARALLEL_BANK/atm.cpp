#include "atm.hpp"  // ATM class
#include "bank.hpp" // Bank class
#include "log.hpp"  // Log class
#include <iostream>
#include <string>
#include <format>

using namespace std;

// Open account
void ATM::O(int id, int pwd, int init_amount){
    // Lock account list
    pthread_mutex_lock(&this->bankptr->account_list_mutex);
    if (bankptr->account_exists(id) != 1){
        
        // Initialize an account
        BankAccount new_account(id, pwd, init_amount);
        bankptr->accounts_list.push_back(new_account);
        pthread_mutex_unlock(&this->bankptr->account_list_mutex);

        // Write to log
        std::string success = std::format("{}: New account id is {}
            with password {} and initial balance {}", atm_id, id, pwd, init_amount);
            
        log->write_to_log(success);
    } else {
        // Write error to log
        std::string failure = std::format("Error {}: Your transaction failed -
                                         account with the same id exists", atm_id);
        log->write_to_log(failure);
    }

}

// Deposit money
void ATM::D(int id, int pwd, in amount){
    // Lock account list
    
    if (bankptr->account_exists(id) == 1){

        if (thisbankptr->get_account(id, pwd)->verify_pwd(pwd)){

            BankAccount* acc_to_dep = bankptr->get_account(id, pwd);

            // Lock account 
            int current_balance = acc_to_dep.get_balance();
            int new_balance = current_balance + amount;

            acc_to_dep->set_balance(new_balance);

            std::string success = std::format("{}: Account {} new balance is {} after
                        {} $ was deposited", atm_id, id, new_balance, amount);
            // Write to log
            log->write_to_log(success);
        } else {
            // write error to log 
            std::string failure = std::format("Error {}: Your transaction failed -
             password for account id {} is incorrect", atm_id, id);   
            
            log->write_to_log(failure);
        }
    } else {
        // Write error to log
        std::string not_exist = std::format("Error {}: Your transaction failed -
                    account id {} does not exist", atm_id, id);
        log->write_to_log(not_exist);
    }
     
}

// Withdraw money
void ATM::W(int id, int pwd, int amount){
    
    if (bankptr->account_exists(id) == 1){
        if (bankptr->get_account(id, pwd)->verify_pwd(pwd)){
            BankAccount* acc_to_with = this->bankptr->get_account(id, pwd);
            if (acc_to_with->get_balance() >= amount){
            
                acc_to_with->set_balance(acc_to_with->get_balance() - amount);

                // write to log
                // "<ATM ID>: Account <id> new balance is <balance> after <amount> $ was withdrawn"
                return;
            } else{
                // write error to log
                // "Error <ATM ID>: Your transaction failed - account id <id> balance is lower than <amount>"
            }
        } else {
            // write error to log 
            // "Error <ATM ID>: Your transaction failed - password for account id <id> is incorrect"
        }
    }
}

// Get Account Balance
void ATM::B(int id, int pwd){
    // if (can read)
    // lock to read
        if (this->bankptr->account_exists(id) == 1){
            if (this->bankptr->get_account(id, pwd)->verify_pwd(pwd)){
                // write to log
                // "<ATM ID>: Account <id> balance is <balance>"
            } else {
                // write error to log 
                // "Error <ATM ID>: Your transaction failed - password for account id <id> is incorrect"
            }
        }
}

// Close Account
void ATM::Q(int id, int pwd){
    // if (can read) (meaning the bank isn't adding a new account)
    if (this->bankptr->account_exists(id) == 1){
        if (this->bankptr->get_account(id, pwd)->verify_pwd(pwd)){
            // lock writer
            BankAccount* acc_to_close = this->bankptr->get_account(id, pwd);
            this->bankptr->accounts_list.earse(acc_to_close);
            // unlock writer

            // write to log
            // "<ATM ID>: Account <id> is now closed. Balance was <balance>"
        } else {
            // write error to log 
            // "Error <ATM ID>: Your transaction failed - password for account id <id> is incorrect"
        }
    }
}

// Transfer money
void ATM::T(int source_id, int pwd, int target_id, int amount){
    // if (can read)
    if (this->bankptr->account_exists(source_id) == 1 
                            && this->bankptr->account_exists(target_id) == 1){
        if (this->bankptr->get_account(id, pwd)->verify_pwd(pwd)){
            Bankaccount* source_acc = this->bankptr->get_account(source_id, pwd);
            Bankaccount* target_acc = this->bankptr->get_account(target_id, pwd);

            if (source_acc->get_balance() >= amount){
                // lock writer
                source_acc->set_balance(source_acc->get_balance() - amount);
                target_acc->set_balance(target_acc->get_balance() + amount);
                // unlock writer

                // write to log
                // "<ATM ID>: Transfer <amount> from account <source_id> to account 
                // <target_id> new accpimt balance is <balance> new target account 
                // balance is <target account balance>"
                return;
            } else {
                // write error to log
                // "Error <ATM ID>: Your transaction failed - account id <source_id> balance is lower than <amount>"
            }
        } else{
            // write error to log
            // "Error <ATM ID>: Your transaction failed - password for account id <source_id> is incorrect"
        }
    }
}

// Close ATM
void ATM::C(int target_atm_id){
    int close_status = this->bankptr->close_atm(target_atm_id);

    // check if atm exists in the bank
    // if (can read (atm-list))

    if (this->bankptr->atm_exists(target_atm_id) = 1){
        if (this->bankptr->atm_list[target_atm_id]->is_active){
            // lock atm-list writer  
            // chekc if atm has active transactions if so, end them
            ATM* atm_to_close = this->bankptr->atm_list[target_atm_id];
            atm_to_close->is_active = false;
            // unlock atm-list writer

           // write to log
            // "Bank: ATM <source ATM ID> close <target ATM ID> successfully"
        } else {
            // write error to log
            // "Error <source ATM ID>: Your close operation failed - ATM ID 
            // <target ATM ID> is already in a closed state"
        }

    } else {
        // write error to log
        // lock log writer
        // "Error <source ATM ID>: Your transaction failed - ATM ID <ATM ID> does not exist"
    }
}

// Rollback to the status {iterations} back 
void ATM::R(int iteration){
    // if (can read)
    // lock rollback_db reader
   // sort current status list by account id
   // sort requsted stauts list by account id
   // iterate over the list and compare the two lists
   // compare two lists, for each iteration add check if exists in current 
   // account list, if not add it to the list if it exists, check the balance and
   // if it's different, update the balance  
}