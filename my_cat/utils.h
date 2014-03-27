#include <stdlib.h>
#pragma once

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
