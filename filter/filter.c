#include <unistd.h>
#include <stdlib.h>

int get_k(char* s)
{
    int ans = 0;
    int cur = 0;
    while (s[cur] != 0)
    {
        ans = ans * 10 + s[cur] - '0';
        cur++;
    }
    return ans;
}

int has_endl(char* s, int last)
{
    int cur = 0;
    while ((s[cur] != 0) && (cur < last))
    {
        if (s[cur] == '\n')
        {
            return cur;
        }
        cur++;
    }

    return -1;
}

void write_ans(char* s, int last)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        write(1, s, last);
        char c = '\n';
        write(1, &c, 1);
    }
}

int main(int argc, char* argv[])
{
    int k = get_k(argv[1]) + 1;
    char* buf = malloc(k);
    int len = 0, count, eof = 0, temp;

    while (1)
    {
        count = read(0, buf + len, k - len);
        if (count == 0)
        {
            eof = 1;
        }
        len += count;
        temp = has_endl(buf, len);
        if (temp != -1)
        {
            write_ans(buf, temp);
            if (temp < len - 1)
            {
                memmove(buf, buf + temp + 1, len - temp - 1);
                len = len - temp - 1;
            }
            else
            {
                len = 0;
            }
        }
        else
        {
            if ((len > 0) && (eof == 1))
            {
                write_ans(buf, len);
                break;
            }
        }

        if (k == len)
        {
            len = 0;
            while (1)
            {
                count = read(0, buf + len, k - len);
                if (count == 0)
                {
                    eof = 1;
                    break;
                }
                len += count;
                temp = has_endl(buf, len);
                if (temp != -1)
                {
                    memmove(buf, buf + temp + 1, len - temp - 1);
                    len = len - temp - 1;
                    break;
                }
                if (k == len)
                {
                    len = 0;
                }
            }
        }
        if (eof == 1)
        {
            break;
        }
    }

    free(buf);
    return 0;
}
