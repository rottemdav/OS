#include "bank.hpp"
#define ACCOUNT 0
#define ATM 1

// Constructor
Bank::Bank(Log* log) : accounts_list(), rollback_db(), fees_account(), 
        atm_list_pointer(), log_ptr(log);


// Destructor
Bank::~Bank();

void Bank::screen_print() {

    std::vector<BankAccount> copied_list = this->accounts_list;
    std::sort(copied_list.begin(), copied_list.end(), [](const BankAccount& tmp1, const BankAccount& tmp2) {
        return tmp1.acc_id < tmp2.acc_id;
    });

    for (const BankAccount& account : copied_list ) {
        std::cout << "Account" << account.acc_id
                  << "Balance - " << account.acc_blc << "$, "
                  << "Account Password - " << account.acc_pwd
                  << std::endl;
    }
}

BankAccount* Bank::get_account(int id) const {
    // Ensure account list locked before calling

    for (const BankAccount& account : this->accounts_list) {
        if (account.get_id() == id) {
            return &account;
        }
    }
    return nullptr;

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
    for (const ATM& atm : this->atm_list_pointer) {
        if (atm->atm_id == atm_id) return 1;
    }
    
    return 0;

}

// rollback db is FIFO - smaller index means older status - the status in idx 1 is the oldest
// get the accounts_list that the bank have
// duplicate it into a temp vector
// pushback to the status vector
 
void Bank::save_status() {
    // get the num of elements
    int db_size = this->rollback_db.size();

     //push the list to the status's list
    auto curr_list = this->accounts_list;

    //initiaze empty status object
    Status new_status(0,0);

    // the status list is empty
    if (db_size == 0) {
        new_status.snapshot_list = curr_list;
        new_status.counter = 1;
        this->rollback_db.push_back(new_status);
        return;
    }

    // check if the db is full - if the vector size is 120. 
    if (db_size >= 120) {
        rollback_db.erase(rollback_db.begin()); // remove the oldest status
    }
    
    new_status.counter = this->rollback_db.back().counter + 1; 
    new_status.snapshot_list = curr_list;
    //push the new status to the db
    this->rollback_db.push_back(new_status);
    return;
} // end of save_status

//collect fee from the bank accounts in the bank
// the return value of the function is the collected fee
int Bank::collect_fee() const {
    //generate the random fee value
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_ditribution<> range(1,5);
    double fee_amount = range(gen);
    fee_amount /= 100;

    int cml_fee = 0;
    //lock account list
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< !!! change here - the bank is allowed to change the bank accounts directly !!! >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    pthread_mutex_lock(&account_list_mutex);
     for (BankAccount& account : this->accounts_list) {
            int calc_amount = static_cast<int>(account.acc_blc * fee_amount); // Calculate fee
            account.acc_blc -= calc_amount;
            cml_fee += calc_amount;
    }
    // Unlock list 
    pthread_mutex_unlock(&account_list_mutex);
    return cml_fee;
}

void Bank::update_fee_account() {
    int fees_collected = collect_fee();
    fees_account->acc_blc += fees_collected;
    return;
}

// ATM will ask the bank to close ATM with a specific ID.
// the bank need to check in each accounts list print if there's pending requests
// ATM the receive a signal to shutdown will finish its current action and then destruct itself and become idle.
int Bank::close_atm(int source_id, int target_id)  const {
    // Aquire write lock for atm list
    atm_list_lock.enter_write();
    
    ATM* atm_to_close = atm_list_pointer[target_id-1] //atm_id-1 -> each atm gets an id serially, so atm n. 2 will be located on idx 1
    
    // Check ATM existence 
    if (atm_exists(target_id) == 0) {
        atm_list_lock.exit_write();
        std::string msg = "Error " + std::to_string(source_id) + ": Your Transaction Failed - ATM ID " + 
                           std::to_string(target_id) + "does not exist";
        log_ptr->write_to_log(msg);
        return -1; 
    }

    // caller atm - > bank - > callee atm terminates - > signals back to the bank for termination - > bank logs to the log - > back to the caller
    // send to atm with atm_id to close
    
    // Flagging the target atm to close 
    atm_to_close->close_req = true;
    /* the callee atm checks if there's active close request, if so - change activation status and notify the bank */

    // If already closed print error 
    if (!atm_to_close->is_active) {
        atm_list_lock.exit_write();
        std::string msg = "Error " + std::to_string(source_id) + ": Your close operation failed - ATM ID" + 
                           std::to_string(target_id) + "is already in a closed state";
        log->write_to_log(msg);
        return -1; 
    }

    // Wait for target ATM to finish operation
    atm_to_close->close_sig.wait(&atm_list_mutex, [atm_to_close]{return !is_active});
    
    // Release ATM list lock
    atm_list_lock.exit_write();

    // Print success message
    std::string msg = "Bank " + std::to_string(source_id) + " closed " + std::to_string(target_id) + " successfully";
    log->write_to_log(msg);
    return 0;
}
 