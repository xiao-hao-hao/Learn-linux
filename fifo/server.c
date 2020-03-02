#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//the path of fifo
#define _FIFO_ "./myfifo"
int main()
{
    int fd = open(_FIFO_, O_WRONLY);
    if (fd < 0)
    {
        printf("open error!\n");
        return 2;
    }
    char buf[1024];
    while (1)
    {
        printf("Please Enter# ");
        fflush(stdout);
        ssize_t s = read(0, buf, sizeof(buf) - 1);
        if (s > 0)
        {
            buf[s] = 0;
            write(fd, buf, s);
        }
    }
    close(fd);
    return 0;
}
