#include "bank.hpp"
#include "atm.hpp"
#include "log.hpp"



int main(int argc, char* argv[]){
    // Input:
    // ./bank <number of VIP threads> <ATM input file 1> <ATM input file 2> ...

    // Create a bank object
    Bank bank;

    // Create log file

    int vip_accounts = atoi(argc[1]);

    // Create an ATM vector
    std::vector<ATM> atm_list;
    std::vector<p_thread_t> threads;
    int idx = 0;

    for (int i = 2; i < argc; i++){
        std::string file_path = argv[i];
        // Create an ATM object and push it to the vector
        ATM new_atm(&bank , file_path, idx, true);
        atm_list.push_back(new_atm);

        // create corresponding thread
        thread.emplace_back();
        pthread_create(&threads[idx], nullptr, ATM::thread_entry, atm_list[idx]);
        idx++;
    }

    // main thread waits for the thread at thread[i] to finish execution
    for (int i = 0; i < atm_list.size(); i++) {
        pthread_join(threads[i], nullptr);
    }
    // Run every atm in a separte thread

    // Do something with the VIP accounts

    // join all threads
    return 0;
}