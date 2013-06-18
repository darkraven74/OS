#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <sys/types.h>

using namespace std;

int main(int argc, char* argv[])
{
   
    vector<int> del;
    int n = atoi(argv[1]);
    if (n < 1)
    {
        return 1;
    }
    int buf_size = 1000;
    char* buf = (char*) malloc(buf_size);
    int len = 0;
    while (1)
    {


        int i;
        int count = read(0, buf + len, buf_size - len);
        len += count;
        del.resize(0);
        for (i = 0 ; i < len; i++)
        {
            if (buf[i] == '\n')
            {
                del.push_back(i);
            }
        }

        if ((int)del.size() > n - 1)
        {
            count = write(1, buf, del[n - 1] + 1);
            memmove(buf, buf + del[n - 1] + 1, buf_size - count);
            len -= count;
            sleep(1);
        }
        else
        {
            count = write(1, buf, len);
            len = 0;
        }





    }
    
        
    free(buf);
    return 0;
}
