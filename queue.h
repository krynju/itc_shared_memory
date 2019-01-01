//
// Created by gulin on 01/01/2019.
//

#ifndef SOI_LAB4_QUEUE_H
#define SOI_LAB4_QUEUE_H


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
        //if (isFull()) return;
        buffer[head] = element;
        ++size;
        ++head;
        if (head == 10) head = 0;
    }

    int dequeue() {
        //if (isEmpty()) return -1;
        int return_value = buffer[tail];
        ++tail;
        --size;
        if (tail == 10) tail = 0;
        return return_value;
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

#endif //SOI_LAB4_QUEUE_H
