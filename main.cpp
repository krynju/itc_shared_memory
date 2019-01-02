#include "monitor.h"
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>


Monitor monitor;

void *consumer(void *queue_num) {
    for (int i = 0; i < 10; i++) {
        int val = monitor.get((long) queue_num);
#ifdef PRINT_2
        std::stringstream ss;
        ss << "i: "<<i<< "\tC: " << (long)queue_num << "\tget val: " << val << "\tfrom q: "<< (long)queue_num << std::endl;
        std::cout << ss.str();
#endif
        sleep(1);
    }
    pthread_exit(NULL);
}

void *producer(void *producer_id) {
    for (int i = 0; i < 10; i++) {
        int val = std::rand() % 32;
        int q = monitor.put(val);

#ifdef PRINT_2
        std::stringstream ss;
        ss << "i: "<<i<< "\tP: " << (long)producer_id << "\tput val: " << val << "\tinto q: " << q << std::endl;
        std::cout << ss.str();
#endif
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    std::srand(time(0));

    if(argc != 2) return -1;
    int producer_count = atol(argv[1]);

    pthread_t threads[5 + producer_count];

    for (long i = 0; i < 5; ++i) {
        pthread_create(&threads[i], NULL, consumer, (void *) i);
        usleep(100000);
    }

    for (long i = 0; i < producer_count; ++i) {
        pthread_create(&threads[5 + i], NULL, producer, (void *) i);
        usleep(100000);
    }

    pthread_exit(NULL);
}


