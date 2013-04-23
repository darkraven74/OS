#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>


char* files[2] = {"temp1", "temp2"};
char* buf;
int buf_size = 5000;

void run(char** args, int file_id)
{
    int fds[2];
    int k, len = 0;
    pipe(fds);
    int pid = fork();
    if (pid == 0)
    {
        //child
        dup2(fds[1], 1);
        close(fds[0]);
        close(fds[1]);
        execvp(args[0], args);
        exit(0);
    }

    close(fds[1]);
    waitpid(pid, NULL, 0);
    while (1)
    {
        k = read(fds[0], buf + len, buf_size - len);
        if ((buf_size - len) <= 0)
        {
            break;
        }
        if (k == 0)
        {
            break;
        }
        len += k;
    }
    int f1 = open(files[file_id], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    write(f1, buf, len);
    write(1, buf, len); 
}

int main(int argc, char* argv[])
{
    int update = atoi(argv[1]);
    char** args = malloc(argc - 1);
    args[argc - 2] = NULL;
    int i = 0, file_id = 0;
    for (i = 2; i < argc; i++)
    {
        args[i - 2] = argv[i];
    }
    buf = malloc(buf_size);
    run(args, 1); 
    while (1)
    {
        run(args, file_id);
        int pid = fork();
        if (pid == 0)
        {
            //child
            execlp("diff", "diff", files[0], files[1], NULL);
        }
        waitpid(pid, NULL, 0);
        file_id = (file_id++) % 2;
        sleep(update);
    }
    free(args);
    free(buf);
    return 0;
}
