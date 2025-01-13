#include "atm.hpp"  // ATM class
#include "vip_th.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <sys/syscall.h>

using namespace std;
#define MAX_STATUS

struct Cmd;

/* the thread entry function serves as the entry point for the thread create by
 * thread_create. It connects pthread library (works with void*) with ATM class.
 * This is the first function that the thread executes after creation.
 * When read_file() finishes - the thread exits and stops running.
*/

void* ATM::thread_entry(void* obj) {
    ATM* atm = static_cast<ATM*>(obj);
    atm->read_file();
    return nullptr;
}

void ATM::read_file() {
    std::string line;
    std::ifstream cmd_file(path);
    if (!cmd_file.is_open()) {
        std::cerr << "error opening the file" << std::endl;
        return ;
    }

    while (std::getline(cmd_file, line)) {
        Cmd cmd = parse_cmd(line); // need to implement new
        cmd.atm_id = this->atm_id;

        if (cmd.vip_lvl > 0) {
            vipptr->push_vip_cmd(cmd); 

            // wait for vip confirmation 
        } else {
            exe_cmd(cmd);
        }

        // Check for shut down signal from the bank
        if (close_req == true) { 
            // switch the flag
            pthread_mutex_lock(&close_mutex);
            is_active = false;
            // send a signal back to the bank that the atm is closed
            pthread_cond_signal(&close_sig);
            pthread_mutex_unlock(&close_mutex);

        }

    }
        // finished go through the file or got interrupted - either way close the file
    cmd_file.close();
}

Cmd ATM::parse_cmd(string command_line) {
    Cmd cmd;
    std::istringstream stream(command_line);
    std::string str_cut;
    std::vector<std::string> splitted_cmd;

    // saves the splitted parts of command in a vector
    while (stream >> str_cut) {
        splitted_cmd.push_back(str_cut); 
    }

    bool is_persistant = false;
    int vip_lvl = 0;

    for (auto &token : splitted_cmd) {
        if (token == "PERSISTENT") {
            is_persistant = true;
        } else if (token.rfind("VIP=", 0) == 0) {
            std::string vip_num = token.substr(4);
            vip_lvl = std::stoi(vip_num);
        } 
    }

    std::cout << "first letter: " << splitted_cmd[0] << endl;

    cmd.cmd_type = splitted_cmd[0];
    splitted_cmd.erase(splitted_cmd.begin());
    if (is_persistant || vip_lvl > 0) {
        splitted_cmd.pop_back();
    }
    for (std::string elem: splitted_cmd) {
        cmd.cmd_param.push_back(stoi(elem));
    }
    cmd.is_persistent = is_persistant;
    cmd.vip_lvl = vip_lvl;
    cmd.atm_id = -1; // to be changed in the read_file() function
    return cmd;

}

