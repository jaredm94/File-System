#include "level2.h"
int openCmd()
{
/****PARAMETERS****/
char * pathname = pathM;
int fd = atoi(parameterM);
/****PARAMETERS****/

printf(BOLDMAGENTA "\nOpened %s with fd = %d.\n" RESET, pathname, open_file(pathname,fd));
}


int open_file(char * pathname, int user_mode)
{
    int dev;
    char temp[128];

    if (pathname[0]=='/')
    {
        dev = root->dev;
    }          // root INODE's dev
    else
    {
        dev = running->cwd->dev;
//        strcpy(temp,running->cwd->name);// account for no / not absolute
//
//        if(strcmp(running->cwd->name,"/")==0)
//        {
//        strcat(temp,"/");
//        }
//
//        strcat(temp,pathname);

    }


    // now we get the file loaded into memory
    int mino = getino(dev, pathname); //get the parent ino

    //check its existance.
    if(mino == -1)
    {
        printf("\nERROR: FILE DOESNT EXIST.\n");
        return -1;
    }

    MINODE * mip  = iget(dev, mino); // load the inode into mem

    if(!S_ISREG(mip->INODE.i_mode))
    {
        printf("\nERROR: TRIED TO OPEN SOMETHING OTHER THEN A FILE.\n");
        iput(mip);
        return -1;
    }
    else if(isBusy(mip)==1)
    {
        printf("\nERROR: FILE IS BUSY CAN'T OPEN.\n");
        iput(mip);
        return -1;
    }
    //now we check to see if the file is being altered else where
    int offset = allocateOFT(mip,user_mode);
    /**************** CHECK TO SEEE IF THE FILE IS OPEN ****************/
    if(offset==-1)
    {
        iput(mip);
        return -1;
    }

    // Update directory access time
    touch_file(pathname);

return offset;
    //look for an open spot in the table

}

int isBusy(MINODE * m)
{
    int i = 0;

    //search through all of the table entries.
    while (i < NFD) /// CHANGED from NOFT
    {

        // if the file is open and something is refrencing return true
        if(running->fd[i]->inodeptr== m && running->fd[i]->refCount>0 && running->fd[i]->mode !=0)
        {
//            printf("\nERROR: THE FILE IS BUSY.\n");
            return 1;
        }
        // might be able to make this an elsee statement
        i++;
    }

    return 0;
}

int allocateOFT(MINODE * mip, int user_mode)
{
    int i = 0;

    //search through all of the table entries.
    while (i < NFD) /// CHANGED from NOFT
    {

           if(running->fd[i]->refCount<1)
        {

            running->fd[i]->mode= user_mode;
            running->fd[i]->refCount =1;
            running->fd[i]->inodeptr = mip;

            switch(user_mode)
            {
            case 0 :
                running->fd[i]->offset = 0;     // R: offset = 0
                mip->INODE.i_atime = time(0L);
                break;
            case 1 :
                truncate(mip);        // W: truncate file to 0 size
                running->fd[i]->offset = 0;
                mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
                break;
            case 2 :
                running->fd[i]->offset = 0;     // RW: do NOT truncate file
                mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
                break;
            case 3 :
                running->fd[i]->offset =  mip->INODE.i_size;  // APPEND mode
                mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
                break;
            default:
                printf("invalid mode\n");
                return(-1);
            }

            mip->dirty = 1;
            return i;
        }
        // might be able to make this an elsee statement
        i++;
    }

    return -1;
}

void truncate(MINODE * mip)
{
    int i = 0;
    int j = 0;

    __le32 indirect_buf[BLKSIZE/4];// le32 is the type of I block
    // and also is of size 4 bytes. BLKSIZE/4 = 256 which is the number of
    // direct  blocks that come from single indirection

    __le32 double_buf[BLKSIZE/4];// for double indirect
    // we are only accounting for direct blocks
    for(i=0; i< 12; i++)
    {
        // if the block isnt empty deallocte it
        if(mip->INODE.i_block[i]!= 0)
        {
            bdelloc(mip->dev,mip->INODE.i_block[i]);
            mip->INODE.i_block[i] = 0;
        }
    }

    // now we deal with the single indirection
    if(mip->INODE.i_block[12]!=0)// if the block isnt empty
    {
        get_block(mip->dev,mip->INODE.i_block[12],(char *) indirect_buf);

        for(i=0; i <256; i++)
        {
            bdelloc(mip->dev,indirect_buf[i]);
        }
        bdelloc(mip->dev,mip->INODE.i_block[12]);
        mip->INODE.i_block[12]=0;// erased the block
    }

    //now we deal with double indirection

    if(mip->INODE.i_block[13]!= 0)
    {
        get_block(mip->dev, mip->INODE.i_block[13], (char *)indirect_buf);

        for (i = 0; i < 256; i++)
        {
            // if the now single indirect block are empty skip
            if (indirect_buf[i] != 0)
            {
                //get the indirect block
                get_block(mip->dev, indirect_buf[i], (char *) double_buf);

                for (j = 0; j < 256; j++)
                {
                    //deallocate the double indirect blocks (contians data)
                    bdelloc(mip->dev, double_buf[j]);
                }
                //erase the single direct block now that its contents are empty
                bdelloc(mip->dev, indirect_buf[i]);
            }
        }

        mip->INODE.i_block[13] = 0;
    }

    mip->INODE.i_size = 0;
}
