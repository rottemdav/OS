#include "read_write.hpp"
#include <iostream>

// Constructor
MultiLock::MultiLock() 
    : active_readers(0), waiting_writers(0), writer_active(false) {
    if (pthread_mutex_init(&lock, nullptr) != 0) {
        std::cerr << "Bank error: pthread_mutex_init failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&readers_allowed, nullptr) != 0) {
        std::cerr << "Bank error: pthread_cond_init failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&writer_allowed, nullptr) != 0) {
        std::cerr << "Bank error: pthread_cond_init failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Destructor
MultiLock::~MultiLock() {
    if (pthread_mutex_destroy(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_destroy failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (pthread_cond_destroy(&readers_allowed) != 0) {
        std::cerr << "Bank error: pthread_cond_destroy failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (pthread_cond_destroy(&writer_allowed) != 0) {
        std::cerr << "Bank error: pthread_cond_destroy failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Enter read
void MultiLock::enter_read() {
    //std::cout << "thread " << pthread_self() << " attempting to lock and read mutex\n";
    if (pthread_mutex_lock(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_lock failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    //std::cout << "thread " << pthread_self() << "acquired read mutex\n";
    while (writer_active || waiting_writers > 0) {
        //std::cout << "thread " << pthread_self() << " waiting to writer to leave\n";
        if (pthread_cond_wait(&readers_allowed, &lock) != 0) {
            std::cerr << "Bank error: pthread_cond_wait failed" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    active_readers++;
    //std::cout << "active readers: " << std::to_string(waiting_writers) << std::endl;
    //std::cout << "thread " << pthread_self() << " releasing read mutex\n";
    if (pthread_mutex_unlock(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_unlock failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Exit read
void MultiLock::exit_read() {
   if (pthread_mutex_lock(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_lock failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    active_readers--;
    //std::cout << "active readers: " << std::to_string(waiting_writers) << std::endl;
    if (active_readers == 0) {
        //std::cout << "thread " << pthread_self() << " signal to let the writer in\n";
        if (pthread_cond_signal(&writer_allowed) != 0) {
            std::cerr << "Bank error: pthread_cond_signal failed" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    //std::cout << "thread " << pthread_self() << " releasing read mutex\n";
    if (pthread_mutex_unlock(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_unlock failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Enter write
void MultiLock::enter_write() {
    //std::cout << "thread " << pthread_self() << " attempting to lock and write mutex\n";
    if(pthread_mutex_lock(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_lock failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    //std::cout << "thread " << pthread_self() << " aquired write mutex\n";
    
    waiting_writers++;
    
    //std::cout << "waiting writers: " << std::to_string(waiting_writers) << std::endl;
    while (writer_active || active_readers > 0) {
       if (pthread_cond_wait(&writer_allowed, &lock) != 0) {
            std::cerr << "Bank error: pthread_cond_wait failed" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    writer_active = true;
    
    waiting_writers--;
    
    //std::cout << "waiting writers: " << std::to_string(waiting_writers) << std::endl;
        //std::cout << "thread " << pthread_self() << " releasing write mutex\n";
    if (pthread_mutex_unlock(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_unlock failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Exit write
void MultiLock::exit_write() {
    if (pthread_mutex_lock(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_lock failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    writer_active = false;
    if (waiting_writers > 0) {
        //std::cout << "thread " << pthread_self() << "|| status: no waiting readers: signal to let the writer in\n";
        if (pthread_cond_signal(&writer_allowed) != 0) {
            std::cerr << "Bank error: pthread_cond_signal failed" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    } else {
        //std::cout << "thread " << pthread_self() << "|| status: no writers: signal to let the readers in\n";
        if (pthread_cond_broadcast(&readers_allowed) != 0) {
            std::cerr << "Bank error: pthread_cond_broadcast failed" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    //std::cout << "thread " << pthread_self() << " releasing write mutex\n";
    if (pthread_mutex_unlock(&lock) != 0) {
        std::cerr << "Bank error: pthread_mutex_unlock failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Get the lock object
pthread_mutex_t* MultiLock::get_lock() {
    return &lock;
}
