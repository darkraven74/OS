#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


char* files[2] = {"temp1", "temp2"};

void run(char** args, int file_id)
{



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
    return 0;
}
