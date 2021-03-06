#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void write_ans(char* s, int first, int last)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        write(1, s + first, last - first + 1);
    }
}

void* my_malloc(size_t size)
{
    void* result = malloc(size);
    if (result == NULL)
    {
        exit(1);
    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        return 1;
    }
    int k = atoi(argv[1]) + 1;
    if (k < 2)
    {
        return 2;
    }
    char* buf = my_malloc(k);
    int len = 0, count, eof = 0, ignore = 0;

    while (!eof)
    {
        count = read(0, buf + len, k - len);
        if (count < 1)
        {
            eof = 1;		
            if (len > 0)
            {
                buf[len] = '\n';
                len++;
            }
        }
        len += count;

        int cur = 0, start = 0;
        while (cur < len)
        {
            if (buf[cur] == '\n')
            {
                if (!ignore)
                {
                    write_ans(buf, start, cur);
                }
                start = cur + 1;
                ignore = 0;
            }
            cur++;
        }

        len -= start;
        memmove(buf, buf + start, len);
        if (len == k)
        {
            len = 0;
            ignore = 1;
        }	
    }

    free(buf);
    return 0;
}
