#include "level1.h"

// Modify INODE.i_mode's permission bits

int chown_file(int newOwner, char *pathname)
{
    int dev, ino;
    MINODE *mp;

    // Get inode
    if(pathname[0] == '/') dev = root->dev;
    else dev = running->cwd->dev;
    ino = getino(dev, pathname);
    if (ino == EXIT_FAILURE)
    {
        printf("CHMOD: File does not exist\n");
        return EXIT_FAILURE;
    }
    mp = iget(dev, ino);

    // Update owner
    mp->INODE.i_uid = newOwner;

    // Write file back
    mp->dirty = 1;
    iput(mp);

    // Update file access time
    touch_file(pathname);

    return EXIT_SUCCESS;
}
