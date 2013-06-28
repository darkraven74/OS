#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char* argv[])
{
    struct addrinfo hints;
    struct addrinfo *result; 

    memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE;   
	hints.ai_protocol = 0;     

    int res = getaddrinfo(NULL, "8822", &hints, &result);
	if (res != 0) {
	   fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
	   exit(EXIT_FAILURE);
	}

	int sfd = socket(result->ai_family, result->ai_socktype,
		       result->ai_protocol);

    if (sfd == -1) {
        perror("socket");
    }

    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, 
            &optval, sizeof(optval));


    if (bind(sfd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind");
    }


    if (listen(sfd, 5) == -1) {
        perror("listen");
    }
    
    while (1) {
        int cfd = accept(sfd, NULL, NULL);
        pid_t pid = fork();
        if (!pid) {
            dup2(cfd, 0);
            dup2(cfd, 1);
            dup2(cfd, 2);
            close(cfd);
            close(sfd);
            printf("Hello\n");
            exit(0);
        }
        exit(0);
    }
    return 0;
}
