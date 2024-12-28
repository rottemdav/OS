#include "bank.h"

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

    BankAccount* account_it = std::find_if(this->accounts_list.begin(), this->accounts_list.end(), [id](const BankAccount& account) {
        return account.acc_id == id;
    });

    if (account_it != this->accounts_list.end()) {
        return &(*account_it); // account_it is an iterator so we're deferencing and returning its pointer
    } 

    return nullptr; // the account was not found - action will be inserted later
}


int Bank::account_exists(int id) const {

    auto account_it = std::find_if(this->accounts_list.begin(), this->accounts_list.end(), [id](const BankAccount& account) {
        return account.acc_id == id;
    });

    //matching account was found
    if (account_it != this->accounts_list.end()) {
        return 1; 
    } 

    return 0;

}

int Bank::atm_exists(int atm_id) const {
    
    auto atm_it = std::find_if(this->atm_list_pointer->begin(), this->atm_list_pointer->end(), [atm_id](const ATM& atm) {
        return atm.atm_id == atm_id;
    });

    //matching account was found
    if (atm_it != this->atm_list_pointer->end()) {
        return 1; 
    } 

    return 0;

}


int Bank::save_status() {
    if (this->)
}

