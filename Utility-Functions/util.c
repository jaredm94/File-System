#include "../project.h"
#include "util.h"
/// DEBUGGING

// For debug statements
/*int debug_print(char *output)
{
    if (!debugging_mode) return EXIT_FAILURE;

    fputs(output, stderr);

    return EXIT_SUCCESS;
}*/

/// DIFFERENT RETURN TYPE

/*
Once you have the ino of an inode, you may load the inode into a slot
in the Minode[] array. To ensure uniqueness, you must search the Minode[]
array to see whether the needed INODE already exists:

If you find the needed INODE already in a Minode[] slot, just inc its
refCount by 1 and return the Minode[] pointer.

If you do not find it in memory, you must allocate a FREE Minode[i], load
the INODE from disk into that Minode[i].INODE, initialize the Minode[]'s
other fields and return its address as a MINODE pointer,
*/
MINODE *iget(int dev, int ino)
{
    int i = 0;
    char buf[BLKSIZE];
    int inode_start_block;

    // Read Group Descriptor
    get_block(dev, 2, buf);
    gp = (GD *) buf;

    // Get inode start block
    inode_start_block = gp->bg_inode_table;

    // Search for an already opened inode
    for (i = 0; i < NMINODES; i++)
    {
        if (minode[i].ino == ino && minode[i].ino > 0)
        {
            //printf("\n...IGET(%d, %d): <MEM> Found inode = %d at minode[%d]\n",
                //dev, ino, ino, i);

            minode[i].refCount++;

            return &minode[i];
        }
    }

    // Find the first avaliable spot to load a node
    for (i = 0; minode[i].refCount != 0; i++);

    // Get the block for this inode
    int inode_block_number = (ino - 1) / 8 + inode_start_block;
    int inode_remainder = (ino - 1) % 8;

    // Grab inode from those directions
    get_block(dev, inode_block_number, buf);

    // we move over the remainder number of inodes in the
    //current block to our destination inode
    ip = (INODE *) buf + inode_remainder;

    // now we load the inode into memory
    minode[i].INODE=*ip;
    minode[i].dev=dev;
    minode[i].ino=ino;
    minode[i].refCount=1;
    minode[i].dirty=0;
    minode[i].mountptr=NULL;

//    if (!getname(dev, ino, minode[i].name)) {
//        strcpy(minode[i].name, "[filename]");
//    }
    //printf("name = %s\n", minode[i].name);

//    get_block(dev, ip->i_block[0], buf);
//    dp = (DIR*) buf;
//    strncpy(minode[i].name, dp->name, dp->name_len);
//    minode[i].name[dp->name_len] = 0;

//    get_minode_name(running->cwd->dev, ino, minode[i].name);
//    strcpy(minode[i].name, "Fred");

    /// NOTE: Might have problem with root case! where ino = 2

    /// TODO: Add name to minode

    return &minode[i];
}

/* Prepends t into s. Assumes s has enough space allocated
** for the combined string.
*/
void prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    size_t i;

    memmove(s + len, s, strlen(s) + 1);

    for (i = 0; i < len; ++i)
    {
        s[i] = t[i];
    }
}

// Recursively get pathname of minode
// NOTE: this will only work for mip's that are directories
int getabsname(MINODE *mip, char name[])
{
    // Parent inode
    MINODE *pip;
    // Buffer for data block
    char buf[BLKSIZE];
    // Holds dir entry
    char temp[128];
    char *cp;
    int  childino, parentino;
    int dev;

    // Device
    dev = mip->dev;
    // Child inode number
    childino = mip->ino;

    /// ROOT CASE
    if (childino == 2)
    {
        if (strlen(name) == 0) prepend(name, "/");
        return EXIT_SUCCESS;
    }

    /// Get parent of mip
    get_block(dev, mip->INODE.i_block[0], buf);
    cp = buf;
    dp = (DIR *) buf;

    while (cp < (buf + BLKSIZE))
    {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;

        if (!strcmp(temp, "..")) parentino = dp->inode;

        cp += dp->rec_len;
        dp = (DIR *) cp;
    }
    pip = iget(dev, parentino);

    /// Get child's name by scanning parent entries
    get_block(dev, pip->INODE.i_block[0], buf);
    cp = buf;
    dp = (DIR *) buf;

    while (cp < (buf + BLKSIZE))
    {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;

        if ((strcmp(temp, ".")) && (dp->inode == childino))
        {
            // Gives name = "/temp..."
            prepend(name, temp);
            prepend(name, "/");
            break;
        }

        // Next entry
        cp += dp->rec_len;
        dp = (DIR *) cp;
    }
    // PRINT SNAPSHOT
//    printf("\tNAME = '%s'\n", name);
//    printf("\tCHILDINO = %d\n", childino);
//    printf("\tPARENTINO = %d\n", parentino);

    /// Recurse
    getabsname(pip, name);
}


