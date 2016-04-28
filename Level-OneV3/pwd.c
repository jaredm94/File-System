#include "level1.h"

// NOTE: int types are preferred to void types, according to KC

int pwd()//prints the current workign directory of the process.
{
    // If current working directory has no name (like root)
    if (strlen(running->cwd->name) == 0)
    {
        char absname[256] = {0};
        getabsname(running->cwd, absname);
        strcpy(running->cwd->name, absname);
    }

    printf(YELLOW "\t...PWD()" RESET ": '%s'\n", running->cwd->name);
}

