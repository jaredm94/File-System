#include "level1.h"
int rmdirCmd()
{
char *pathname = pathM;
rmdir(pathname);

}

int rmdir(char *pathname)
{


    char * parent, * child;// of the path name
    char temp[2][strlen(pathname)+1];// to hold two copies of pathname
    char buf[BLKSIZE];
    int dev;
    int i =0;

    if(pathname != NULL)
    {
        strcpy(temp[0],pathname);
        strcpy(temp[1],pathname);

        parent = dirname(temp[0]);
        child = basename(temp[1]);

        if(pathname[0] == '/')
        {
            dev = root->dev;
        }
        else
        {
            dev = running->cwd->dev;
        }

        int mino = getino(dev, pathname); //get the parent ino
        MINODE * mip  = iget(dev, mino); // load the inode into mem

        //if the file is a directory and empty and not busy
        // continue
        if(S_ISDIR(mip->INODE.i_mode) && isDirEmpty(mip)==1 && isFDBusy(mip)==0)
        {
            // now that we passed the tests deallocate all of the direct i blocks
            for (i=0; i<12; i++)
            {
                //printf("\n\n %s \n\n",child);
                if (mip->INODE.i_block[i]==0)
                    continue;
                bdelloc(mip->dev, mip->INODE.i_block[i]);
                mip->INODE.i_block[i] =0;
            }

            //delloc the inode
            idelloc(mip->dev, mip->ino);
            //need to touch

            iput(mip);// (which clears mip->refCount = 0);

            //now we get the parents direcotry and remove the childs
            //entry from the parent directory and decrement the parent link count by one.
            mino = getino(dev, parent);
            mip = iget(dev,mino);
            //printf("\n\n %s \n\n",child);
            rm_child(mip,child);
            mip->dirty = 1;
            mip->INODE.i_links_count--;
            iput(mip);
            //printf("\nREMOVED: %s.",pathname);

            printf(YELLOW "\t...RMDIR(%s)" RESET ": Directory has been removed\n", pathname);
            return 0;
        }
        else
        {
            printf("\nERROR: THE PATH NAME LEADS TO A FILE THAT IS BUSY || NOT A DIRECTORY || IS NOT EMPTY.\n");
            return 1;
        }
    }
    else
    {
//        printf("\nERROR: FILE NOT SPECIFIED.\n");
    }
}

//We find if the file is open in the open file table and
//therefore considered busy
int isFDBusy(MINODE * file)
{
    int i = 0;

    //search through all of the table entries.
    while (i < NFD) /// CHANGED from NOFT
    {

        // if the file is open and something is refrencing return true
        if(running->fd[i]->inodeptr==file && running->fd[i]->refCount>0)
        {
//            printf("\nERROR: THE FILE IS BUSY.\n");
            return 1;
        }
        // might be able to make this an elsee statement
        i++;
    }

    return 0;
}

//check to see if the directory is empty. Check link count > 2 not empty
// If link count ==2 go through the data blocks to check for files.
int isDirEmpty(MINODE * dir)
{
    if(dir->INODE.i_links_count>2)// if its contains any other links (other then . and ..)
    {
//        printf("\nERROR: THE DIRECTORY IS NOT EMPTY.\n");
        return 0;
    }
    else if(dir->INODE.i_links_count == 2)//check for files
    {
        int i = 0;
        char buf[BLKSIZE];
        char * cp;

        for(i=0; i < 12; i++)// for the non indirect blocks
        {
            if(dir->INODE.i_block[i] != 0)
            {
                get_block(dir->dev,dir->INODE.i_block[i],buf);

                dp = (DIR *) buf;
                cp = dp;

                while(cp < buf + BLKSIZE)
                {
                    // if its contains files with names other then . or .. we know its not empty.
                    if(strncmp(dp->name,".",1)==1 && strncmp(dp->name,"..",2)==1)
                    {
//                        printf("\nERROR: THE DIRECTORY IS NOT EMPTY.\n");
                        return 0;
                    }

                    //increment through all of the records in the current block
                    cp+=dp->rec_len;
                    dp = (DIR *) cp;

                }

            }
        }
    }
    //return true if all of the tests are passed
    return 1;
}