/// FILE FUNCTIONS

// For Open
int do_have_permissions(MINODE *mip, int uid)
{
    // Root has all access
    if (uid == 0) return EXIT_SUCCESS;
    /// TODO Check if uid has permisions on inode

    return EXIT_FAILURE;
}

int is_reg_file(MINODE *mip)
{
    return S_ISREG(mip->INODE.i_mode);
}

int is_dir_file(MINODE *mip)
{
    return S_ISDIR(mip->INODE.i_mode);
}

/// INODE FUNCTIONS

int get_block(int fd, int blk, char buf[ ])// get the block
{
    lseek(fd, (long)blk*BLKSIZE, 0);
    read(fd, buf, BLKSIZE);
}
int put_block(int fd, int blk, char buf[ ])// replace the black whats in the with whats in the buffer
{
    lseek(fd, (long)blk*BLKSIZE, 0);
    write(fd, buf, BLKSIZE);
}

int tst_bit(char *buf, int bit)// test to see whether a bit is "on" == 1
{
    int i, j;
    i = bit/8;
    j=bit%8;
    if (buf[i] & (1 << j))
        return 1;
    return 0;
}

int set_bit(char *buf, int bit)// set a bit
{
    int i, j;
    i = bit/8;
    j=bit%8;
    buf[i] |= (1 << j);
}

int clr_bit(char *buf, int bit)// clear a bit
{
    int i, j;
    i = bit/8;
    j=bit%8;
    buf[i] &= ~(1 << j);
}

