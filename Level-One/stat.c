#include "level1.h"

/* EXAMPLE ----------------------------------------------------------------
    File: 'diskimage'
    Size: 1474560   	Blocks: 2888       IO Block: 4096   regular file
    Device: 808h/2056d	Inode: 2098144     Links: 1
    Access: (0644/-rw-r--r--)  Uid: ( 1000/    luke)   Gid: ( 1000/    luke)
    Access: 2016-04-17 18:35:46.438280038 -0700
    Modify: 2016-04-04 11:43:57.000467351 -0700
    Change: 2016-04-04 11:43:57.000467351 -0700
    Birth: -
--------------------------------------------------------------------------*/

// Find inode and print out info
int stat_file()
{
    char *pathname = pathM;

    int dev, i;
    char pathname_cpy[128],
        *filename, filetype[128],
        *t1 = "xwrxwrxwr-------",
        *t2 = "----------------";

    // Get current device
    if(pathname[0] == '/') dev = root->dev;
    else dev = running->cwd->dev;

    // Get inode from disk into memory
    int ino = getino(dev, pathname);
    MINODE *mp = iget(dev, ino); /// TODO: Figure out why this line is causing us not to iput() correctly

    // No file to stat?
    if (mp == NULL) return EXIT_FAILURE;

    // Get basic file info
    strcpy(pathname_cpy, pathname);
    filename = basename(pathname_cpy);

    // NOTE: We only account for regular files and directories
    if (is_dir_file(mp)) strcpy(filetype, "directory file");
    else strcpy(filetype, "regular file");

    // Print info
    printf(YELLOW "\t...STAT(%s)" RESET ": File exists. Listing information...\n",
        pathname);
    printf(MAGENTA);
    printf("\t\t| File: '%s'\n", filename);
    printf("\t\t| Size: %d\tBlocks: %d\tMode: %4x\t%s\n", mp->INODE.i_size, mp->INODE.i_blocks, mp->INODE.i_mode, filetype);
    printf("\t\t| Device: %d\tInode: %d\tLinks: %d\n", mp->dev, mp->ino, mp->INODE.i_links_count);
    printf("\t\t| Access: ");
    for (i = 8; i >= 0; i--)
    {
        if (mp->INODE.i_mode & (1 << i)) printf("%c", t1[i]);
        else printf("%c", t2[i]);
    }
    printf("\tUID: %d\tGID: %d\n", mp->INODE.i_uid, mp->INODE.i_gid);
    printf("\t\t| Access: %d\n", mp->INODE.i_atime);
    printf("\t\t| Modify: %d\n", mp->INODE.i_mtime);
    printf("\t\t| Creation: %d\n", mp->INODE.i_ctime);
    printf(RESET);

    // Put block back to memory
    // NOTE: Everytime you access a file, write it back
    mp->dirty;
    iput(mp);

    // Update file access time
    touch_file(pathname);

    return EXIT_SUCCESS;
}