void ATM::exe_cmd(Cmd cmd) {
    std::string cmd_type = cmd.cmd_type;
    bool is_persistant = cmd.is_persistent;
    //std::cout << "the vip lvl is: " << cmd.vip_lvl << endl;

    if (cmd.vip_lvl == 0) {
        // ATM wake up 100ms
        sleep(0.1);
    }
    
    // If command has persistent it will run twice in case failed
    if (cmd_type == "O") {
        // open account - <account> <password> <initial amount> <persistent>
        
        int acc_id = (cmd.cmd_param[0]);
        int acc_pwd = (cmd.cmd_param[1]);
        int amount = (cmd.cmd_param[2]);
         
        int res = O(acc_id, acc_pwd, amount, is_persistant);

        // If coomand failed and is persistent, try again
        if ((res == FAILURE) && is_persistant) {
            // Second run
            is_persistant = false;
            O(acc_id, acc_pwd, amount, is_persistant);
        }
        
    } else if (cmd_type == "D") {
        // deposit - <account> <password> <amount> <persistent>
              
        int acc_id = (cmd.cmd_param[0]);
        int acc_pwd = (cmd.cmd_param[1]);
        int amount = (cmd.cmd_param[2]);
        
        int res = D(acc_id, acc_pwd, amount, is_persistant);
                
        // If coomand failed and is persistent, try again
        if ((res == FAILURE) && is_persistant) {
            // Second run
            is_persistant = false;
            D(acc_id, acc_pwd, amount, is_persistant);
        }
        
    } else if (cmd_type == "W") {
        // withdraw - <account> <password> <amount> <persistent>
        
        int acc_id = (cmd.cmd_param[0]);
        int acc_pwd = (cmd.cmd_param[1]);
        int amount = (cmd.cmd_param[2]);

        int res = W(acc_id, acc_pwd, amount, is_persistant);
       
        // If coomand failed and is persistent, try again
        if ((res == FAILURE) && is_persistant) {
            // Second run
            is_persistant = false;
            W(acc_id, acc_pwd, amount, is_persistant);
        }
        
    } else if (cmd_type == "B") {
        // check balance - <account> <password> <persistent>

        int acc_id = (cmd.cmd_param[0]);
        int acc_pwd = (cmd.cmd_param[1]);

        int res = B(acc_id, acc_pwd, is_persistant);

        // If coomand failed and is persistent, try again
        if ((res == FAILURE) && is_persistant) {
            // Second run
            is_persistant = false;
            B(acc_id, acc_pwd, is_persistant);
        }
        

    } else if (cmd_type == "Q") {
        // close account - <account> <password> <persistent>

        int acc_id = (cmd.cmd_param[0]);
        int acc_pwd = (cmd.cmd_param[1]);

        int res = Q(acc_id, acc_pwd, is_persistant);
        // If coomand failed and is persistent, try again
        if ((res == FAILURE) && is_persistant) {
            // Second run
            is_persistant = false;
            Q(acc_id, acc_pwd, is_persistant);
        }

    } else if (cmd_type == "T") {
        // make a transaction -  <source account> <password> <target account> <amount> <persistent>
 
        int acc_id = (cmd.cmd_param[0]);
        int acc_pwd = (cmd.cmd_param[1]);
        int target_amount = (cmd.cmd_param[2]);
        int amount = (cmd.cmd_param[3]);

        int res = T(acc_id, acc_pwd, target_amount, amount, is_persistant);

        // If coomand failed and is persistent, try again
        if ((res == FAILURE) && is_persistant) {
            // Second run
            is_persistant = false;
            T(acc_id, acc_pwd, target_amount, amount, is_persistant);
        }

    } else if (cmd_type == "C") {
        // close atm - <target ATM ID> <persistent>

        int target_atm = (cmd.cmd_param[0]);

        int res = C(target_atm, is_persistant);
        // If coomand failed and is persistent, try again
        if ((res == FAILURE) && is_persistant) {
            // Second run
            is_persistant = false;
            C(target_atm, is_persistant);
        }

    } else if (cmd_type == "R") {
        // rollback - <iterations> <persistent>

        int iterations = (cmd.cmd_param[0]);
        
        int res = R(iterations, is_persistant);
        // If coomand failed and is persistent, try again
        if ((res == FAILURE) && is_persistant) {
            // Second run
            is_persistant = false;
            R(iterations, is_persistant);
        }
    }

    if (cmd.vip_lvl == 0) {
        //sleep for 1 sec
        sleep(1);
    }

} // end parse_comand

// Open account - write to account list
int ATM::O(int id, int pwd, int init_amount, bool is_per){
    // Lock account-list read
    
    bankptr->get_account_list_lock()->enter_read();
    if (bankptr->account_exists(id) != 1){
        bankptr->get_account_list_lock()->exit_read();
        
        // Enter write mode in account list
        bankptr->get_account_list_lock()->enter_write();
        
        // Initialize an account and insert to list 
        BankAccount new_account(id, pwd, init_amount);
        bankptr->get_account_list()->push_back(new_account);

        // Exit write mode 
        bankptr->get_account_list_lock()->exit_write();
        

        // Write to log
        std::string success = std::to_string(atm_id) + ": New account id id " + 
            std::to_string(id) + " with password " + std::to_string(pwd) +
            " and initial balance " + std::to_string(init_amount);
       
        log_ptr->write_to_log(success);

        return SUCCESS;
    } else {
        // Release list lock, format error message and print error
        bankptr->get_account_list_lock()->exit_read();
        std::string failure = "Error " + std::to_string(atm_id) +
                    ": Your transaction failed - account with the same id exists";
        if (is_per == false)
            log_ptr->write_to_log(failure);
        
        return FAILURE; 
    }

}

// Deposit money - write to account, read from account list
int ATM::D(int id, int pwd, int amount, bool is_per){
    // Aquire list lock
    bankptr->get_account_list_lock()->enter_read();

    // Verify account existence
    if (bankptr->account_exists(id) == 1){
        BankAccount* acc_to_dep = bankptr->get_account(id);

        // Verify account password
        if (acc_to_dep->verify_pwd(pwd)){
            // Aquire write lock on account
            acc_to_dep->get_acc_lock()->enter_write();

            // Once aquired lock on account we can exit the read 
            bankptr->get_account_list_lock()->exit_read();

            int current_balance = acc_to_dep->get_balance();
            int new_balance = current_balance + amount;
            
            // Set new balance to account
            acc_to_dep->set_balance(new_balance);

            // Release the account write lock
            acc_to_dep->get_acc_lock()->exit_write();
            
            // Format success message
            std::string success = std::to_string(atm_id) + ": Account " +
                                  std::to_string(id) + " new balance is " +
                                  std::to_string(new_balance) + " after " +
                                  std::to_string(amount) + " $ was deposited";

            // Write to log
            log_ptr->write_to_log(success);

            return SUCCESS;
        } else {
            // Release list lock and print error
            bankptr->get_account_list_lock()->exit_read();
            if (is_per == false)
                log_ptr->print_inc_pass(atm_id, id);
            
            return FAILURE;
        }
    } else {
        // Release list lock and print error
        bankptr->get_account_list_lock()->exit_read();
        if (is_per == false)
            log_ptr->print_no_acc(atm_id, id);

        return FAILURE;
    }
     
}

