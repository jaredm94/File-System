#include "level1.h"

/*
1. pip points at the parent minode[] of "/a/b", name is a string "c")
2. allocate an inode and a disk block for the new directory;
        ino = ialloc(dev);
        bno = balloc(dev);
3. mip = iget(dev, ino) to load the inode into a minode[] (in order to
   write contents to the INODE in memory).
4. Write contents to mip->INODE to make it as a DIR.
5. iput(mip); which should write the new INODE out to disk.
*/


int mymkdir(MINODE *pip, char *name)// this is a helper function the algorithm is above
{
    char *cp;// traversing i nodes and dir.

    int ino,bno;
    char buf[BLKSIZE];

    // Allocate space for new inode under cwd
    ino = ialloc(running->cwd->dev);// allocated a new inode
    bno = balloc(running->cwd->dev);//allocated a block

    MINODE *mip = iget(running->cwd->dev,ino);// get the newly allocated inode

    ip = &mip->INODE;
// set all of the directory descriptors
    ip->i_mode = 0x41ED;		// OR 040755: DIR type and permissions
    ip->i_uid  = running->uid;	// Owner uid
    ip->i_gid  = running->gid;	// Group Id
    ip->i_size = BLKSIZE;		// Size in bytes
    ip->i_links_count = 2;	        // Links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  // set to current time
    ip->i_blocks = 2;                	// LINUX: Blocks count in 512-byte chunks

    ip->i_block[0] = bno;             // new DIR has one data block
    int i = 1;

    for(i=1; i<15; i++) // set the iblocks
    {
        ip->i_block[i]=0;
    }

    mip->dirty = 1;               // mark minode dirty
    iput(mip);                    // write INODE to disk

    /// Write ". and .." to the directory entries

    get_block(pip->dev,bno,buf);
    // Memory is precleared  when we allocated it. Also cleared when we deallocate.

    // .
    dp=(DIR *)buf;
    dp->inode=ino;
    strncpy(dp->name, ".", 1);
    dp->name_len = 1;
    dp->rec_len = 12;//<================================ IDEAL_LEN = 4*[ (8 + name_len + 3)/4 ].
    dp->file_type =2;

    // Go to the next dir entry spot
    cp = buf + dp->rec_len;
    dp = (DIR *)cp;

    // ..
    dp->inode = pip->ino;
    dp->name_len = 2;
    strncpy(dp->name, "..", 2);
    dp->rec_len = BLOCK_SIZE - 12; // because this is the last directory entry in the block.
    dp->file_type =2;

    //write block to disk:
    put_block(pip->dev, bno, buf);

    enter_name(pip, ino, name);// put a refrence to this directory into the parent.
}

