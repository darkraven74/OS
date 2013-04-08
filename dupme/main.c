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

int main(int argc, char* argv[])
{
    int k = get_k(argv[1]) + 1;
    char* buf = malloc(k);
    int len = 0, count, eof = 0;


    while (1)
    {
        count = read(0, buf + len, k - len);
        if (count == 0)
        {
            eof = 1;
        }



        if (k == len)
        {

        }
    }



	return 0;
}
