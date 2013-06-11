#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

int main(int argc, char* argv[])
{
    
    printf("prop start!\n");
    struct pollfd fds[argc - 1];
    int i;
    for (i = 1; i < argc; i++)
    {
        fds[i - 1].fd = atoi(argv[i]);
        if (i % 2 == 1)
        {
           fds[i - 1].events = POLLIN | POLLERR;
        }
        else
        {
            fds[i - 1].events = POLLERR;
        }
        fds[i - 1].revents = 0;
    }

    int work = argc - 1;
    int alive[argc - 1];
    for (i = 0; i < argc - 1; i++)
    {
        alive[i] = 1;
    }

    char* buf[argc / 2];
    int buf_len[argc / 2];
    int buf_size = 5000;
    for (i = 0; i < argc / 2; i++)
    {
       buf[i] = malloc(buf_size);
       buf_len[i] = 0;
    } 

    while(work > 0)
    {
        poll(fds, argc - 1, -1);

        //printf("work--- %d\n", work);
        //printf("events: --%d %d----\n", fds[0].events, fds[1].events);
        //printf("rev: --%d %d-----\n", fds[0].revents, fds[1].revents);


        for (i = 0; i < argc - 1; i++)
        {
            if (fds[i].revents != 0)
            {
				if (fds[i].revents & POLLERR)
                {
                    printf("pollerr\n");
                    if (i % 2 == 0)
                    {
                        fds[i].events = 0;
                        fds[i + 1].events = 0;
                        alive[i] = 0;
                        alive[i + 1] = 0;
                        work -= 2;
                    }
                    else
                    {
                        fds[i].events = 0;
                        fds[i - 1].events = 0;
                        alive[i] = 0;
                        alive[i - 1] = 0;
                        work -= 2;
                    }
                    
                }

                if (fds[i].revents & POLLIN)
                {
                   printf("readin to buf\n");
                   int count = read(fds[i].fd, buf[i / 2] + buf_len[i / 2],
                        buf_size - buf_len[i / 2]);
                   //printf("count: %d\n", count);
                   if (count == 0)
                   {
                       work--;
                       alive[i] = 0;
                       fds[i].events = 0;
                   }
                   if (count == 0 && buf_len[i / 2] == 0)
                   {
                       work--;
                       alive[i + 1] = 0;
                       fds[i + 1].events = 0;
                   }
                   buf_len[i / 2] += count;
                   if (buf_len[i / 2] == buf_size)
                   {
                       fds[i].events = POLLERR;
                   }
                   if (buf_len[i / 2] > 0)
                   {
                       printf("pollout added\n");
                       fds[i + 1].events = POLLOUT | POLLERR;
                       //printf("111 %d \n", fds[i + 1].events);
                   }
                }
                if (fds[i].revents & POLLOUT)
                {
                    printf("writing to fds\n");
                    int count = write(fds[i].fd, buf[i / 2], buf_len[i / 2]);
                    buf_len[i / 2] -= count;
                    memcpy(buf[i / 2], buf[i / 2] + count, buf_len[i / 2]);
                    if (buf_len[i / 2] < buf_size && alive[i - 1])
                    {
                        fds[i - 1].events |= POLLIN;
                    }
                    if (buf_len[i / 2] == 0)
                    {
                        fds[i].events = POLLERR;
                    }
                    if (buf_len[i / 2] == 0 && alive[i - 1] == 0)
                    {
                        fds[i].events = 0;
                        alive[i] = 0;
                        work--;
                    }
                }
                
                fds[i].revents = 0;

            }

        }

    }
        
    return 0;
}
