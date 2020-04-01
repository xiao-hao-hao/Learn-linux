#pragma once

#include <iostream>
#include <unistd.h>
#include <vector>
#include <semaphore.h>
using namespace std;

class RingQueue
{
public:
    RingQueue(int cap = 20)
        :_cap(cap), _ring(cap)
    {
        _producer_step = _consumer_step = 0;
        sem_init(&_sem_blank, 0, cap);
        sem_init(&_sem_data, 0, 0);
    }
    ~RingQueue()
    {
        sem_destroy(&_sem_blank);
        sem_destroy(&_sem_data);
    }
    void PushData(int &data)
    {
        sleep(1);
        P(_sem_blank);
        _ring[_producer_step] = data;
        V(_sem_data);
        ++_producer_step;
        _producer_step %= _cap;
    }
    void PopData(int &data)
    {
        //sleep(5);
        //sleep(1);
        P(_sem_data);
        data = _ring[_consumer_step];
        V(_sem_blank);
        ++_consumer_step;
        _consumer_step %= _cap;
    }
private:
    void P(sem_t &sem)
    {
        sem_wait(&sem);
    }
    void V(sem_t &sem)
    {
        sem_post(&sem);
    }
private:
    vector<int> _ring;
    int _cap;
    sem_t _sem_blank;
    sem_t _sem_data;
    int _producer_step;
    int _consumer_step;
};