// Withdraw money - write to account, read from account list
int ATM::W(int id, int pwd, int amount, bool is_per){
    // Aquire list lock
    bankptr->get_account_list_lock()->enter_read();

    // Verify account existence
    if (bankptr->account_exists(id) == 1){
        BankAccount* acc_to_with = bankptr->get_account(id);

        // Verify account password
        if (acc_to_with->verify_pwd(pwd)){
            // Aquire account lock
            acc_to_with->get_acc_lock()->enter_write();
            
            // Release list lock
            bankptr->get_account_list_lock()->exit_read();
            
            // Get current balance and check if can withdraw
            int current_balance = acc_to_with->get_balance();

            if (current_balance >= amount){
                int new_balance = current_balance - amount;
                acc_to_with->set_balance(new_balance);
                
                // Finished operation so release account lock
                acc_to_with->get_acc_lock()->exit_write();

                // Write success to log
                std::string success = std::to_string(atm_id) + ": Account " +
                                      std::to_string(id) + " new balance is " +
                                      std::to_string(new_balance) + " after " +
                                      std::to_string(amount) + " $ was withdrawn";
                // Write to log
                log_ptr->write_to_log(success);

                return SUCCESS;
            } else{
                // Exit write and print error
                acc_to_with->get_acc_lock()->exit_write();
                std::string failure = "Error " + std::to_string(atm_id) +
                                      ": Your transaction failed - account id " +
                                      std::to_string(id) + " balance is lower than " +
                                      std::to_string(amount);
                if (is_per == false)
                    log_ptr->write_to_log(failure);
                
                return FAILURE;
            }
        } else {
            // Release list lock and print error
            bankptr->get_account_list_lock()->exit_read();
            if (is_per == false)    
                log_ptr->print_inc_pass(atm_id, id);
            
            return FAILURE;
        }
    } else {
        // Release list lock and print error
        bankptr->get_account_list_lock()->exit_read();
        if (is_per == false)
            log_ptr->print_no_acc(atm_id, id);
        
        return FAILURE;
    }
}

// Get Account Balance - read from account and from list
int ATM::B(int id, int pwd, bool is_per){
    // Aquire list lock
    bankptr->get_account_list_lock()->enter_read();

    // Verify account existence
    if (bankptr->account_exists(id) == 1){
        BankAccount* acc_to_check = bankptr->get_account(id);
        
        // Verify password
        if (acc_to_check->verify_pwd(pwd)){
                     
            // Aquire account lock
            acc_to_check->get_acc_lock()->enter_read();
            
            // Release list lock
            bankptr->get_account_list_lock()->exit_read();
            
            int balance = acc_to_check->get_balance();
            
            // Format message
            std::string success = std::to_string(atm_id) + ": Account " +
                                  std::to_string(id) + " balance is " +
                                  std::to_string(balance);

            // Write to log
            log_ptr->write_to_log(success);
            acc_to_check->get_acc_lock()->exit_read();

            return SUCCESS;
        } else {
            // Release list lock and print error
            bankptr->get_account_list_lock()->exit_read();
            if (is_per == false)
                log_ptr->print_inc_pass(atm_id, id);
            
            return FAILURE;
        }
    } else {
        // Release list lock and print error
        bankptr->get_account_list_lock()->exit_read();
        if (is_per == false)
            log_ptr->print_no_acc(atm_id, id);
        
        return FAILURE;
    }
}

