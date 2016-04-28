#include "level2.h"

int _lseek()
{
    //From fd, find the OFT entry.
/****PARAMETERS****/
int fd = atoi(pathM);
 int position = atoi(parameterM);
/****PARAMETERS****/
// change OFT entry's offset to position but make sure NOT to over run either end
    //of the file.
    if(position > running->fd[fd]->inodeptr->INODE.i_size || position < 0)
    {
        printf("\nERROR: BAD OFFSET VALUE RETURNING FROM LSEEK.\n");
        return -1;
    }

    running->fd[fd]->offset = position;

    return position;
}
