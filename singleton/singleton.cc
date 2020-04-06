#include "Person.hpp"

void* run(void *arg)
{
    pthread_detach(pthread_self());
    while (1)
    {
        Singleton::get_instance()->eat("apple");
    }
}

int main()
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;
    pthread_create(&tid1, nullptr, run, nullptr);
    pthread_create(&tid2, nullptr, run, nullptr);
    pthread_create(&tid3, nullptr, run, nullptr);
    while (1);
    return 0;
}
