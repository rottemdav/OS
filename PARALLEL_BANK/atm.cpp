#include "atm.hpp"  // ATM class
#include "bank.hpp" // Bank class
#include "log.hpp"  // Log class
#include <iostream>
#include <string>
#include <format>
#include <sstream>

using namespace std;
#define MAX_STATUS

/* the backbone function of the thread, run through the commands file and process
 * each command with parse_command(cmd).
*/
void read_file() {
    std::string line;
    std::ifstream cmd_file(path);
    if (!file.is_open) {
        std::cerr << "error opening the file" << std::endl;
        return 1;
    }

    while (std::getline(cmd_file, line)) {
        // parse the commnd and execute the matching command
        parse_command(line);

        //check for shut down signal from the bank
        if (close_req == true) { 
            // switch the flag
            is_active = false;
            // send a signal back to the bank that the atm is closed
            close_sig.notify_one();

        }
    }

    // finished go through the file or got interrupted - either way close the file
    cmd_file.close();
    return;
}

void parse_command(string command) {
    std::istringstream stream(command);
    std::string s_cut;
    std::vector<std::string> splitted_cmd;

    while (stream >> str_cut) {
        splitted_cmd.push_back(str_cut); // saves the splitted parts of command in a vector
    }
    
    if (splitted_cmd[0] == "O") {
        // open account - <account> <password> <initial amount>
        O(splitted_cmd[1], splitted_cmd[2], splitted_cmd[3]);

    } else if (splitted_cmd[0] == "D") {
        // deposit - <account> <password> <amount> :
        D(splitted_cmd[1], splitted_cmd[2], splitted_cmd[3]);
        
    } else if (splitted_cmd[0] == "W") {
        // withdraw - <account> <password> <amount> :
        W(splitted_cmd[1], splitted_cmd[2], splitted_cmd[3]);
        
    } else if (splitted_cmd[0] == "B") {
        // check balance - <account> <password>
        B(splitted_cmd[1], splitted_cmd[2]);

    } else if (splitted_cmd[0] == "Q") {
        // close account - <account> <password>
        Q(splitted_cmd[1], splitted_cmd[2]);

    } else if (splitted_cmd[0] == "T") {
        // make a transaction -  <source account> <password> <target account> <amount>
        T(splitted_cmd[1], splitted_cmd[2], splitted_cmd[3], splitted_cmd[4]);

    } else if (splitted_cmd[0] == "C") {
        // close atm -  <target ATM ID>
        C(splitted_cmd[1]);

    } else if (splitted_cmd[0] == "R") {
        // rollback - <iterations>
        C(splitted_cmd[1]);

    }
    return;
}

/* the thread entry function serves as the entry point for the thread create by
 * thread_create. It connects pthread library (works with void*) with ATM class.
 * This is the first function that the thread executes after creation.
 * When read_file() finishes - the thread exits and stops running.
*/
static void* thread_entry(void* obj) {
    ATM* atm = static_cast<ATM*>(obj);
    atm->read_file();
    return nullptr;
}

