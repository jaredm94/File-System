#include "level1.h"

// Find inode
//      - If inode exists, update time field
//      - Else, create new file with given name
int touch_file(char *pathname)
{
    int dev;

    // Get current device (absolute or relative)
    if(pathname[0] == '/') dev = root->dev;
    else dev = running->cwd->dev;

    // Attempt getting ino
    int ino = getino(dev, pathname);
    if (ino < 0)
    {
            printf(YELLOW "\t...TOUCH_FILE(%s)" RESET ": File didn't exist. Creating file...\n", pathname);

            // Create file
            creat_file(pathname);

            return EXIT_FAILURE;
    }

    // Get in-memory inode from ino
    MINODE *mp = iget(dev, ino);

    // Update inode time to current
    mp->INODE.i_atime = mp->INODE.i_mtime = time(0L);

    printf(YELLOW "\t...TOUCH_FILE(%s)" RESET ": Time updated to %d\n", pathname, mp->INODE.i_atime);

    // Write file back to disk
    mp->dirty = 1;
    iput(mp);

    return EXIT_SUCCESS;
}