// Uses name[] to search from the root of fd (file descriptor) to find the inode from the given pathname and
// returns memory location of the of the inode.
int getino(int fd, char *name)
{
    char buf[BLKSIZE], buf2[BLKSIZE];
    char tokenized_String[64][128];
    char *cp;
    int  i, j;
    char temp[128];// to temp hold that directory/files name for comparison
    int name_len;// the length of the tokenized strings
    int inode_start_block;

    // Read Group Descriptor
    get_block(fd, 2, buf);
    gp = (GD *) buf;

    // Get inode start block
    inode_start_block = gp->bg_inode_table;

    // get inode start block
    get_block(fd, inode_start_block, buf);

    /// ROOT CASE
    if (!strcmp(name, "/")) return 2;

    if(name[0]=='/')// for absulote path name
    {
        // Get inode pointer to root
        ip = (INODE *) buf + 1;        //<======= explain why +1
    }
    else// from the current working directory
    {
        ip = &running->cwd->INODE;
    }

    name_len = tokenize(name, tokenized_String, "/");// tokenize the string and find out the length of the toked path

    // Loop through the name of each (tokenized) directory trying to find the next file/directory
    for (j = 0; j < name_len; j++)
    {
        // Print token we're looking for
        //printf("\n[%d/%d] Finding name '%s'...\n", j, name_len, tokenized_String[j]);

        /* ADD CHECK TO SEE IF ITS A DIRECTORY OR NOT BEFOR PROCEEDING  */ // <==================
        // Read INODE's first block
        get_block(fd, ip->i_block[0], buf);// go to the first block so we can find out what info we need file
        //type etc
        // Get block starters
        cp = buf;
        dp = (DIR *) buf;

        //printf("\t\t\tSearching through '%s' directory...\n", dp->name);

        // Loop through blocks of inode
        while (cp < (buf + BLKSIZE)) // cp is a char * if we go further then the byte size of a blk stop.
        {
            // Get directory name
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;

            //printf("\t\t\t\t...Inspecting entry '%s'", temp);
            //if (dp->file_type == 2) printf("\t(directory)");
            //else printf("\t(file)");
            //printf("\n");

            // Compare with current filename
            if (strcmp(temp, tokenized_String[j]) == 0)
            {
                // We've found the name, now we get its inode
                //printf("\t\t\t\t\t*** FOUND ***\n");

                // Get the block for this inode
                int inode_block_number = (dp->inode - 1) / 8 + inode_start_block;
                int inode_remainder = (dp->inode - 1) % 8;

//                // Print for debugging
//                printf("\tinode # = %d\n", (inode_block_number + inode_remainder - 2));
//                printf("\tip block # = %d\n", inode_block_number);
//                printf("\tip remainder # = %d\n", inode_remainder);

                get_block(fd, inode_block_number, buf2);

                // Grab inode from those directions
                ip = (INODE *) buf2 + inode_remainder;// we move over the remainder number of inodes in the
                //current block.
                if(j==name_len-1)// if we've reached the final file/directory return its inode number
                {
                    // Print end inode information
//                    printf("\t| tokenized_String[%d] = %s\n", j, tokenized_String[j]);
//                    printf("\t| inode # = %d\n", dp->inode);
//                    printf("\t| inode # = %d\n", (inode_block_number + inode_remainder - 2));
//                    printf("\t| ip block # = %d\n", inode_block_number);
//                    printf("\t| ip remainder # = %d\n", inode_remainder);
                    return dp->inode;
                }
                // Stop this search
                break;
            }

            //printf(". ");
            // Let's see the next directory entry!
            cp += dp->rec_len;
            dp = (DIR *) cp;
        }
    }

    //printf("** ERROR: GETINO '%s' COULD NOT BE FOUND **\n", name);

    return -1; /// NOTE: Changed spec from EXIT_FAILURE
    //Jared: return -1;

    /* // At this point, we know we've got our INODE
     // Now, we display the block info
     printf("\n[%d/%d] Displaying inode info for pathname = '%s'...\n", name_len, name_len, name);
     printf("\tmode=%4x\n", ip->i_mode);
     printf("\tuid=%d  gid=%d\n", ip->i_uid, ip->i_gid);
     printf("\tsize=%d\n", ip->i_size);
     printf("\ttime=%s\n", ctime(&ip->i_ctime));
     printf("\tlink=%d\n", ip->i_links_count);
     printf("\tflags=%d\n", ip->i_flags); */

}

/*
This function searches the data blocks of a DIR inode (inside an Minode[])
for name. You may assume a DIR has only 12 DIRECT data blocks. Return its inode number
*/
// Note: Will search for child name in given mip
int search(MINODE *mip, char *name)
{

    int i;
    char *cp;
    char buf[BLKSIZE];
    char temp [128];
//  int inode_start_block;


    // Read Group Descriptor
    //get_block(mip->dev, 2, buf);
    //gp = (GD *)buf;

    // Get inode start block
    //inode_start_block = gp->bg_inode_table;

    for(i=0; i < mip->INODE.i_blocks && i < 12; i++ )// look through every iblock
    {
        if (mip->INODE.i_block[i] == 0)
        {
            //printf("\t\t\tNothing's here (or after) at ip->i_block[%d]\n", i);
            return 0;
        }

        get_block(mip->dev, mip->INODE.i_block[i], buf);// go to the first block so we can find out what info we need file
        //type etc
        // Get block starters
        cp = buf;
        dp = (DIR *) buf;

        // Loop through blocks of inode
        while (cp < (buf + BLKSIZE))
        {
            strncpy(temp, dp->name, dp->name_len);// copy to add null nerminating char for comparison
            // Note: dp->name isn't null-terminated
            temp[dp->name_len] = 0;

            //printf("\t\t\t...Inspecting entry '%s'\n", temp);

            if(strcmp(temp,name)==0)//if theres a match return the value;
            {
                return dp->inode;
            }
            cp += dp->rec_len;// go to the next directory entry
            dp = (DIR *) cp;
        }

    }
    return 0;
}