// Open account - write to account list
void ATM::O(int id, int pwd, int init_amount){
    // Lock account-list read
    bankptr->account_list_lock.enter_read();
    if (bankptr->account_exists(id) != 1){
        bankptr->account_list_lock.exit_read();
        
        // Enter write mode in account list
        bankptr->account_list_lock.enter_write();
        
        // Initialize an account and insert to list 
        BankAccount new_account(id, pwd, init_amount);
        bankptr->accounts_list.push_back(new_account);

        // Exit write mode 
        bankptr->account_list_lock.exit_write();
        

        // Write to log
        std::string success = std::format("{}: New account id is {}
            with password {} and initial balance {}", atm_id, id, pwd, init_amount);
            
        log_ptr->write_to_log(success);
    } else {
        // Release list lock, format error message and print error
        bankptr->account_list_lock.exit_read();
        std::string failure = std::format("Error {}: Your transaction failed -
                                         account with the same id exists", atm_id);
        log_ptr->write_to_log(failure);
    }

}

// Deposit money - write to account, read from account list
void ATM::D(int id, int pwd, in amount){
    // Aquire list lock
    bankptr->account_list_lock.enter_read();

    // Verify account existence
    if (bankptr->account_exists(id) == 1){
        BankAccount* acc_to_dep = bankptr->get_account(id);

        // Verify account password
        if (acc_to_dep->verify_pwd(pwd)){
            // Aquire write lock on account
            acc_to_dep->account_lock.enter_write();

            // Once aquired lock on account we can exit the read 
            bankptr->account_list_lock.exit_read();

            int current_balance = acc_to_dep->get_balance();
            int new_balance = current_balance + amount;
            
            // Set new balance to account
            acc_to_dep->set_balance(new_balance);

            // Release the account write lock
            acc_to_dep->account_lock.exit_write();
            
            // Format success message
            std::string success = std::format("{}: Account {} new balance is {} after
                        {} $ was deposited", atm_id, id, new_balance, amount);
            
            // Write to log
            log_ptr->write_to_log(success);
        } else {
            // Release list lock and print error
            bankptr->account_list_lock.exit_read();
            log_ptr->print_inc_pass(atm_id, id);
        }
    } else {
        // Release list lock and print error
         bankptr->account_list_lock.exit_read();
        log_ptr->print_no_acc(atm_id, id);
    }
     
}

// Withdraw money - write to account, read from account list
void ATM::W(int id, int pwd, int amount){
    // Aquire list lock
    bankptr->account_list_lock->enter_read();

    // Verify account existence
    if (bankptr->account_exists(id) == 1){
        BankAccount* acc_to_with = bankptr->get_account(id);

        // Verify account password
        if (acc_to_with->verify_pwd(pwd)){
            // Aquire account lock
            acc_to_with->account_lock.enter_write();
            
            // Release list lock
            bankptr->account_list_lock.exit_read();
            
            // Get current balance and check if can withdraw
            int current_balance = acc_to_with->get_balance();

            if (current_balance >= amount){
                int new_balance = current_balance - amount;
                acc_to_with->set_balance(new_balance);
                
                // Finished operation so release account lock
                acc_to_with->account_lock.exit_write();

                // Write success to log
                std::string success = std::format("{}: Account {} new balance is {}
                 after {} $ was withdrawn", atm_id, id, new_balance, amount);
                // Write to log
                log_ptr->write_to_log(success);
            } else{
                // Exit write and print error
                acc_to_with->account_lock.exit_write();
                std::string failure = std::format("Error {}: Your transaction failed
                 - account id {} balance is lower than {}", atm_id, id, amount);
                log_ptr->write_to_log(failure);
            }
        } else {
            // Release list lock and print error
            bankptr->account_list_lock.exit_read();
            log_ptr->print_inc_pass(atm_id, id);
        }
    } else {
        // Release list lock and print error
        bankptr->account_list_lock.exit_read();
        log_ptr->print_no_acc(atm_id, id);
    }
}

// Get Account Balance - read from account and from list
void ATM::B(int id, int pwd){
    // Aquire list lock
    bankptr->account_list_lock.enter_read();

    // Verify account existence
    if (bankptr->account_exists(id) == 1){
        BankAccount* acc_to_check = bankptr->get_account(id);
        
        // Verify password
        if (acc_to_check->verify_pwd(pwd)){
                     
            // Aquire account lock
            acc_to_check->account_lock.enter_read();
            
            // Release list lock
            bankptr->account_list_lock.exit_read();
            
            int balance = acc_to_check->get_balance();
            
            // Format message
            std::string message = std::format("{}: Account {} balance is {}",
                                            atm_id, id, balance);

            // Write to log
            log_ptr->write_to_log(message);
            acc_to_check->account_lock.exit_read();
        } else {
            // Release list lock and print error
            bankptr->account_list_lock.exit_read();
            log_ptr->print_inc_pass(atm_id, id);
        }
    } else {
        // Release list lock and print error
        bankptr->account_list_lock.exit_read();
        log_ptrg->print_no_acc(atm_id, id);
    }
}

// Close Account - need to make sure no one is reading or writing to it
void ATM::Q(int id, int pwd){
    // Aquire list read lock
    bankptr->account_list_lock->enter_read();
    
    // Verify account existence
    if (bankptr->account_exists(id) == 1){
        BankAccount* acc_to_close = bankptr->get_account(id);
        
        // Verify account password
        if (acc_to_close->verify_pwd(pwd)){
            // Aquire account write lock
            acc_to_close->account_lock.enter_write();
            int balance = acc_to_close->get_balance();
            // Release account list read lock and aquire write
            bankptr->account_list_lock->exit_read();
            bankptr->account_list_lock->enter_write();

            // Remove the account from the list
            bankptr->accounts_list.earse(
                std::remove(bankptr->accounts_list.begin(),
                            bankptr->accounts_list.end(),
                            *acc_to_close),
                bankptr->accounts_list.end());

            // Release account list write lock
            bankptr->account_list_lock->exit_write();
            
            // BankAccount destructor will call pthread_mutex_destroy for 
            // both locks so no need to exit_write()

            // Format message
            std::string message = std::format("{}: Account {} is now closed.
                Balance was {}", atm_id, id, balance);
            
            // Write message to log
            log_ptr->write_to_log(message);
        } else {
            // Release list lock and print error
            bankptr->account_list_lock.exit_read();
            log_ptr->print_inc_pass(atm_id, id);
        }
    } else {
        // Release list lock and print error
        bankptr->account_list_lock.exit_read();
        log_ptr->print_no_acc(atm_id, id);
    }
}

// Transfer money - read from list, write to accounts 
void ATM::T(int source_id, int pwd, int target_id, int amount){
    // Aquire acount list lock
    bankptr->account_list_lock.enter_read();

    // Verify acccounts existence 
    if (bankptr->account_exists(source_id) == 1 
        && bankptr->account_exists(target_id) == 1){
            Bankaccount* source_acc = bankptr->get_account(source_id);
            Bankaccount* target_acc = bankptr->get_account(target_id);
        
        // Verify account's password
        if (source_acc->verify_pwd(pwd)){
           
            // Aquire write locks for both accounts
            source_acc->enter_write();
            target_acc->enter_write();

            // Release list lock
            bankptr->account_list_lock.exit_read();
            
            // Get accounts current balances 
            int source_current_balance = source_acc->get_balance();
            int target_current_balance = target_acc->get_balance();
            
            // Verify source account have enough money
            if (source_current_balance >= amount){
                
                // Calculate accounts new balances
                int sc_n_blc = source_current_balance - amount;
                int tg_n_blc = target_current_balance + amount;

                // Set new balances
                source_acc->set_balance(sc_n_blc);
                target_acc->set_balance(tg_n_blc);
                
                // Release accounts write lock
                source_acc->exit_write();
                target_acc->exit_write();
                
                // Format message 
                std::string message = std::format("{}: Transfer {} from account {}
                to account {} new account balance is {} new target account balance
                is {}", atm_id, amount, source_id, target_id, sc_n_blc, tg_n_blc);

                // Write to log
                log->write_to_log(message);
            } else {
                // Release accounts write lock
                source_acc->exit_write();
                target_acc->exit_write();

                // Format message
                std::string failure = std::format("Error {}: Your transaction 
                failed - account id {} balance is lower than {}", 
                atm_id, source_id, amount);

                // Write error to log
                log_ptr->write_to_log(failure);
            }
        } else{
            // Release list lock and print error
            bankptr->account_list_lock.exit_read();
            log_ptr->print_inc_pass(atm_id, source_id);
        }
    } else {
        // Release list lock and print error
        bankptr->account_list_lock.exit_read();
        log_ptr->print_no_acc(atm_id, source_id);
    }
}

// Close ATM
void ATM::C(int target_atm_id){
    // the whole implementation is in the bank. 
    int close_status = bankptr->close_atm(atm_id, target_atm_id);
}

// Rollback to the status {iterations} back 
void ATM::R(int iteration){
    // Aquire account list write lock
    bankptr->account_list_lock.enter_write();
    
    
    // Preform the rollback
    int rollback_index = rollback_db.size() - 1 - iteration;
    Status& rollback_status = bankptr->rollback_db[rollback_index];
    
    
    bankptr->account_list_lock.exit_write();

}