// Close Account - need to make sure no one is reading or writing to it
int ATM::Q(int id, int pwd, bool is_per){
    // Aquire list read lock
    bankptr->get_account_list_lock()->enter_read();
    
    // Verify account existence
    if (bankptr->account_exists(id) == 1){
        BankAccount* acc_to_close = bankptr->get_account(id);
        
        // Verify account password
        if (acc_to_close->verify_pwd(pwd)){
            // Aquire account write lock
            acc_to_close->get_acc_lock()->enter_write();
            int balance = acc_to_close->get_balance();
            // Release account list read lock and aquire write
            bankptr->get_account_list_lock()->exit_read();
            bankptr->get_account_list_lock()->enter_write();

            // Remove the account from the list
            bankptr->get_account_list()->erase(
                std::remove(bankptr->get_account_list()->begin(),
                            bankptr->get_account_list()->end(),
                            *acc_to_close),
                bankptr->get_account_list()->end());

            // Release account list write lock
            bankptr->get_account_list_lock()->exit_write();
            
            // BankAccount destructor will call pthread_mutex_destroy for 
            // both locks so no need to exit_write()

            // Format message
            std::string success = std::to_string(atm_id) + ": Account " +
                                  std::to_string(id) + " is now closed. Balance was "
                                  + std::to_string(balance);  

            // Write message to log
            log_ptr->write_to_log(success);

            return SUCCESS;
        } else {
            // Release list lock and print error
            bankptr->get_account_list_lock()->exit_read();
            if (is_per == false)
                log_ptr->print_inc_pass(atm_id, id);

            return FAILURE;
        }
    } else {
        // Release list lock and print error
        bankptr->get_account_list_lock()->exit_read();
        if (is_per == false)
            log_ptr->print_no_acc(atm_id, id);

        return FAILURE;
    }
}

// Transfer money - read from list, write to accounts 
int ATM::T(int source_id, int pwd, int target_id, int amount, bool is_per){
    // Aquire acount list lock
    bankptr->get_account_list_lock()->enter_read();

    // Verify acccounts existence 
    if (bankptr->account_exists(source_id) == 1 
        && bankptr->account_exists(target_id) == 1){
            BankAccount* source_acc = bankptr->get_account(source_id);
            BankAccount* target_acc = bankptr->get_account(target_id);
        
        // Verify account's password
        if (source_acc->verify_pwd(pwd)){
           
            // Aquire write locks for both accounts
            source_acc->get_acc_lock()->enter_write();
            target_acc->get_acc_lock()->enter_write();

            // Release list lock
            bankptr->get_account_list_lock()->exit_read();
            
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
                source_acc->get_acc_lock()->exit_write();
                target_acc->get_acc_lock()->exit_write();
                
                // Format message 
                std::string success = std::to_string(atm_id) + ": Transfer " +
                                      std::to_string(amount) + " from account " +
                                      std::to_string(source_id) + " to account " +
                                      std::to_string(target_id) + " new account " +
                                      "balance is " + std::to_string(sc_n_blc) +
                                      " new target account balance is " +
                                      std::to_string(tg_n_blc);
                
                // Write to log
                log_ptr->write_to_log(success);

                return SUCCESS;
            } else {
                // Release accounts write lock
                source_acc->get_acc_lock()->exit_write();
                target_acc->get_acc_lock()->exit_write();

                // Format message
                std::string failure = "Error " + std::to_string(atm_id) +
                                      ": Your transaction failed - account id " +
                                      std::to_string(source_id) + " balance is " +
                                      "lower than " + std::to_string(amount); 

                // Write error to log
                if (is_per == false)
                    log_ptr->write_to_log(failure);
                
                return FAILURE;
            }
        } else{
            // Release list lock and print error
            bankptr->get_account_list_lock()->exit_read();
            if (is_per == false)
                log_ptr->print_inc_pass(atm_id, source_id);

            return FAILURE;
        }
    } else {
        // Release list lock and print error
        bankptr->get_account_list_lock()->exit_read();
        if (is_per == false)
            log_ptr->print_no_acc(atm_id, source_id);
        return FAILURE;
    }
}

// Close ATM
int ATM::C(int target_atm_id, bool is_per){
    // the whole implementation is in the bank. 
    int close_status = bankptr->close_atm(atm_id, target_atm_id, is_per);
    return close_status;
}

// Rollback to the status {iterations} back 
int ATM::R(int iteration, bool is_per){
    
    // Calculate the requsted iteration
    size_t   rollback_index = bankptr->get_status_vector().size() - 1 - iteration;

    // If there is no such iteration do nothing
    if (rollback_index < 0 || rollback_index >= bankptr->get_status_vector().size())
        return FAILURE;
    
   
    // Aquire account list write lock
    bankptr->get_account_list_lock()->enter_write();
   
    // Aquire the required status
    Status& rollback_status = bankptr->get_status_vector()[rollback_index];
    
    // Replace the current account list with the snapshot
    *(bankptr->get_account_list()) = rollback_status.get_snapshot_list();

    // Trim the status vector to most recent status, from the back
    while (bankptr->get_status_vector().size() > rollback_index + 1)
        bankptr->get_status_vector().pop_back();

    // Release the write lock
    bankptr->get_account_list_lock()->exit_write();
    
    // Format the message
    std::string success = std::to_string(atm_id) + ": Rollback to " + 
                          std::to_string(iteration) + "bank iterations ago was" +
                          "completed successfully";

    log_ptr->write_to_log(success);

    // Return success
    return SUCCESS;
}