#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

void* my_malloc(size_t size)
{
    void* result = malloc(size);
    if (result == NULL)
    {
        exit(1);
    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        return 1;
    }
    int update = atoi(argv[1]);
    if (update < 1)
    {
        return 2;    
    }
    
    const char* f_old = "file_old";
    const char* f_new = "file_new";
 
    char** args = my_malloc(sizeof(char*) * (argc - 1));
    memcpy(args, argv + 2, sizeof(char*) * (argc - 2));
    args[argc - 2] = NULL;
    
    int buf_size = 5000;
    char* buf = my_malloc(buf_size);
    char* old_buf = my_malloc(buf_size);
    int old_len = 0;
        
    while (1)
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
        do
        {
            k = read(fds[0], buf + len, buf_size - len);
            len += k;
        } while (k > 0 && buf_size - len != 0);
        close(fds[0]);
        
        int fd_old = open(f_old, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        int fd_new = open(f_new, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        write(fd_old, old_buf, old_len);
        write(fd_new, buf, len);
        write(1, buf, len);  
        close(fd_old);
        close(fd_new);       
        
        memcpy(old_buf, buf, len);
        old_len = len;
        
        pid = fork();
        if (pid == 0)
        {
            //child
            execlp("diff", "diff", "-u", f_old, f_new, NULL);
            exit(0);
        }
        waitpid(pid, NULL, 0);
        sleep(update);
    }
    free(args);
    free(buf);
    free(old_buf);
    return 0;
}
