#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//the path of fifo
#define _FIFO_ "./myfifo"
int main()
{
    if (mkfifo(_FIFO_, 0644) < 0)
    {
        perror("mkfifo");
        return 1;
    }
    int fd = open(_FIFO_, O_RDONLY);
    if (fd < 0)
    {
        printf("open error!\n");
        return 2;
    }
    char buf[1024];
    while (1)
    {
        ssize_t s = read(fd, buf, sizeof(buf) - 1);
        if (s > 0)
        {
            buf[s] = '\0';
            printf("client->server# %s\n", buf);
        }
        else
        {
            break;
        }
    }
    printf("server quit...\n");
    close(fd);
    return 0;
}
