#include "bank.h"

// Constructor
Bank::Bank() : accounts_list(), rollback_db(), fees_account(), atm_list_pointer() {
    pthread_mutex_init(&account_list_mutex, nullptr);
    pthread_mutex_init(&atm_list_mutex, nullptr);
}

// Destructor
Bank::~Bank() {
    pthread_mutex_destroy(&account_list_mutex);
    pthread_mutex_destroy(&atm_list_mutex);
}

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
    // Lock the list
    pthread_mutex_lock(&account_list_mutex);

    for (const BankAccount& account : this->accounts_list) {
        if (account.get_id() == id) {
            // Unlock list 
            pthread_mutex_unlock(&list_mutex);
            return &account;
        }
    }

    // Unlock list to allow other ATM to access it
    pthread_mutex_unlock(&account_list_mutex);
    return nullptr;

    // --- previous implementation --- 
    // BankAccount* account_it = std::find_if(this->accounts_list.begin(), this->accounts_list.end(), [id](const BankAccount& account) {
    //     return account.acc_id == id;
    // });

    // if (account_it != this->accounts_list.end()) {
    //     return &(*account_it); // account_it is an iterator so we're deferencing and returning its pointer
    // } 

    // return nullptr; // the account was not found - action will be inserted later
}


int Bank::account_exists(int id) const {
    // Lock account list
    pthread_mutex_lock(&list_mutex);
    if (this->get_account(id) != nullptr) {
        // Unlock list
        pthread_mutex_unlock(&list_mutex);
        return 1;
    }

    // Unlock list
    pthread_mutex_unlock(&list_mutex);
    return 0;

    // --- previous implementation ---
     
    // auto account_it = std::find_if(this->accounts_list.begin(), this->accounts_list.end(), [id](const BankAccount& account) {
    //     return account.acc_id == id;
    // });

    // //matching account was found
    // if (account_it != this->accounts_list.end()) {
    //     return 1; 
    // } 

}

int Bank::atm_exists(int atm_id) const {
    // Lock ATM list
    pthread_mutex_lock(&atm_list_mutex);

    // Check if atm exists in list
    for (const ATM& atm : this->atm_list_pointer)

    // Unlock ATM list    
    pthread_mutex_unlock(&atm_list_mutex);
    return 0;

    // --- previous implementation --- 

    // auto atm_it = std::find_if(this->atm_list_pointer->begin(), this->atm_list_pointer->end(), [atm_id](const ATM& atm) {
    //     return atm.atm_id == atm_id;
    // });

    // //matching account was found
    // if (atm_it != this->atm_list_pointer->end()) {
    //     return 1; 
    // } 

    // return 0;

}


int Bank::save_status() {
    if (this->)
}

