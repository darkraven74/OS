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

int main(int argc, char* argv[])
{
    int k = get_k(argv[1]);
    char* buf = malloc(k);
    int len = 0;

    while (1)
    {
        read(0, buf, k);
        break;
    }

    write(1, buf, k);
    printf("%d\n", k);

	return 0;
}
