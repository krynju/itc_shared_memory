#ifndef __monitor_h
#define __monitor_h

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <iostream>

#include <unistd.h>
#include <semaphore.h>
#include <unistd.h>
#include <sstream>
#include <cstdlib>

class Queue {
private:
    unsigned int head;
    unsigned int tail;
    int buffer[10];
    unsigned int size = 0;
public:
    Queue() {
        head = 0;
        tail = 0;
    }

    void enqueue(int element) {
        if (isFull()) return;
        buffer[head] = element;
        ++size;
        ++head;
        if (head == 10) head = 0;
    }

    int dequeue() {
        if (isEmpty()) return -1;
        int a = buffer[tail];
        ++tail;
        --size;
        if (tail == 10) tail = 0;
        return a;
    }

    bool isFull() {
        return size == 10;
    }

    bool isEmpty() {
        return size == 0;
    }

    unsigned int getSize() {
        return size;
    }
};

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

class Condition {
    friend class Monitor;

private:
    Semaphore w;
    int waitingCount;
public:
    Condition() : w(0) {
        waitingCount = 0;
    }

    void wait() {
        w.p();
    }

    bool signal() {
        if (waitingCount) {
            --waitingCount;
            w.v();
            return true;
        } else
            return false;
    }
};

class Monitor {
private:
    Semaphore s;

    void enter() {
        s.p();
    }

    void leave() {
#ifdef PRINT_1
        std::stringstream ss;
        ss << "Q0 = " << queues[0].getSize() << "\tQ1 = " << queues[1].getSize() << "\tQ2 = " << queues[2].getSize()
           << "\tQ3 = " << queues[3].getSize() << "\tQ4 = " << queues[4].getSize() << std::endl;
        std::cout << ss.str();
#endif
        s.v();
    }

    void wait(Condition &cond) {
        ++cond.waitingCount;
        leave();
        cond.wait();
    }

    void signal(Condition &cond) {
        if (cond.signal())
            enter();
    }

    Condition fullness;
    Condition emptiness[5];
    int overall_buffers_state;
    Queue queues[5];

public:
    Monitor() : s(1) {
        overall_buffers_state = 0;
    }

    int put(int element) {
        enter();
        if (overall_buffers_state == 50) {
            wait(fullness);
        }
        int q;
        while(1){
            int i = std::rand()%5;
            if (!queues[i].isFull()) {
                queues[i].enqueue(element);
                q = i;
                break;
            }
        }

        overall_buffers_state += 1;
        if (queues[q].getSize() == 1) {
            signal(emptiness[q]);
        }
        leave();
        return q; //return queue number
    }

    int get(int queue_number) {
        enter();
        if (queues[queue_number].isEmpty()) {
            wait(emptiness[queue_number]);
        }
        overall_buffers_state -= 1;

        int ret = queues[queue_number].dequeue();

        if (overall_buffers_state == 49) {
            signal(fullness);
        }
        leave();
        return ret;
    }
};


#endif