/*
This function releases a Minode[] pointed by mip.
ALGORITHM
(1). dec refCount by 1.
(2). if (after dec) refCount > 0 ==> return;

 (3). if (Minode[].dirty == 0) ==> no need to write back, so return;

 (4). (refCount < 0 AND dirty==1) case ==> must write the INODE back to disk.

 (5). To write an INODE back to disk:

    Use minode's (dev, ino) to determine which INODE on disk,i.e. Use Mailman's
    algorithm to determine the disk block and inode's offset in that block.
    Read that block into a buf[ ], let INODE *ip point at the INODE in buf[ ].
    Copy mip->INODE into *ip in buf[ ];
    Write the block (in buf[ ]) back to disk.
    */
    /*
int iput(MINODE *mip)
{
    int inode_start_block;
    char buf[BLOCK_SIZE];

    // Read Group Descriptor
    get_block(mip->dev, 2, buf);
    gp = (GD *)buf;

    // Get inode start block
    inode_start_block = gp->bg_inode_table;

    //1.
    mip->refCount--;

    //2.
    if(mip->refCount > 0)
    {
        return 0;
    }

    //3.
    if(mip->dirty == 0)
    {
        return 0;
    }

    //4. and 5. (refCount < 0 AND dirty==1)
    // Get the block for this inode
    int inode_block_number = (mip->ino - 1) / 8 + inode_start_block;
    int inode_remainder = (mip->ino - 1) % 8;

    // Grab block from those directions
    get_block(mip->dev, inode_block_number, buf);

    // We move over the remainder number of inodes in the
    // current block
    ip = (INODE *) buf + inode_remainder;
    *ip = mip->INODE;


}
*/
int iput(MINODE *mip)
{
    int inode_start_block;
    char buf[BLOCK_SIZE];

    // Read Group Descriptor
    get_block(mip->dev, 2, buf);
    gp = (GD *)buf;

    // Get inode start block
    inode_start_block = gp->bg_inode_table;

    //1.
    mip->refCount--;

    //2.
    if(mip->refCount > 0)
    {
        return 0;
    }

    //3.
    if(mip->dirty == 0)
    {
        return 0;
    }

    //4. and 5. (refCount < 0 AND dirty==1)
    // Get the block for this inode
    int inode_block_number = (mip->ino - 1) / 8 + inode_start_block;
    int inode_remainder = (mip->ino - 1) % 8;

    // Grab block from those directions
    get_block(mip->dev, inode_block_number, buf);

    // We move over the remainder number of inodes in the
    // current block
    ip = (INODE *) buf + inode_remainder;
    *ip = mip->INODE;
    put_block(mip->dev, inode_block_number, buf);

}

/*
   Given the parent DIR (MINODE pointer) and myino, this function finds
   the name string of myino in the parent's data block. This is the SAME
   as SEARCH() by myino, then copy its name string into myname[ ].
*/
int findmyname(MINODE *mip, int myino, char myname[])
{

    int i;
    char *cp;
    char buf[BLKSIZE];


    for(i=0; i<mip->INODE.i_blocks && i<12; i++ )// look through every iblock

    {
        if (ip->i_block[i] == 0)
        {
            return 0;
        }

        get_block(mip->dev, mip->INODE.i_block[i], buf);// go to the first block so we can find out what info we need file
        //type etc
        // Get block starters
        cp = buf;
        dp = (DIR *) buf;

        // Loop through blocks of inode
        while (cp < (buf + BLKSIZE))
        {

            if(dp->inode == myino)//if theres a match return the value;
            {
                strncpy(myname,dp->name,dp->name_len);
                return dp->inode;
            }
            cp += dp->rec_len;// go to the next directory entry
            dp = (DIR *) cp;
        }

    }

    return 0;
}


