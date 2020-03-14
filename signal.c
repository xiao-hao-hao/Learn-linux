#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void printsigset(sigset_t *set)
{
    int i = 1;
    for (; i < 32; ++i)
    {
        if (sigismember(set, i))
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }
    }
    putchar('\n');
}

int main()
{
    sigset_t s, p;
    sigemptyset(&s);
    sigaddset(&s, 2);
    sigprocmask(SIG_BLOCK, &s, NULL);
    while (1)
    {
        sigpending(&p);
        printsigset(&p);
        sleep(1);
    }
    return 0;
}
