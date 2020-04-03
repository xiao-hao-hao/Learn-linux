#ifndef _THREad_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <iostream>
#include <queue>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define NUM 5

class Task
{
public:
    Task()
    {
        _x = 1;
        _y = 1;
        _op = '+';
    }
    Task(int x, int y, char op)
        :_x(x), _y(y), _op(op)
    {}
    ~Task()
    {}
    void Run()
    {
        int result = 0;
        switch(_op)
        {
        case '+':
            result = _x + _y;
            break;
        case '-':
            result = _x - _y;
            break;
        case '*':
            result = _x * _y;
            break;
        case '/':
            result = (_y==0 ? -1 : _x/_y);
            break;
        default:
            break;
        }
        cout << "thread id: " << pthread_self() << ":" << 
            _x << _op << _y << '=' << result << endl;
        sleep(1);
    }
private:
    int _x;
    int _y;
    char _op;
};

class ThreadPool
{
public:
    ThreadPool(int num = NUM):_num(num)
    {
        pthread_t tid;
        for (int i = 0; i < _num; ++i)
            pthread_create(&tid, nullptr, Routine, this);
        pthread_mutex_init(&_lock, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_lock);
        pthread_cond_destroy(&_cond);
    }
    static void *Routine(void *arg)
    {
        ThreadPool *self = (ThreadPool*)arg;
        while (true)
        {
            self->LockTaskQueue();
            while (self->TaskQueueEmpty())
                self->ThreadWait();
            Task t;
            self->PopTask(t);
            self->UnlockTaskQueue();
            t.Run();
        }
    }
    void PopTask(Task &t)
    {
        t = _q.front();
        _q.pop();
    }
    void PushTask(Task &t)
    {
        LockTaskQueue();
        _q.push(t);
        UnlockTaskQueue();
        ThreadWakeUp();
    }
public:
    bool TaskQueueEmpty()
    {
        return _q.size() == 0;
    }
    void ThreadWait()
    {
        pthread_cond_wait(&_cond, &_lock);
    }
    void ThreadWakeUp()
    {
        pthread_cond_signal(&_cond);
    }
    void LockTaskQueue()
    {
        pthread_mutex_lock(&_lock);
    }
    void UnlockTaskQueue()
    {
        pthread_mutex_unlock(&_lock);
    }
private:
    int _num;
    queue<Task> _q;
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
};

#endif
