#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


char* file1 = "temp1";
char* file2 = "temp2";

int main(int argc, char* argv[])
{
    int update = atoi(argv[1]);
    char** args = malloc(argc - 1);
    args[argc - 2] = NULL;
    int i = 0;
    for (i = 2; i < argc; i++)
    {
        args[i - 2] = argv[i];
    }
    while (1)
    {
        int pid = fork();
        if (pid == 0)
        {
            //child
            execlp("diff", "diff", file1, file2, NULL);
        }
        waitpid(pid, NULL, 0);

        sleep(update);
    }
    return 0;
}