/*
 For a DIR Minode, extract the inode numbers of . and ..
  Read in 0th data block. The inumbers are in the first two dir entries.
  */
int findino(MINODE *mip, int *myino,int *parentino)
{
    int inode_start_block;
    char buf[BLKSIZE];
    char cp;

    // Read Group Descriptor
    get_block(mip->dev, 2, buf);
    gp = (GD *)buf;

    // Get inode start block
    inode_start_block = gp->bg_inode_table;

    // Get the block for this inode
    int inode_block_number = (mip->ino - 1) / 8 + inode_start_block;

    // Grab the block
    get_block(mip->dev, inode_block_number, buf);

    //now we move t the first and second dir entris " . and .."
    cp=buf;//hold on so we can add the number of bytes of the record to it in order to find the next recod
    dp=buf;
    *myino = dp->inode;
    cp += dp->rec_len;// go to the next directory entry
    dp = (DIR *)cp;
    *parentino = dp->inode;

    return 0;
}


/********* END OF INODE FUNCTIONS *********/

/********* OTHER *********/



int tokenize(char path[],char returnval[][128],char * tok)// takes a path and a token and tokenizes the string based on the token.
{
    char *s = 0;
    char pathcpy[128];
    int i = 0;

    // Tokenize
    strncpy(pathcpy, path, 128);
    s = strtok(pathcpy, tok);
    strcpy(returnval[0], s);//copy the first tokenize

    for (i = 1; (s = strtok(0, tok)); i++)//copy the rest of the tokenized strings
    {
        strcpy(returnval[i], s);
    }

    // Insert our custom terminating string
    strcpy(returnval[i], "$");

    return i;
}

int print_dir_entries(MINODE *pip)
{
    int i, j;
    char *cp;
    char sbuf[BLKSIZE], temp[256], buf2[BLKSIZE], buf[BLKSIZE],
        *t1 = "xwrxwrxwr-------",
        *t2 = "----------------";
    MINODE *mip;
    INODE * ix;

    ix = &(pip->INODE);

    for (i=0; i<12; i++)   // ASSUME DIRs only has 12 direct blocks
    {
        if (ix->i_block[i] == 0)
            return 0;

        get_block(pip->dev,ix->i_block[i], sbuf);
        dp = (DIR *)sbuf;
        cp = sbuf;

        printf(CYAN);

        while (cp < sbuf + BLKSIZE)
        {
            // Get name
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;

            // Dir entry
            mip = iget(pip->dev, dp->inode);

            // Entry type
            printf("\t\t|%4d  ", mip->ino);
            if(is_dir_file(mip)) printf("d");
            else if(is_reg_file(mip)) printf("-");
            else printf("l");
            // Permissions
            for (j = 8; j >= 0; j--)
            {
                if (mip->INODE.i_mode & (1 << j)) printf("%c", t1[j]);
                else printf("%c", t2[j]);
            }
            // Ref count
            printf(" %3d ", mip->INODE.i_links_count);
            // User ID
            printf(" %3d ", mip->INODE.i_uid);
            // Grpup ID
            printf(" %3d ", mip->INODE.i_gid);
            // Size
            printf(" %9u ", mip->INODE.i_size);
            // Time
            printf(" %9d ", mip->INODE.i_atime);
            // Name
            printf(" %15s ", temp);

            // If symbolic, print it's link location
            if(!is_dir_file(mip) && !is_reg_file(mip))
            {
                get_block(running->cwd->dev, mip->INODE.i_block[0], buf);
                printf(" -> %s ", buf);
            }
            printf("\n");

            // Grab next inode
          /*  int inode_block_number = (dp->inode - 1) / 8 + inode_start_block;
            int inode_remainder = (dp->inode - 1) % 8;
            get_block(mip->dev, inode_block_number, buf2);

            ip = (INODE *) buf2 + inode_remainder;*/

            // Move to next entry
            cp += dp->rec_len;
            dp = (DIR *)cp;
            iput(mip);

        }

        printf(RESET);

    }
    return 0;
}
/********* END OF OTHER *********/

