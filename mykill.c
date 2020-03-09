#include <stdio.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s pid sigid\n", argv[0]);
        return 1;
    }
    kill(atoi(argv[1]), atoi(argv[2]));
    return 0;
}
