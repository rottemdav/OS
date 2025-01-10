#include "read_write.hpp"

// ---- Multi lock functions ----
// ---- Writer priority ----

// Constructor
MultiLock::MultiLock(int& readers) : active_readers(readers) {
    active_readers = 0;
    waiting_writers = 0;
    writer_active = false;

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

MultiLock::enter_read(){
    // Aquire lock
    pthread_mutex_lock(&lock);
    
    // Writers priority, wait until no one needs to write
    while (writer_active || waiting_writers > 0)
        pthread_cond_wait(&readers_allowed, &lock);
    
    // Increase active readers amount
    active_readers++;
    
    // Allow more readers
    pthread_mutex_unlock(&lock);
}

MultiLock::exit_read(){
    // Aquire lock
    pthread_mutex_lock(&lock);

    // Decrease readers amount
    active_readers--;

    // If last reader, signal to writers that writing is allowed
    if (active_readers == 0)
        pthread_cond_signal(&writer_allowed);
    
    // Release the lock 
    pthread_mutex_unlock(&lock);
}

MultiLock::enter_write(){
    // Aquire lock
    pthread_mutex_lock(&lock);

    // Increase waiting writers
    waiting_writers++;

    // Wait for other writer to finish or readers to stop reading
    while (writer_active || active_readers > 0)
        pthread_cond_wait(&writer_allowed, &lock);

    // Let writer enter critical section - mark active and remove from waiting
    writer_active = true;
    waiting_writers--;

    // Release the lock
    pthread_mutex_unlock(&lock);
}

MultiLock::exit_write(){
    // Aquire lock
    pthread_mutex_unlock(&lock);

    // Writer ended so not active
    writer_active = false;

    // If there are writers waiting, let them enter first, otherwise let readers
    // -> writer priority 
    if (waiting_writers > 0){
        pthread_cond_signal(&writer_allowed);
    } else {
        pthread_cond_broadcast(&readers_allowed);
    }

    // Release the lock
    pthread_mutex_unlock(&lock);
}

MultiLock::get_lock(){
    return *lock;
}
// need to implement writers-priority