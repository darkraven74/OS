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
          	    //...
				continue;	  
            }
            if (fd[i].revents & POLLIN) 
			{
            	char* buf = (char*)my_malloc(buf_size);   
				my_read(fd[i].fd, buf, buf_size);				
			
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
