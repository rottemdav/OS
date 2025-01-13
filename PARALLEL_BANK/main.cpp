#include "bank.hpp"
#include "atm.hpp"
#include "log.hpp"
#include "vip_th.hpp"



int main(int argc, char* argv[]){
    // Input:
    // ./bank <number of VIP threads> <ATM input file 1> <ATM input file 2> ...
    bool finish = false;

    // Create log file
    Log log("log.txt");

    int vip_accounts = atoi(argv[1]);
    //create the thread pool for the vip threads
    VipQueue vip_queue;
    vip_queue.finish = &finish;
    std::vector<pthread_t> vip_threads;
    if (vip_accounts > 0) {
        vip_threads.resize(vip_accounts);

        for (int i = 0; i < vip_accounts; i++){
        pthread_create(&vip_threads[i], nullptr, VipQueue::vip_thread_entry, &vip_queue);
        }
    }
    

    int atms_num = argc - 2;
    // Create an ATM vector
    std::vector<ATM> atm_list;
    atm_list.reserve(atms_num); // since we know how many atms there are, we reserve the memory so there won't be any allocations
    std::vector<pthread_t> threads;
        //pthread_t* atms = new pthread_t[atms_num];

    // Create a bank object
    Bank bank(&log, &atm_list);

    PrintThread prnt_th{&bank, &finish};
    pthread_t printer_thread;
    pthread_create(&printer_thread, nullptr, Bank::print_thread_entry, &prnt_th);

    for (int i = 2; i < argc; i++){
        std::string file_path = argv[i];
        printf("path: %s\n", argv[i]);
        // Create an ATM object and push it to the vector
        //ATM new_atm(&bank , file_path, i-1, true, &log, false);
        atm_list.emplace_back(&bank,&vip_queue, file_path, i-1, true, &log, false);

        // create matching thread and run every atm in a separte thread
            //thread.emplace_back();
        pthread_t thread;
        pthread_create(&thread, nullptr, ATM::thread_entry, &atm_list.back());
        threads.push_back(thread);
    }

    //join all vip threads
    for (pthread_t& thread: vip_threads) {
        pthread_join(thread, nullptr);
    }

    // main thread waits for the thread at thread[i] to finish execution
    // join all atm threads
    for (auto &th : threads) {
        pthread_join(th, nullptr);
    }

    pthread_join(printer_thread, nullptr);

    finish = true;

    /*
    for (size_t i = 0; i < atm_list.size(); i++) {
        pthread_join(atms[i], nullptr);
    }*/

    // Do something with the VIP accounts
    return 0;
}