//This function puts the directory entry into the parent directory
int enter_name(MINODE *pip, int myino, char *myname)
{

    int ideal_len = 8 + strlen(myname) + 3;// the ideal length of the rec len we need to insert
    char buf[BLKSIZE];
    int i =0;
    char * cp;

    for (i = 0; i < 12; i++)
    {
        if (pip->INODE.i_block[i]==0) break;// if the block is empty we found it

        get_block(pip->dev, pip->INODE.i_block[i], buf);  // get the block to see if theres space to place a new dir rec in the blk

        cp = buf;// so we can go through the dir entries + rec_len
        dp = (DIR*) buf;

        // step to LAST entry in block: int blk = parent->INODE.i_block[i];
        while (cp + dp->rec_len < buf + BLKSIZE)
        {
            // print DIR entries to see what they are
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        // dp NOW points at last entry in block

        /* we subtract the actual length by the ideal. It should be greater then zero since the last rec_len size is
           BLKSIZE-(summation of the rec_len's in the block). The difference is whats left if theres enough
           we put our new refrence into this block. otherwise we use a new block. */
        int ideal_last_block_len = 8 + strlen(dp->name) + 3;
        int remain = dp->rec_len - ideal_last_block_len;

        // now we figure out if there's room left in this block
        if (remain >= ideal_len )
        {

            dp->rec_len = ideal_last_block_len;
            cp += dp->rec_len;
            dp = (DIR *) cp;
            dp->inode=myino;
            dp->name_len = strlen(myname);
            strncpy(dp->name,myname, dp->name_len);
            dp->rec_len = remain; //<================================ IDEAL_LEN = 8 + name_len + 3
            dp->file_type =2;
            //printf("\n\n\n\n %s \n\n\n",dp->name);

            //put the block back
            put_block(pip->dev, pip->INODE.i_block[i], buf);

            return 0;
        }

    } // goes through i_blocks until it finds an open spot.If a new i_block is needed its made below.

    if(i<12)
    {
        //load in a new block
        pip->INODE.i_block[i] = balloc(pip->dev);
        memset(buf,0,BLKSIZE);//reset the memory so we write a clean buffer to the block spot on disk
        dp = (DIR *)buf;

        dp->inode=myino;
        dp->name_len = strlen(myname);
        strncpy(dp->name, myname, dp->name_len);
        dp->rec_len = BLKSIZE;  //<================================ IDEAL_LEN = 8 + name_len + 3
        dp->file_type =2;


        pip->INODE.i_size +=1024;//increase parents because we allocated a newblock

        //put the block back;
        put_block(pip->dev, pip->INODE.i_block[i], buf);

        return 0;
    }
    else
    {
        //printf("/n/n/ I_BLOCKS FULL/n/n");
    }

}






/* 1. pahtname = "/a/b/c" start mip = root;         dev = root->dev;
            =  "a/b/c" start mip = running->cwd; dev = running->cwd->dev;
2. Let
     parent = dirname(pathname);   parent= "/a/b" OR "a/b"
     child  = basename(pathname);  child = "c"
3. Get the In_MEMORY minode of parent:
         pino  = getino(&dev, parent);
         pip   = iget(dev, pino);
   Verify : (1). parent INODE is a DIR (HOW?)   AND
            (2). child does NOT exists in the parent directory (HOW?);
4. call mymkdir(pip, child);
5. inc parent inodes's link count by 1;
   touch its atime and mark it DIRTY
6. iput(pip);
*/
int make_dirCmd()
{
    char *path = pathM;
    make_dir(path);
}

int make_dir(char *path )
{

    char * parent, * child;
    char temp[2][strlen(path)+1];
    char buf[BLKSIZE];
    int dev;

    strcpy(temp[0],path);
    strcpy(temp[1],path);

    parent = dirname(temp[0]);
    child = basename(temp[1]);

    if(path[0] == '/')
    {
        dev = root->dev;
    }
    else
    {
        dev = running->cwd->dev;
    }

    // Get parent inode
    int pino = getino(dev, parent);

    if(pino == -1)
    {
        printf("\nERROR: PARENT DIR(%s) DOESNT EXIST\n",parent);
        return -1;
    }

    MINODE *pip = iget(dev, pino);

    // Find info about what kind of file it is
    //get_block(dev,pip->INODE.i_block[0],buf);// get the directory entries
    //dp=(DIR *) buf;

    //if(dp->file_type == 2)
    //{
    if(pip->INODE.i_mode = DIR_MODE)
    {
        if(search(pip,child) == 0)
        {

            mymkdir(pip, child);

            pip->INODE.i_links_count++;// increase link count because child directory points to parent.
            //add a touch
            touch_file(path);


            pip->dirty=1;

            iput(pip);

            printf(YELLOW "\t...MAKE_DIR(%s)" RESET ": New directory has been added to parent block\n", path);

            return 0;

            //if the child alreadye exists print errorr
        }
        else
        {
            printf("\nERROR: CHILD ALREADY EXIST IN PARENT DIRECTORY \n");
            return 1;
        }
    }
    else
    {
        printf("\nERROR: TRIED TO MAKE A DIRECTORY IN SOMETHING OTHER THEN A DIRECTORY \n");
        return 1;
    }




}
