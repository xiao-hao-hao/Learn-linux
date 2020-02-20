#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX 1024
using namespace std;

int main()
{
    char *myargv[MAX];
    char cmd[MAX];
    char *p = cmd;
    int i = 0;
    while (1)
    {
        pid_t id = fork();
        if (id == 0)
        {
            //i = 0;
            cout << "[zby@centos shell]#";
            gets(cmd);
            myargv[i] = cmd;
            while (*p)
            {
                if (isspace(*p))
                {
                    *p = '\0';
                    ++p;
                    myargv[++i] = p;
                    continue;
                }
                ++p;
            }
            myargv[++i] = NULL;
            execvp(myargv[0], myargv);
            exit(1);
        }
        else if (id > 0)
        {
            waitpid(id, NULL, 0);
        }
        else
        {}
    }
    return 0;
}
