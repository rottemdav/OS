#include "read_write.hpp"

// ---- Multi lock functions ----

// Constructor
MultiLock::MultiLock(int& readers) : active_readers(readers) {
    pthread_mutex_init(&read_lock, nullptr);
    pthread_mutex_init(&write_lock, nullptr);
}

// Destructor
MultiLock::~MultiLock() {
     pthread_mutex_destroy(&read_lock);
     pthread_mutex_destroy(&write_lock);
}

MultiLock::enter_read(){
    pthread_mutex_lock(&read_lock);
    active_readers++;

    // If first reader, wait for writer to finish
    if (active_readers == 1)
        pthread_mutex_lock(&write_lock); 
    
    // Allow more readers
    pthread_mutex_unlock(&read_lock);
}

MultiLock::exit_read(){
    pthread_mutex_lock(&read_lock);
    active_readers--;

    // If last reader, enable writing
    if (active_readers == 0)
        pthread_mutex_unlock(&write_lock);

    pthread_mutex_unlock(&read_lock);
}

MultiLock::enter_write(){
    pthread_mutex_lock(&write_lock);
}

MultiLock::exit_write(){
    pthread_mutex_unlock(&write_lock);
}