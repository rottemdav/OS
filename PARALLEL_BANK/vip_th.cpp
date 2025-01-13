#include "vip_th.hpp"
#include <pthread.h>
#include "atm.hpp"

void VipQueue::push_vip_cmd(Cmd& cmd) {
    // push safely a new vip command to the priritized queue
    pthread_mutex_lock(&vip_lock);
    vip_queue.push(cmd);
    //std::cout << "pushed new cmd into the queue..." << endl;
    pthread_cond_signal(&vip_available);
    //std::cout << "sent conditional signal..." << endl;
    pthread_mutex_unlock(&vip_lock);
}

void* VipQueue::vip_thread_entry(void* obj) {
    VipQueue* vip_queue = static_cast<VipQueue*>(obj);
    //std::cout << "in vip thread_entry" << endl;
    vip_queue->vip_worker();
    return nullptr;
}

void VipQueue::vip_worker() {
    while(1) {
        Cmd cmd;
        //std::cout << "in worker vip thread" << endl;
        {
            pthread_mutex_lock(&vip_lock);

            while(vip_queue.empty() && !finish->load()) {
                pthread_cond_wait(&vip_available, &vip_lock);
                //std::cout << "got the conditional signal!" << endl;
            }

            if (finish->load() && vip_queue.empty()) {
                pthread_mutex_unlock(&vip_lock);
                return;
            }

            cmd = vip_queue.top();
            //std::cout << "got cmd: " << cmd.cmd_type << " " << cmd.cmd_param[0] << endl;
            vip_queue.pop();

            pthread_mutex_unlock(&vip_lock);
        }

        //cmd.atm->get_log_ptr()->write_to_log("executing command from vip thread.");
        cmd.atm->exe_cmd(cmd);
    }
}