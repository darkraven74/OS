#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int my_open(const char* pathname, int flags, mode_t mode)
{
    int result = open(pathname, flags, mode);
    if (result == -1)
    {
        perror("open error");
        exit(1);
    }
    return result;
}

int my_close(int fd)
{
    int result = close(fd);
    if (result == -1)
    {
        perror("close error");
        exit(1);
    }
    return result;
}

int main(int argc, char* argv[])
{
    int BUF_SIZE = 4096;
    if (argc < 3)
    {
        perror("wrong arguments count");
        exit(1);
    }
    char* args[argc];
    args[0] = argv[1];
    args[argc - 1] = NULL;
    for (int i = 2; i < argc; i++)
    {
        int fd = my_open(argv[i], O_RDONLY, S_IRUSR);
        args[i - 1] = (char*)my_malloc(BUF_SIZE);
        sprintf(args[i - 1], "%d", fd);
    }
    int pid = fork();
    if (pid == -1)
    {
        perror("fork error");
        exit(1);
    }
    if (!pid)
    {
        int result = execvp(args[0], args);
        if (result == -1)
        {
            perror("exec error");
            exit(1);
        }
        exit(0);
    }
    pid_t result = waitpid(pid, NULL, 0);
    if (result == -1)
    {
        perror("waitpid error");
        exit(1);
    }
    for (int i = 2; i < argc; i++)
    {
        my_close(atoi(args[i - 1]));
        free(args[i - 1]);
    }
    return 0;
}
