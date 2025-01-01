#include "bank.h"
#include "atm.h"

int main(int argc, char* argv[]){
    // Input:
    // ./bank <number of VIP threads> <ATM input file 1> <ATM input file 2> ...

    // Create a bank object
    Bank bank;

    int vip_accounts = atoi(argc[1]);

    // Create an ATM vector
    std::vector<ATM> atm_list;

    for (int i = 2; i < argv; i++){
        std::string file_path = argv[i];
        // Create an ATM object and push it to the vector
        ATM atm(&bank , file_path, i-1, true);
        atm_list.push_back(atm);
    }

    // Run every atm in a separte thread

    // Do something with the VIP accounts

    // join all threads
    return 0;
}