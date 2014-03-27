#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "utils.h"

int my_read(int fd, char* buf, size_t len)
{
    int count = read(fd, buf, len);
    if (count < 0)
    {
        perror("read error");
        exit(1);
    }
    return count;
}

int my_write(int fd, char* buf, size_t len)
{
    int total = 0;
    while (total < len)
    {
        int count = write(fd, buf, len);
        if (count < 0)
        {
            perror("write error");
            exit(1);
        }
        total += count;
    }
    return total;
}

int main(int argc, char* argv[])
{
    int BUF_SIZE = 4096;
    char* buf = (char*)my_malloc(BUF_SIZE);
    for (int i = 1; i < argc; i++)
    {
        int fd = atoi(argv[i]);
        while (1)
        {
            int count = my_read(fd, buf, BUF_SIZE);
            if (!count)
            {
                break;
            }
            my_write(1, buf, count);
        }
    }
    free(buf);
    return 0;
}
