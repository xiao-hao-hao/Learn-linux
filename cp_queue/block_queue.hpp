#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

class BlockQueue
{
public:
    BlockQueue(int cap = 5) : _cap(cap)
    {
        pthread_mutex_init(&lock, nullptr);
        pthread_cond_init(&cond_c, nullptr);
        pthread_cond_init(&cond_p, nullptr);
    }
    //producer
    void PushData(int &data)
    {
        LockQueue();
        while (IsFull())
        {
            //pthread_cond_signal(&cond_c);
            ProducerWait();
        }
        q.push(data);
        UnlockQueue();
        pthread_cond_signal(&cond_c);
    }
    //consumer
    void PopData(int &data)
    {
        LockQueue();
        while (IsEmpty())
        {
            //pthread_cond_signal(&cond_p);
            ConsumerWait();
        }
        data = q.front();
        q.pop();
        UnlockQueue();
        pthread_cond_signal(&cond_p);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond_c);
        pthread_cond_destroy(&cond_p);
    }
private:
    bool IsFull()
    {
        return q.size() == _cap;
    }
    bool IsEmpty()
    {
        return q.size() == 0;
    }
    void LockQueue()
    {
        pthread_mutex_lock(&lock);
    }
    void UnlockQueue()
    {
        pthread_mutex_unlock(&lock);
    }
    void ProducerWait()
    {
        pthread_cond_wait(&cond_p, &lock);
    }
    void ConsumerWait()
    {
        pthread_cond_wait(&cond_c, &lock);
    }
private:
    queue<int> q;
    int _cap;
    pthread_mutex_t lock;
    pthread_cond_t cond_c;
    pthread_cond_t cond_p;
};