//given a directory loaded in memory, delete the entry name from its i blocks
int rm_child(MINODE * parent, char *name)
{
    int i =0;
    int j =0;
    char * cp = NULL;
    char buf[BLKSIZE];
    DIR * previous_entry = NULL;

    //search pareent inodes data blocks for the name.
    //direct iblocks
    for(i = 0; i < 12; i++)

    {
        if(parent->INODE.i_block[i]!=0)// if the i block isnt empty
        {
            get_block(parent->dev,parent->INODE.i_block[i],buf);
            cp = buf;
            dp = (DIR *) cp;

            // if the first entry is the first in the data block
            //slide the non empty blocks down.
            if(strncmp(dp->name,name,dp->rec_len)==0)
            {
                //dealloc the the now empty block
                bdelloc(parent->dev, parent->INODE.i_block[i]);
                parent->INODE.i_size -= BLKSIZE;

                //slide the blocks over for direct blocks
                for(j=i; j< 11; j++)
                {
                    parent->INODE.i_block[j] = parent->INODE.i_block[j+1];
                }
                // make sure the final block is set to zero
                parent->INODE.i_block[j] = 0;

                return 0;
            }

            previous_entry = dp;
            cp+= dp->rec_len;
            dp = (DIR *) cp;

            //while we are still in the blk find the name
            while(cp < buf + BLKSIZE)
            {
                //if we found the file delete it in the parents records
                if(strncmp(name,dp->name,strlen(name))==0)
                {
                  //if we find the file on the last block
                    if(cp + dp->rec_len >= buf + BLKSIZE)
                    {
                        // we've now deleted the directory entry
                        previous_entry->rec_len += dp->rec_len;
                        put_block(parent->dev,parent->INODE.i_block[i],buf);
                        return 0;
                    }

                    // this happens when it wasnt the last block deleted.
                    //shift all of the other records over to the left
                    // to fill in the gap left by our deleted entry.
                    // we increment our pointers because we want to fill
                    //in previous_entry with DP's info. rec len is the summation of the area we need to shift over
                    //using memmove
                    int rec_len = dp->rec_len;
                    previous_entry = dp;
                    cp+= dp->rec_len;
                    dp = (DIR *) cp;

                    while(cp + dp->rec_len < buf + BLKSIZE)
                    {
                        //this is where we slide records over
                        // *previous_entry = *dp;
                        memmove(previous_entry,dp,dp->rec_len);
                        //printf("\n %s: %d",dp->name,dp->rec_len);
//                        printf( "\n\n Previous: %d %d %s %d %d",previous_entry->file_type,previous_entry->inode,previous_entry->name,
//                                previous_entry->name_len,previous_entry->rec_len);
                        previous_entry = (DIR *) (((char *) previous_entry) + previous_entry->rec_len);// we move over to the next spot to shift over records
                        cp+= dp->rec_len;// get the next record to shift
                        dp = (DIR *) cp;

                    }

                    //set the final record in the blocks size to
                    // its rec_len + the delete files ideal length 4+namlen+3
                    memmove(previous_entry,dp, 8+dp->name_len+3);
                    previous_entry->rec_len += rec_len;

//                    printf( "\n\n Previous: %d %d %s %d %d",previous_entry->file_type,previous_entry->inode,previous_entry->name,
//                            previous_entry->name_len,previous_entry->rec_len);
                    /*dp->rec_len += previous_entry->rec_len;
                    rec_len += dp->rec_len;
                    memmove(previous_entry,copyfrom,rec_len);*/

                    put_block(parent->dev,parent->INODE.i_block[i],buf);
                    return 0;
                }

                previous_entry = dp;
                cp+= dp->rec_len;
                dp = (DIR *) cp;
            }
        }
    }

    return 1;
}
