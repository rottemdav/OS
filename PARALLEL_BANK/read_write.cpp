#include "read_write.hpp"

// Constructor
MultiLock::MultiLock() 
    : active_readers(0), waiting_writers(0), writer_active(false) {
    pthread_mutex_init(&lock, nullptr);
    pthread_cond_init(&readers_allowed, nullptr);
    pthread_cond_init(&writer_allowed, nullptr);
}

// Destructor
MultiLock::~MultiLock() {
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&readers_allowed);
    pthread_cond_destroy(&writer_allowed);
}

// Enter read
void MultiLock::enter_read() {
    pthread_mutex_lock(&lock);
    while (writer_active || waiting_writers > 0) {
        pthread_cond_wait(&readers_allowed, &lock);
    }
    active_readers++;
    pthread_mutex_unlock(&lock);
}

// Exit read
void MultiLock::exit_read() {
    pthread_mutex_lock(&lock);
    active_readers--;
    if (active_readers == 0) {
        pthread_cond_signal(&writer_allowed);
    }
    pthread_mutex_unlock(&lock);
}

// Enter write
void MultiLock::enter_write() {
    pthread_mutex_lock(&lock);
    waiting_writers++;
    while (writer_active || active_readers > 0) {
        pthread_cond_wait(&writer_allowed, &lock);
    }
    writer_active = true;
    waiting_writers--;
    pthread_mutex_unlock(&lock);
}

// Exit write
void MultiLock::exit_write() {
    pthread_mutex_lock(&lock);
    writer_active = false;
    if (waiting_writers > 0) {
        pthread_cond_signal(&writer_allowed);
    } else {
        pthread_cond_broadcast(&readers_allowed);
    }
    pthread_mutex_unlock(&lock);
}

// Get the lock object
pthread_mutex_t* MultiLock::get_lock() {
    return &lock;
}
