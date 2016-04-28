#include "level1.h"

int change_dir(char *pathname)// this is the equivalent of cd. It sets the current working directy
{
    int ino, dev = running->cwd->dev;
    MINODE *mip = running->cwd;

    if (strlen(pathname) > 0)// find and set it to that i node if the name isnt empty
    {
        ino = getino(dev, pathname);//get the inum

        if (ino < 0)
        {
            printf(YELLOW "\t...CHANGE_DIR(%s)" RESET ": <ERROR> Directory does not exist!\n");
            return EXIT_FAILURE;
        }

//        if (ino == 2)

        //printf("ino = %d", ino);
        mip = iget(dev, ino);// get the block
        // Put cwd back onto disk
        iput(running->cwd);
        // Change cwd to new minode
        running->cwd = mip;

        // Get name of new cwd
//        char absname[256] = {0};
//        getabsname(running->cwd, absname);
//        strcpy(running->cwd->name, absname);
        //strcpy(mip->name, pathname);
    }
    else// set to the root inode if name is empty
    {
        mip = iget(dev, 2);//get the root
        iput(running->cwd);//decrement refCount to cwd
        running->cwd = mip;
    }

    printf(YELLOW "\t...CHANGE_DIR(%s)" RESET ": Changed directories!\n");

    // Update directory access time
    //touch_file(pathname);

    return 0;
}
