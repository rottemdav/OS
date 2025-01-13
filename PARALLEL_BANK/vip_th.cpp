#include "vip_th.hpp"
#include <pthread.h>
#include "atm.hpp"

void VipQueue::push_vip_cmd(Cmd& cmd) {
    // push safely a new vip command to the priritized queue
    pthread_mutex_lock(&vip_lock);
    vip_queue.push(cmd);
    pthread_cond_signal(&vip_available);
    pthread_mutex_unlock(&vip_lock);
}

void* VipQueue::vip_thread_entry(void* obj) {
    VipQueue* vip_queue = static_cast<VipQueue*>(obj);
    vip_queue->vip_worker();
    return nullptr;
}

void VipQueue::vip_worker() {
    while(1) {
        Cmd cmd;
        {
            pthread_mutex_lock(&vip_lock);

            while(vip_queue.empty() && !finish) {
                pthread_cond_wait(&vip_available, &vip_lock);
            }

            if (finish && vip_queue.empty()) {
                pthread_mutex_unlock(&vip_lock);
                return;
            }

            cmd = vip_queue.top();
            vip_queue.pop();

            pthread_mutex_unlock(&vip_lock);
        }

        cmd.atm->exe_cmd(cmd);
    }
}