#include <unistd.h>
#include "bank.hpp"

#define _ACCOUNT_ 0
#define _ATM_ 1


// Constructor
Bank::Bank(Log* log, std::vector<ATM>* atm) : accounts_list(), rollback_db(), fees_account(), 
        atm_list_pointer(atm), log_ptr(log), rollback_req(0), reg_queue(), vip_queue() {} 


// Destructor
Bank::~Bank() {}

// <<<<<<<<<<< --------------------------- threading functions  ------------------------------ >>>>>>>>>>>>

// the operating function for the regular worker thread
void* Bank::reg_thread_entry(void* obj) {
    Bank* bank = static_cast<Bank*>(obj);
    while (1) {
        Cmd cmd;
        bool get_next_cmd = bank->pop_and_copy_reg_cmd(cmd); //need to implement
        if (!get_next_cmd) {
            //reach the end of the queue
            break;
        }
    }
    bank->exe_cmd(cmd); // need to implement
    return nullptr;
}

// the operating function for the vip worker thread
void* Bank::vip_thread_entry(void* obj) {
    //fill in the same as the regular thread entry
}

// <<<<<<<<<<< --------------------------- bank functions  ------------------------------ >>>>>>>>>>>>

void Bank::print_to_screen() {

    account_list_lock.enter_read();

    std::vector<BankAccount> copied_list = this->accounts_list;
    std::sort(copied_list.begin(), copied_list.end(), [](const BankAccount& tmp1, const BankAccount& tmp2) {
        return tmp1.get_id() < tmp2.get_id();
    });

    std::cout << "\033[1;1H";
    std::cout << "\033[2J";
    for (const BankAccount& account : copied_list ) {
        std::cout << "Account" << account.get_id()
                  << "Balance - " << account.get_balance() << "$, "
                  << "Account Password - " << account.get_pwd()
                  << std::endl;
    }
    account_list_lock.exit_read();

    //check if there's atms to close
    int sigs_received = 0;
    int req_num = 0;
    atm_list_lock.enter_read();
    auto &atm_list = *atm_list_pointer;
    for (auto &atm : atm_list) {
        if (atm.get_close_req()) {
            req_num++;
            pthread_cond_t* close_sig = atm.get_close_sig();
            pthread_mutex_t* atm_list_mutex = atm_list_lock.get_lock();
            pthread_mutex_lock(atm_list_mutex);
                while(atm.get_is_active()) {
                    pthread_cond_wait(close_sig, atm_list_mutex); //mutex pointer
                    sigs_received++;
                }
            pthread_mutex_unlock(atm_list_mutex); 

        }
    }
    atm_list_lock.exit_read();

}

void* Bank::print_thread_entry(void* obj) {
    PrintThread* prnt_th =  static_cast<PrintThread*>(obj);
    Bank* bank = prnt_th->bank;
    bool* finished = prnt_th->finished;
    while(1) {
        //sleep for 0.5 secs
        sleep(0.5);
        if (*finished)  {
            break;
        }
        bank->print_to_screen();

    }
    return nullptr;
}

BankAccount* Bank::get_account(int id) const {
    // Ensure account list locked before calling
    for (BankAccount& account : *get_account_list()) {
        if (account.get_id() == id) {
            return &account; // Return the address of the matching account
        }
    }
    return nullptr; // Return nullptr if no matching account is found
}



int Bank::account_exists(int id) const {
    // Ensure acccount list locked before calling
    
    if (this->get_account(id) != nullptr) return 1;
    
    // else 
    return 0; 
}

int Bank::atm_exists(int atm_id) const {
    // Ensure atm list locked before calling

    // Check if atm exists in list
    for (const ATM& atm : *this->atm_list_pointer) {
        if (atm.get_id() == atm_id) return 1;
    }
    
    return 0;

}

// rollback db is FIFO - smaller index means older status - the status in idx 1 is the oldest
// get the accounts_list that the bank have
// duplicate it into a temp vector
// pushback to the status vector
 
int Bank::save_status() {
    // get the num of elements
    int db_size = this->rollback_db.size();

     //push the list to the status's list
    auto curr_list = this->accounts_list;

    //initiaze empty status object
    Status new_status(0,0);

    // the status list is empty
    if (db_size == 0) {
        new_status.set_snapshot_list(curr_list);
        new_status.set_counter(1);
        this->rollback_db.push_back(new_status);
        return 1;
    }

    // check if the db is full - if the vector size is 120. 
    if (db_size >= 120) {
        rollback_db.erase(rollback_db.begin()); // remove the oldest status
    }
    
    new_status.set_counter(this->rollback_db.back().get_counter() + 1); 
    new_status.set_snapshot_list(curr_list);
    //push the new status to the db
    this->rollback_db.push_back(new_status);
    return 1;
} // end of save_status

