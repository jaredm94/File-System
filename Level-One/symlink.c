#include "level1.h"
int symlinkCmd()
{
    char *old_pathname = pathM;
    char *new_pathname = parameterM;
    symlink(old_pathname,new_pathname);
    }
// Creates symbolic link from file described in new_pathname to file in old_pathname
int symlink(char *old_pathname, char *new_pathname)
{


    int dev, old_ino, old_pino, new_ino, new_pino;
    MINODE *old_mip, *old_pmip, *new_mip, *new_pmip;
    char old_pathname_copy[256], old_pathname_copy_2[256],
        new_pathname_copy_1[256], new_pathname_copy_2[256],
        *old_dir_name, *old_base_name, *new_dir_name, *new_base_name;
    char old_abspathname[256] = {0};

    // Get running device
    if (old_pathname[0] == '/') dev = root->dev;
    else dev = running->cwd->dev;

    // Old pathname inode
    old_ino = getino(dev, old_pathname);
    old_mip = iget(dev, old_ino);

    // Verify old_pathname exists
    if (old_ino < 0) return EXIT_FAILURE;

    // Get old basename
    strcpy(old_pathname_copy, old_pathname);
    old_base_name = basename(old_pathname_copy);

    /// Get old absolute pathname
    strcpy(old_pathname_copy_2, old_pathname);
    old_pino = getino(dev, dirname(old_pathname_copy_2));
    old_pmip = iget(dev, old_pino);
    getabsname(old_pmip, old_abspathname);
    strcat(old_abspathname, "/");
    strcat(old_abspathname, old_base_name);

    // Splitup new pathname
    strcpy(new_pathname_copy_1, new_pathname);
    strcpy(new_pathname_copy_2, new_pathname);
    new_dir_name = dirname(new_pathname_copy_1);
    new_base_name = basename(new_pathname_copy_2);

    // Does new parent exist and is a directory?
    new_pino = getino(dev, new_dir_name);
    if (new_pino < 0) return EXIT_FAILURE;
    new_pmip = iget(dev, new_pino);
    if (!is_dir_file(new_pmip)) return EXIT_FAILURE;

    // Does new file already exist?
    new_ino = getino(dev, new_pathname);
    if (new_ino >= 0) return EXIT_FAILURE;

    printf(YELLOW "\t...SYMLINK(%s, %s)" RESET ": <GOOD> New file parent dir exists, but new internal file does not\n",
            old_pathname, new_pathname);

    /// Creat new symbolic link file
    mysymfile(new_pmip, old_abspathname, new_base_name);

    // Increment old link count and write back
    old_mip->INODE.i_links_count += 1;
    old_mip->dirty = 1;
    iput(old_mip);

    // Write new parent back
    new_pmip->dirty = 1;
    iput(new_pmip);

    return EXIT_SUCCESS;
}

int mysymfile(MINODE *pip, char *old_name, char *name)// this is a helper function the algorithm is above
{
    char *cp;// traversing i nodes and dir.

    int ino, bno;
    char buf[BLKSIZE];

    // Allocate space for new inode under cwd
    ino = ialloc(running->cwd->dev);// allocated a new inode
    bno = balloc(running->cwd->dev);//allocated a block

    MINODE *mip = iget(running->cwd->dev, ino);// get the newly allocated inode

    ip = &mip->INODE;

    // FILE type mode
    ip->i_mode = 0120775; /// LNK mode
    ip->i_uid  = running->uid;	// Owner uid
    ip->i_gid  = running->gid;	// Group Id
    ip->i_size = 0;		// Size in bytes
    ip->i_links_count = 1;
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  // set to current time
    ip->i_blocks = 1;

    // File has no data blocks
    int i = 0;
    for(i=0; i<15; i++) // set the iblocks
    {
        ip->i_block[i]=0;
    }

    ip->i_block[0] = bno;

    /// Write string of old name into iblock
    // Add old name into block of new
    memset(buf, 0, BLKSIZE);
    strcpy(buf, old_name);
    put_block(mip->dev, bno, buf);

    // Name into parent
    enter_name(pip, ino, name);

    // Write back
    mip->dirty = 1;
    iput(mip);
}
