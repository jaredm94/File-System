#include "level1.h"

// SOURCE: http://www.eecs.wsu.edu/~cs360/unlink.html
/*
(1). get pathname's INODE into memory

(2). verify it's a FILE (REG or LNK), can not be a DIR;

(3). decrement INODE's i_links_count by 1;

(4). if i_links_count == 0 ==> rm pathname by

        deallocate its data blocks by:

     Write a truncate(INODE) function, which deallocates ALL the data blocks
     of INODE. This is similar to printing the data blocks of INODE.

        deallocate its INODE;

(5). Remove childName=basename(pathname) from the parent directory by

        rm_child(parentInodePtr, childName)

     which is the SAME as that in rmdir or unlink file operations.
*/

// Remove link file. If the file it refers to has no more link counts remaining, remove that file too.
int unlink(char *pathname)
{
    int dev, ino;
    MINODE *mip, *pip;
    char new_pathname_copy_1[256], new_pathname_copy_2[256], *new_dir_name, *new_base_name;
    char buf[BLKSIZE];

    // Pathname's inode into memory
    if (pathname[0] == '/') dev = root->dev;
    else dev = running->cwd->dev;
    // Parent
    strcpy(new_pathname_copy_1, pathname);
    strcpy(new_pathname_copy_2, pathname);
    ino = getino(dev, dirname(new_pathname_copy_1));
    pip = iget(dev, ino);
    // Child
    ino = getino(dev, pathname);
    mip = iget(dev, ino);

    // Verify it's a file
    ///if (is_dir_file(mip)) return EXIT_FAILURE;

    // account for syslink
    if (!is_dir_file(mip) && !is_reg_file(mip))
    {
        int origino;
        MINODE *origmip;

        // Decrement original file's link count
        get_block(dev, mip->INODE.i_block[0], buf);
        origino = getino(dev, buf);
        origmip = iget(dev, origino);

        origmip->INODE.i_links_count -= 1;

        // Put it back where you found it, kiddo
        origmip->dirty = 1;
        iput(origmip);
    }

    // Decrement inode's child i_link_counts by 1
    if (mip->INODE.i_links_count > 0) mip->INODE.i_links_count -= 1;

    // Check if we should remove the file
    if (mip->INODE.i_links_count == 0)
    {
        /// Remove pathname (truncate file, deallocate inode, and remove from parent directory)
        // Deallocate all inode blocks
        truncate(mip);
        // Deallocate inode
        idelloc(dev, ino);
    }

    // Remove name from parent directory
    rm_child(pip, basename(new_pathname_copy_2));

    printf(YELLOW "\t...UNLINK(%s)" RESET ": Unlinked file still has references. Doing nothing.\n",
           pathname);

    // Put back child and parent
    mip->dirty = 1;
    iput(mip);
    pip->dirty = 1;
    iput(pip);

    // Update directory access time


    return EXIT_SUCCESS;
}