//collect fee from the bank accounts in the bank
// the return value of the function is the collected fee
int Bank::collect_fee() {
    //generate the random fee value
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> range(1,5);
    double fee_amount = range(gen);
    fee_amount /= 100;

    //int cml_fee = 0;
    //lock account list
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< !!! change here - the bank is allowed to change the bank accounts directly !!! >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    
    get_account_list_lock()->enter_write();
     for (BankAccount account : accounts_list) {
            int calc_amount = static_cast<int>(account.get_balance() * fee_amount); // Calculate fee
            int balance = account.get_balance();
            balance -= calc_amount;
            account.set_balance(balance);
    }
    // Unlock list 
    get_account_list_lock()->exit_write();
    return 1;
}

/*void Bank::update_fees_account() {
    int fees_collected = collect_fee();
    fees_account->acc_blc += fees_collected;
    return;
} */

// ATM will ask the bank to close ATM with a specific ID.
// the bank need to check in each accounts list print if there's pending requests
// ATM the receive a signal to shutdown will finish its current action and then destruct itself and become idle.
int Bank::close_atm(int source_id, int target_id, bool is_per) {
    // Aquire write lock for atm list
    atm_list_lock.enter_write();
    
    ATM* atm_to_close = &(*atm_list_pointer)[target_id-1];  //atm_id-1 -> each atm gets an id serially, so atm n. 2 will be located on idx 1
    
    // Check ATM existence 
    if (atm_exists(target_id) == 0) {
        atm_list_lock.exit_write();
        std::string failure = "Error " + std::to_string(source_id) + ": Your Transaction Failed - ATM ID " + 
                           std::to_string(target_id) + "does not exist";
        if (is_per == false)
            log_ptr->write_to_log(failure);
        return FAILURE; 
    }

    pthread_mutex_t* atm_list_mutex = atm_list_lock.get_lock();
    pthread_cond_t* close_sig = atm_to_close->get_close_sig();

    // caller atm - > bank - > callee atm terminates - > signals back to the bank for termination - > bank logs to the log - > back to the caller

    if (source_id == target_id) {
        atm_list_lock.exit_write();
        pthread_mutex_lock(atm_list_mutex);
        atm_to_close->set_is_active(false);
        pthread_mutex_unlock(atm_list_mutex);
        atm_list_lock.enter_write();
        return SUCCESS;
    }
    
    // Flagging the target atm to close 
    atm_to_close->set_close_req(true);
    /* the callee atm checks if there's active close request, if so - change activation status and notify the bank */


    // If already closed print error 
    if (!atm_to_close->get_is_active()) {
        atm_list_lock.exit_write();
        std::string failure = "Error " + std::to_string(source_id) + ": Your close operation failed - ATM ID" + 
                           std::to_string(target_id) + "is already in a closed state";
        if (is_per == false)
            log_ptr->write_to_log(failure);
        return FAILURE; 
    }   

    // Release ATM list lock
    atm_list_lock.exit_write();

    // Wait for the ATM to finish its operation

    pthread_mutex_lock(atm_list_mutex);
    // Wait for target ATM to finish operation
    while (atm_to_close->get_is_active()) {  // Use the getter method
        pthread_cond_wait(close_sig, atm_list_mutex); //mutex pointer
    }
    pthread_mutex_unlock(atm_list_mutex);
    
    atm_list_lock.enter_write();

    // Print success message
    std::string success = "Bank " + std::to_string(source_id) + " closed " + std::to_string(target_id) + " successfully";
    log_ptr->write_to_log(success);
    atm_list_lock.exit_write();
    return SUCCESS;
}

 

 // Status function    
int Status::get_counter() const {
    return counter;
}

const std::vector<BankAccount> Status::get_snapshot_list() const {
    return snapshot_list;
}

void Status::set_counter(int value) {
    counter = value;
}

void Status::set_snapshot_list(std::vector<BankAccount>& new_list ) {
    snapshot_list = new_list;
}

MultiLock* Bank::get_account_list_lock(){
    return &account_list_lock;
}

std::vector<BankAccount>* Bank::get_account_list() const {
    return const_cast<std::vector<BankAccount>*>(&accounts_list);
}

std::vector<Status> Bank::get_status_vector(){
    return rollback_db;
}

int Bank::get_close_req_num() {
    atm_list_lock.enter_read();
    int req_num = 0;
    auto &atm_list = *atm_list_pointer;
    for (auto &atm : atm_list) {
        if (atm.get_close_req()) {
            req_num++;
        }
    }
    atm_list_lock.enter_read();
    return req_num;
}
