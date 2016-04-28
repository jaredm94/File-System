#include "level1.h"

// Modify INODE.i_mode's permission bits
// You may specify newMode in Octal form, e.g. 0644 (or you may use +x, -r, +w, etc fancier formats.)

// Called like: chmod 640 file1
// r = 4, w = 2, x = 1
int chmod_file()
{
    /// PARAMS
    int newMode = atoi(pathM);
    char *pathname = parameterM;
    
    int i, dev, ino;
    MINODE *mp;

    // Convert newMode to octal
    int uid, gid, oid, octalPerm;
    oid = newMode % 10;
    gid = (newMode % 100 - oid) / 10;
    uid = ((newMode % 1000) - gid - oid) / 100;
    newMode = (oid * 1) + (gid * 8) + (uid * 64);

    // Get inode
    if(pathname[0] == '/') dev = root->dev;
    else dev = running->cwd->dev;
    ino = getino(dev, pathname);
    if (ino == EXIT_FAILURE)
    {
        printf(YELLOW "\t...CHMOD(%d, %s)" RESET ": File does not exist\n", newMode, pathname);
        return EXIT_FAILURE;
    }
    mp = iget(dev, ino);

    // Wipe past permission bits
    int octalmask = 01000, pastMode = mp->INODE.i_mode;
    mp->INODE.i_mode -= mp->INODE.i_mode % octalmask;
    // Insert new permissions
    mp->INODE.i_mode += newMode;
    printf(YELLOW "\t...CHMOD(%d, %s)" RESET ": Mode updated from %o to %o\n", newMode, pathname, pastMode, mp->INODE.i_mode);

    // Write file back
    mp->dirty = 1;
    iput(mp);

    // Update file access time
    touch_file(pathname);

    return EXIT_SUCCESS;
}
