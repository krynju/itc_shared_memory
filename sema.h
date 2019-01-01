//
// Created by gulin on 01/01/2019.
//

#ifndef SOI_LAB4_SEMAPHORE_H
#define SOI_LAB4_SEMAPHORE_H

#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <iostream>

#include <unistd.h>
#include <unistd.h>
#include <sstream>
#include <cstdlib>

class Semaphore {
private:
    sem_t sem;
public:
    Semaphore(int value) {
        if (sem_init(&sem, 0, value) != 0)
            throw "sem_init: failed";
    }

    ~Semaphore() {
        sem_destroy(&sem);
    }

    void p() {
        if (sem_wait(&sem) != 0)
            throw "sem_wait: failed";
    }

    void v() {
        if (sem_post(&sem) != 0)
            throw "sem_post: failed";

    }
};


#endif //SOI_LAB4_SEMAPHORE_H
