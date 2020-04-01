#include "block_queue.hpp"
void* consumer(void *arg)
{
    BlockQueue *bq = (BlockQueue*)arg;
    int data;
    while (true)
    {
        bq->PopData(data);
        cout << "consumer data is : " << data << endl;
    }
}

void* producer(void *arg)
{
    BlockQueue *bq = (BlockQueue*)arg;
    srand(time(nullptr));
    while (true)
    {
        int data = rand()%100 + 1;
        bq->PushData(data);
        cout << "producer data is : " << data << endl;
        sleep(1);
    }
}

int main()
{
    BlockQueue *bq = new BlockQueue();
    pthread_t c, p;
    pthread_create(&p, nullptr, producer, (void*)bq);
    pthread_create(&c, nullptr, consumer, (void*)bq);
    pthread_join(c, nullptr)
    pthread_join(p, nullptr);
    delete bq;
    return 0;
}
