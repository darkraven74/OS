#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <functional>
#include <vector>
#include <sys/wait.h>
#include <signal.h>
#include <poll.h>
#include <string>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <memory.h>


using namespace std;

int pid;
int buf_size = 1000;

struct state
{
	int msg = 0; //0 1 2 3
	string result;
};

void foo(int)
{
	kill(pid, SIGINT);
}

void* my_malloc(size_t size)
{
    void* result = malloc(size);
    if (result == NULL)
    {
        perror("error malloc");
        exit(1);
    }
    return result;
}

void my_close(int fd)
{
    if (close(fd) == -1)
	{
        perror("error close");
		exit(1);
	}
}

int my_read(int fd, char *buf, size_t len)
{
    int count = read(fd, buf, len);
    if (count < 0)
    {
        perror("error read");
        exit(1);
    }
    return count;
}

int evaluate(int x, int y)
{
	//-1 first; 0 - draw; 1 - second
	if (x == y)
	{
		return 0;
	}
	if (((x == 1) && (y == 2)) || ((x == 2) && (y == 3)) || ((x == 3) && (y == 1)))
	{
		return -1;
	}
	return 1;
}

int find(int x)
{
	if (x % 2 == 0)
	{
		return x - 1;
	}
	return x + 1;
}


int main(int argc, char* argv[]) 
{
    if ((pid = fork())) 
	{
        signal(SIGINT, &foo);
        wait(NULL);
        return 0;
    }
    setsid();
    addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;    
	hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    addrinfo *result;
    if (getaddrinfo(NULL, "8844", &hints, &result) != 0) 
	{
        perror("error getaddrinfo");
        return 1;
    }

    if (result == NULL) 
	{
        perror("error result");
        return 1;
    }

    int socket_fd = socket(result->ai_family, result->ai_socktype,
            result->ai_protocol);
    if (socket_fd == -1) 
	{
        perror("error socket");
        return 1;
    }

    int optval = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    if (bind(socket_fd, result->ai_addr, result->ai_addrlen) == -1) 
	{
        perror("error bind");
        return 1;
    }

    int count_u = 10;
    if (listen(socket_fd, count_u) == -1) 
	{
        perror("Error in listen");
        return 1;
    }

	vector<pollfd> fd(1);    
    fd[0].fd = socket_fd;
    fd[0].events = POLLIN;

    const int timeout = -1;
	vector<state> data(1);
    while(true) 
	{
        int c = poll(fd.data(), fd.size(), timeout);
        if (c == -1) 
		{
            perror("error poll");
            return 1;
        }
        for (int i = 1; i < data.size(); i++) 
		{
            if (fd[i].revents & (POLLERR | POLLHUP | POLLRDHUP)) 
			{
          		int j = find(i);
				if (i > j)
				{
					swap(i, j);
				}
				my_close(fd[i].fd);
				my_close(fd[j].fd);
				swap(fd[i], fd[fd.size() - 2]);
				swap(fd[j], fd[fd.size() - 1]);
				swap(data[i], data[data.size() - 2]);
				swap(data[j], data[data.size() - 1]);
				fd.resize(fd.size() - 2);
				data.resize(data.size() - 2);
				continue;	  
            }
            if (fd[i].revents & POLLIN) 
			{
            	char* buf = (char*)my_malloc(buf_size);   
				my_read(fd[i].fd, buf, buf_size);				
				if (data[i].msg)
				{
					data[i].result = "wait\n";
					fd[i].events |= POLLOUT;
				}
				else
				{
					data[i].msg = atoi(buf);
				}
				int j = find(i);
				if (data[j].msg)
				{
					int res = evaluate(data[i].msg, data[j].msg);
					if (res == 0)
					{
						data[i].result = "draw\n";
						data[j].result = "draw\n";	
					}					
					if (res == -1)
					{
						data[i].result = "win\n";
						data[j].result = "loose\n";	
					}
					if (res == 1)
					{
						data[i].result = "loose\n";
						data[j].result = "win\n";	
					} 
					fd[i].events |= POLLOUT;
					fd[j].events |= POLLOUT;
                    data[i].msg = 0;
                    data[j].msg = 0;
				}
				free(buf);
				
			
				
            }
            if (fd[i].revents & POLLOUT) 
			{
				int count = write(fd[i].fd, data[i].result.c_str(), data[i].result.length());
				fd[i].events = POLLIN | POLLRDHUP;
            }
        }
        if (fd[0].revents & POLLIN) 
		{
            printf("new client\n");
            int fd_ac = accept(socket_fd, result->ai_addr,
                    &result->ai_addrlen);
            if (fd_ac == -1) 
			{
                perror("error accept");
                return 1;
            }
			pollfd t;
			t.fd = fd_ac;
			t.events = POLLIN | POLLRDHUP;
			fd.push_back(t);
			data.resize(fd.size());
        }
    }
}
