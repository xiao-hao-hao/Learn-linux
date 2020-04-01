#include "cp.hpp"

void* consumer(void *arg)
{
    RingQueue *rq = (RingQueue*)arg;
    while (true)
    {
        int data = 0;
        rq->PopData(data);
        cout << "consumeer data done : " << data << endl;
    }
}

void* producer(void *arg)
{
    RingQueue *rq = (RingQueue*)arg;
    srand(time(nullptr));
    while (true)
    {
        int data = rand() % 100 + 1;
        rq->PushData(data);
        cout << "producer data done : " << data << endl;
    }
}

int main()
{
    RingQueue *rq = new RingQueue(5);
    pthread_t c, p;
    pthread_create(&c, nullptr, consumer, (void*)rq);
    pthread_create(&p, nullptr, producer, (void*)rq);
    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    delete rq;
    return 0;
}
