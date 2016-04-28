#include "level1.h"

// Hard link old file to new file
// NOTE: Links cannot be across different devices (would have to check for level 3)
// SOURCE: http://www.eecs.wsu.edu/~cs360/link.html
int linkCmd()
{
    char *old_pathname = pathM;
    char *new_pathname = parameterM;
    link(old_pathname,new_pathname);
}


int link(char *old_pathname, char *new_pathname)
{

    int dev, old_ino, new_ino, new_pino;
    MINODE *old_mip, *new_mip, *new_pmip;
    char new_pathname_copy_1[256], new_pathname_copy_2[256], *new_dir_name, *new_base_name;

    // Get running device
    if (old_pathname[0] == '/') dev = root->dev;
    else dev = running->cwd->dev;

    // Old pathname inode
    old_ino = getino(dev, old_pathname);
    old_mip = iget(dev, old_ino);

    // Check if old file is not dir
    if (is_dir_file(old_mip)) return EXIT_FAILURE;

    // Splitup new pathname
    strcpy(new_pathname_copy_1, new_pathname);
    strcpy(new_pathname_copy_2, new_pathname);

    new_dir_name = dirname(new_pathname_copy_1);
    new_base_name = basename(new_pathname_copy_2);


    // Does new parent exist and is a directory?
    if(strcmp(new_dir_name,".")==0)// if the path is the current directory
    {
        new_pino = running->cwd->ino;
        new_pmip = iget(dev,running->cwd->ino);
    }
    else
    {
        new_pino = getino(dev, new_dir_name);
        if (new_pino < 0) return EXIT_FAILURE;
        new_pmip = iget(dev, new_pino);
        if (!S_ISDIR(new_pmip->INODE.i_mode)) return EXIT_FAILURE;
    }

    // Does new file already exist?
    new_ino = getino(dev, new_pathname);
    if (new_ino >= 0) return EXIT_FAILURE;

    printf(YELLOW "\t...LINK(%s, %s)" RESET ": <GOOD> New file parent dir '%s' exists, but internal (new) file '%s' does not\n",
           old_pathname, new_pathname, new_dir_name, new_base_name);

    // Create new entry (file) in dir (= new_pino) with inode number (= old_ino)
    mylinkfile(new_pmip, old_ino, new_base_name);


    // Incrememnt i_links_count of inode (= old_mip) by 1 and write back
    old_mip->INODE.i_links_count += 1;
    old_mip->dirty = 1;
    iput(old_mip);

    // Close new parent
    new_pmip->dirty = 1;
    iput(new_pmip);

    return 1;
}

int mylinkfile(MINODE *pip, int old_ino, char *name)// this is a helper function the algorithm is above
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
    ip->i_mode = FILE_MODE;
    ip->i_uid  = running->uid;	// Owner uid
    ip->i_gid  = running->gid;	// Group Id
    ip->i_size = 0;		// Size in bytes
    ip->i_links_count = 1;
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  // set to current time
    ip->i_blocks = 1;                	// LINUX: Blocks count in 512-byte chunks

    // File has no data blocks
    int i = 0;
    for(i=0; i<15; i++) // set the iblocks
    {
        ip->i_block[i]=0;
    }

    ip->i_block[0] = bno;

    mip->dirty = 1;               // mark minode dirty
    iput(mip);                    // write INODE to disk

    enter_name(pip, old_ino, name);
}
