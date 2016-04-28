#include "level1.h"

int rm_file(char * pathname)
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

        //debug_print("Debugging write");

        //if the file is a FILE and isnt busy
        // continue
        if(S_ISREG(mip->INODE.i_mode) && isFDBusy(mip)==0)
        {
            // now that we passed the tests deallocate all of the direct i blocks
            for (i=0; i<12; i++)
            {

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
            //entry from the parent directory and DO NOT (BECAUSE ITS A FILE) decrement the parent link count by one.
            mino = getino(dev, parent);
            mip = iget(dev,mino);

            rm_child(mip,child);
            mip->dirty = 1;

            iput(mip);

            printf(YELLOW "\t...RM_FILE(%s)" RESET ": File has been removed\n", pathname);
            return 0;
        }
        else
        {
            printf("\nERROR: THE PATH NAME LEADS TO A FILE THAT IS BUSY || NOT A FILE\n");
            return 1;
        }
    }
    else
    {
        printf("\nERROR: FILE NOT SPECIFIED.\n");
    }
}
