#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <wait.h>
#include <string.h>

char* itoa(int value, char* result, int base) {
    
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}


int main(int argc, char* argv[])
{

    char* fds[argc + 1];
    int fds_size = 100;
    int i;

    for (int i = 1; i < argc; i++)
    {
        fds[i] = (char*)malloc(fds_size);
        int fd = 0;
        if (i % 2 == 1)
        {
            fd = open(argv[i], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        }
        else
        {
            fd = open(argv[i], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        }
        itoa(fd, fds[i], 10);
    }
   
    fds[argc] = NULL;
    fds[0] = (char*) "./propagater\0";
    int pid = fork();
    if (pid == 0)
    {
        //execlp(fds[0], fds[0], fds[1], fds[2], 0);
        execvp(fds[0], fds);
        exit(0);

    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}
