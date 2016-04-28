#include "level2.h"

/*
    This function displays the currently opened files as follows:

        fd     mode    offset    INODE
       ----    ----    ------   --------
         0     READ    1234   [dev, ino]
         1     WRITE      0   [dev, ino]
      --------------------------------------
      to help the user know what files has been opened.
*/
int pfd()
{
    int i;
    printf(RESET BOLDCYAN "\n\t\tfd\tmode\toffset\tINODE\n" RESET CYAN);
    printf("\t\t----------------------------------\n");
    for (i = 0; i < NFD; i++)
    {
        if (running->fd == 0) break;
        if (running->fd[i]->inodeptr != 0) // This is a good way to tell if FD isn't used
        {
            printf("\t\t%d", i);
            printf("\t%d", running->fd[i]->mode);
            printf("\t%d", running->fd[i]->offset);
            printf("\t[%d, %d]\n", running->fd[i]->inodeptr->dev, running->fd[i]->inodeptr->ino);
        }
        else break;
    }
    printf("\t\t----------------------------------\n" CYAN);
    return EXIT_SUCCESS;
}
