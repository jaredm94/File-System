#include "level2.h"

/*
HOW TO cp ONE file:
    cp src dest:
    1. fd = open src for READ;
    2. gd = open dst for WR|CREAT;
       NOTE:In the project, you may have to creat the dst file first, then open it
            for WR, OR  if open fails due to no file yet, creat it and then open it
            for WR.
    3. while( n=read(fd, buf[ ], BLKSIZE) ){
           write(gd, buf, n);  // notice the n in write()
       }
*/

int cpCmd()
{
    /****PARAMTERS****/
    char *src = pathM;
    char *dest = parameterM;
    cp(src,dest);

    }



// SOURCE: http://www.eecs.wsu.edu/~cs360/write_ext2.html
int cp(char * src, char * dest)
{
    int fd, gd, n;
    char buf[BLKSIZE];

    /// QUESTION: How do you open a file with particular mode

    // Open src for READ
    if(fd = open_file(src,0)==-1)
    {
        printf("\nERROR: INVALID SOURCE.\n");
        return -1;
    }

    // Attempt to make destination file, just in case it doesn't exist
    if(getino(running->cwd->dev,dest)==-1)
    {
        creat_file(dest);
    }
    else// set the size to zero
    {

        int ino = getino(running->cwd->dev,dest);
        MINODE * mip = iget(running->cwd->dev,ino);
        mip->dirty = 1;
        mip->INODE.i_size = 0;
        iput((mip));
    }
    // Open dest for WR|CREAT
    gd = open_file(dest,2);

    // Read source block into buffer
    while (n = myread(fd, buf, BLKSIZE))
    {
        // Write buffer to destination block
        mywrite(gd, buf, n);
    }

    close_file(dest);
    close_file(src);
}
