#ifndef READ_WRITE
#define READ_WRITE

#include <pthread.h>

// Multiple readers - single writer lock
class MultiLock {
    int& active_readers; // Reference to readers integer
    ..int& waiting_writers; // Reference to writers integer
    pthread_mutex_t read_lock;
    pthread_mutex_t write_lock;

    public:
    
    // Constructor
    MultiLock(int& readers){}

    // Destructor
    ~MultiLock(){} 

    void enter_read();

    void exit_read();

    void enter_write();
    
    void exit_write();

};
 

#endif // READ_WRITE