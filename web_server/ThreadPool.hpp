#pragma once

#include <iostream>
#include <pthread.h>
#include <queue>

using namespace std;

typedef void *(*Handler_t)(void *args);

class Task
{
public:
    Task(int _sock = -1)
    {}
    ~Task()
    {}
    void SetTask(int _sock, Handler_t handler)
    {
        h = handler;
        sock = _sock;
    }
    void Run()
    {
        h((void*)&sock);
    }
private:
    int sock;
    Handler_t h;
};

class ThreadPool
{
public:
    ThreadPool(int _num = 5) : num(_num)
    {
        pthread_mutex_init(&lock, nullptr);
        pthread_cond_init(&cond, nullptr);
    }
    ~ThreadPool()
    {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond);
    }
    void LockQueue()
    {
        pthread_mutex_lock(&lock);
    }
    void UnlockQueue()
    {
        pthread_mutex_unlock(&lock);
    }
    void WakeupThread()
    {
        pthread_cond_signal(&cond);
    }
    void PushTask(Task t)
    {
        LockQueue();
        q.push(t);
        UnlockQueue();
        WakeupThread();
    }
    void ThreadWait()
    {
        pthread_cond_wait(&cond, &lock);
    }
    bool IsQueueEmpty()
    {
        return q.empty();
    }
    Task PopTask()
    {
        Task t = q.front();
        q.pop();
        return t;
    }
    static void* Routine(void *args)
    {
        ThreadPool *tp = (ThreadPool*)args;//拿到当前对象
        while (true)
        {
            tp->LockQueue();
            //从任务队列中拿任务
            while(tp->IsQueueEmpty())
            {
                //如果没有任务就要将当前线程休眠
                //当任务到来的时候唤醒线程
                tp->ThreadWait();
            }
            //处理任务
            Task t = tp->PopTask();
            tp->UnlockQueue();
            t.Run();
        }
    }
    void InitThreadPool()
    {
        pthread_t tid;
        for (int i = 0; i < num; ++i)
        {
            pthread_create(&tid, nullptr, Routine, this);
            pthread_detach(tid);
        }
    }
private:
    int num;
    queue<Task> q;//消费场所
    pthread_mutex_t lock;
    pthread_cond_t cond;
};
