#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <boost/optional.hpp>

using namespace std;


struct LazyStream
{

    size_t buf_size;
    char delim;
    int buf_len = 0;
    vector<char> buf;
    vector<char> out_buf;
    int fd;
    int ignore = 0;


    LazyStream(int _fd, size_t _buf_size, char _delim) : fd(_fd), buf_size(_buf_size), delim(_delim)
    {
        buf = vector<char> (buf_size);
        out_buf = vector<char> (buf_size);
    }
    

    boost::optional<vector<char>> lazy_read()
    {

        vector<char> temp;
        vector<char> temp2;
        int was_del = 0;
        while (1)
        {
            int count = read(fd, buf.data() + buf_len, buf_size - buf_len);
            int cur = 0;
            buf_len += count;
            while (cur <= buf_len)
            {
                if (buf[cur] == delim)
                {
                   was_del = 1;     
                   if (!ignore)
                   {
                       temp = buf;
                       memmove(temp.data(), temp.data() + cur, buf_size - cur);
                       buf.resize(cur);
                       temp2 = move(buf);
                       buf = move(temp);
                       buf_len -= cur;
                       boost::optional<vector<char>> x(temp2);
                       return x;
                   }
                   else 
                   {
                       ignore = 0;
                       memmove(buf.data(), buf.data() + cur, buf_size - cur);
                       buf_len -= cur;
                       boost::optional<vector<char>> x;
                       return x;
                   } 
                }
                cur++;
            } 
            if (!was_del)
            {
                ignore = 1;
                buf_len = 0;
            }
        }
    } 
    
    void lazy_write(vector<char>& src)
    { 
        int start = 0;
        while (start < src.size())
        {
            int len = std::min(buf_size, src.size() - start); 
            write(fd, src.data() + start, len);
            start += len;
        }
    }
};


int main(int argc, char* argv[])
{
    int fd1 = open("f1", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    LazyStream str(fd1, 10000, '\n');
    vector<char> src(3);
    src[0] = 'a';
    src[1] = 'b';
    src[2] = '\n';
    //str.lazy_write(src);
    boost::optional<vector<char>> gg = str.lazy_read();
    if (gg.is_initialized())
    {
       vector<char> temp = gg.get();
       for (int i = 0; i < temp.size(); i++)
       {
           printf("%c", temp[i]);
       }

    } 
    printf("\n");
     
   
    
   
    
        
    return 0;
}
