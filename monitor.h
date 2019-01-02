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
#include <unistd.h>
#include <sstream>
#include <cstdlib>

#include "queue.h"
#include "sema.h"

#define MAX_OVERALL_CAPACITY 50
#define MIN_OVERALL_CAPACITY 0

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
        if (overall_buffers_state == MAX_OVERALL_CAPACITY) {
            wait(fullness);
        }
        int q;
        bool checked[5] = {false, false, false, false, false};
        while (1) {
            int i = std::rand() % 5;
            if (checked[i])
                continue;
            else
                checked[i] = true;
            if (!queues[i].isFull()) {
                queues[i].enqueue(element);
                q = i;
                break;
            }
        }

#ifdef PRINT_1
        std::stringstream ss;
        ss << "|Q0= " << queues[0].getSize() << "\t|Q1= " << queues[1].getSize() << "\t|Q2= " << queues[2].getSize()
           << "\t|Q3= " << queues[3].getSize() << "\t|Q4= " << queues[4].getSize() << "\t| put " << q << std::endl;
        std::cout << ss.str();
#endif

        overall_buffers_state += 1;
        if (queues[q].getSize() == MIN_OVERALL_CAPACITY + 1) {
            signal(emptiness[q]);
        }

        leave();
        return q; //return queue number
    }

    int get(int queue_number) {
        enter();
        if (queues[queue_number].isEmpty())
            wait(emptiness[queue_number]);

        overall_buffers_state -= 1;

        int ret = queues[queue_number].dequeue();

#ifdef PRINT_1
        std::stringstream ss;
        ss << "|Q0= " << queues[0].getSize() << "\t|Q1= " << queues[1].getSize() << "\t|Q2= " << queues[2].getSize()
           << "\t|Q3= " << queues[3].getSize() << "\t|Q4= " << queues[4].getSize() << "\t| get "<< queue_number <<std::endl;
        std::cout << ss.str();
#endif

        if (overall_buffers_state == MAX_OVERALL_CAPACITY - 1) {
            signal(fullness);
        }

        leave();
        return ret;
    }
};


#endif
