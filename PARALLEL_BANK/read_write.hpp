#ifndef READ_WRITE
#define READ_WRITE

#include <pthread.h>

// Multiple readers - single writer lock
class MultiLock {
    int active_readers;       // Number of active readers
    int waiting_writers;      // Number of waiting writers
    bool writer_active;       // Is a writer active?

    pthread_mutex_t lock;
    pthread_cond_t readers_allowed;
    pthread_cond_t writer_allowed;

public:
    // Constructor
    MultiLock();

    // Destructor
    ~MultiLock();

    // Reader lock functions
    void enter_read();
    void exit_read();

    // Writer lock functions
    void enter_write();
    void exit_write();

    // Get the lock object
    pthread_mutex_t* get_lock();
};

#endif // READ_WRITE
