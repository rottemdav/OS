#ifndef VIP_TH_H
#define VIP_TH_H

#include <pthread.h>
#include "atm.hpp"

class ATM;

struct Cmd {
    // each vip command will store a pointer to the atm that called  it
    ATM* atm;
    std::string cmd_type;
    std::vector<int> cmd_param;
    bool is_persistent;
    int vip_lvl;
    int atm_id;
};

struct VipComp {
    bool operator()(const Cmd& a, const Cmd& b) {
        return a.vip_lvl < b.vip_lvl;
    }
};

class VipQueue {
    // priority queue for the commands
    std::priority_queue<Cmd, std::vector<Cmd>, VipComp > vip_queue;

    pthread_mutex_t vip_lock;
    pthread_cond_t vip_available;

public:
    bool* finish;

    //Constructor
    VipQueue(): vip_queue(), finish() {
        pthread_mutex_init(&vip_lock, nullptr);
        pthread_cond_init(&vip_available, nullptr);
    }; 

    //Destructor
    ~VipQueue() {
        pthread_mutex_destroy(&vip_lock);
        pthread_cond_destroy(&vip_available);
    }
    

    static void* vip_thread_entry(void* obj);
    void vip_worker();
    void push_vip_cmd(Cmd& cmd);

};

//extern VipQueue vip_queue;

#endif // ATM_H