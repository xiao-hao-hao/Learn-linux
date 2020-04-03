#include "thread_pool.hpp"

int main()
{
    ThreadPool *tp = new ThreadPool(3);
    srand(time(nullptr));
    string s = "+-*/";
    while (true)
    {
        int x = rand()%500 + 1;
        int y = rand()%50 + 1;
        char op = s[rand()%4];
        Task t(x, y, op);
        tp->PushTask(t);

    }
    delete tp;
    return 0;
}
