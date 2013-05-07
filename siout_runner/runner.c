#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sys/wait.h>
using namespace std;


int k = 4096;
char* data[100];
int data_len = 0;   
vector<int> pids;



void run()
{

    char* data2[data_len - 2];
    for (int i = 1; i < data_len - 1; i++)
    {
        data2[i - 1] = data[i];
    }

    int pid = fork();
    if (pid == 0)
    {
        //child
        int file_i = open(data[0], O_RDONLY, S_IRUSR);
        int file_o = open(data[data_len - 1], O_WRONLY | O_CREAT, S_IWUSR);  
        dup2(file_o, 1);
        close(file_o);
        dup2(file_i, 0);
        close(file_i);
        execvp(data2[0], data2);
        exit(0);
    }

    pids.push_back(pid);


}



int has_endl(char* s, int last)
{
    int cur = 0;
    if (s[0] == '\0')
    {
        return -1;
    }
    while (cur <= last)
    {
        if (s[cur] == '\0')
        {
            return cur;
        }
        cur++;
    }

    return -1;
}

int has_endl_endl(char* s, int last)
{
    int cur = 0;
    while (cur < last - 1)
    {
        if (s[cur] == '\0' && s[cur + 1] == '\0')
        {
            return cur;
        }
        cur++;
    }

    return -1;
}



void write_ans_2(char* s, int last)
{
    int j = 0;
    data_len = 0;
    while (1)
    {

        int i = has_endl(s, last);
        if (i == -1)
        {
            break;
        }
        memcpy(data[j], s, i);
        j++;
        data_len++;
        memmove(s, s + i + 1, last - i);  
    } 
    run();
}



int main(int argc, char* argv[])
{
    char* input = argv[1];
    int f1 = open(input, O_RDONLY, S_IRUSR);
    char* buf = (char*)malloc(k);

    for (int  i = 0; i < 100; i++)
    {
        data[i] = (char*)malloc(100000);
    } 
    int len = 0, count, eof = 0, temp;

    while (1)
    {
        count = read(f1, buf + len, k - len);
        if (count == 0)
        {
            eof = 1;
        }
        len += count;
        temp = has_endl_endl(buf, len);
        if (temp != -1)
        {

            write_ans_2(buf, temp);
            if (temp < len - 1)
            {
                memmove(buf, buf + temp + 2, len - temp - 2);
                len = len - temp - 2;
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
                write_ans_2(buf, len);
                break;
            }
        }

        if (k == len)
        {
            len = 0;
            while (1)
            {
                count = read(f1, buf + len, k - len);
                if (count == 0)
                {
                    eof = 1;
                    break;
                }
                len += count;
                temp = has_endl_endl(buf, len);
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

    for (int i = 0; i < pids.size(); i++)
    {
        waitpid(pids[i], NULL, 0);
    }
    printf("%s\n", data[0]);
    printf("%s\n", data[1]);
    printf("%s\n", data[2]);
    free(buf);
    return 0;

}
