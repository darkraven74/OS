#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <sys/types.h>

using namespace std;


struct msg
{
    vector<char> pref;
    int end;
    int args;
    int empty = 1;
    vector<vector<char>> data;
    msg(vector<char> _pref): pref(_pref) {}

    msg() {}

    msg(char* _pref)
    {
        for (int i = 0; i < strlen(_pref); i++)
        {
            pref.push_back(_pref[i]);
        }

    }


    void print()
    {
        if (empty)
        {
            printf("NULL\n");
            return;
        }
        for (int i = 0; i < pref.size(); i++)
        {
            printf("%c", pref[i]);
        }
        printf(" ");
        for (int i = 0; i < args; i++)
        {
            for (int j = 0; j < data[i].size(); j++)
            {
                printf("%c", data[i][j]);

            } 

            printf(" ");

        }

        
        printf("\n");
    }

    void parse(int x, char* buf)
    {
        args = 0;
        if (buf[x - 1] <= '9' && buf[x - 1] >= '0')
        {
            args = buf[x - 1] - '0';
        }
        int i;
        data.resize(args);
        int cur = x;
        for (i = 0; i < args; i++)
        {
            while (buf[cur] != ' ')
            {
                data[i].push_back(buf[cur]);
                cur++;
            }
            cur++;
        }
        
        end = cur - 1;
    }
};

struct msgstream
{
    int buf_size = 1000;
    int len = 0;
    int fd;
    vector<msg> cmd;
    char* buf;
    msgstream(int _fd, vector<msg> _cmd): fd(_fd), cmd(_cmd) 
    {
        buf = (char*) malloc(buf_size);
    } 

    ~msgstream()
    {
        free(buf);
    }

    msg getMessage()
    {
        int count = read(fd, buf + len, buf_size - len);
        int i = 0;
        len += count;
        while (i < len)
        {
            int j = 0;
            while (j < cmd.size())
            {
                int cur = 0;
                int tmp = i;
                int err = 0;
                while (cur < cmd[j].pref.size())
                {
                    if (buf[tmp] != cmd[j].pref[cur])
                    {
                        j++;
                        err = 1;
                        break;
                    }
                    cur++;
                    tmp++;
                }
                if (!err)
                {
                    msg ans(cmd[j].pref); 
                    ans.parse(tmp, buf);
                    memmove(buf, buf + ans.end + 1, buf_size - ans.end - 1);
                    len -= ans.end;
                    len--;
                    ans.empty = 0;
                    return ans;
                }
            }
            i++;
        }

        len = 0;
        msg ans;
        return ans;
    }
    
};
int main(int argc, char* argv[])
{
       
    vector<msg> data;
    msg m("ping");
    data.push_back(m);
    msg m1("pong");
    data.push_back(m1);
    msg m2("msg1");
    data.push_back(m2);

    msgstream stream(0, data);  

    while (1)
    {
        msg m3 = stream.getMessage();
        m3.print();

    }

    
        
    return 0;
}
