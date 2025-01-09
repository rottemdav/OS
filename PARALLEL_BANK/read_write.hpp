#ifndef READ_WRITE
#define READ_WRITE

#include <pthread.h>

// Multiple readers - single writer lock
class MultiLock {
    int active_readers; // Reference to readers integer
    int waiting_writers; // Reference to writers integer
    bool writer_active; 

    pthread_mutex_t lock;
    pthread_cond_t readers_allowed;
    pthread_cond_t writer_allowed;

    public:
    
    // Constructor
    MultiLock(){}

    // Destructor
    ~MultiLock(){} 

    void enter_read();

    void exit_read();

    void enter_write();
    
    void exit_write();

};
 

#endif // READ_WRITE