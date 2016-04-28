#include "level2.h"

int writeCmd()
{
    /****PARAMETERS****/
    int fd = atoi(pathM);
    char * passThis = parameterM;
    /****PARAMETERS****/

    int i =0;
    int count =0;
    int c = 0;

    while(count != strlen(passThis))
    {

        i=mywrite(fd,passThis,strlen(passThis));
        count += i;
    }

    printf(BOLDYELLOW"\n\tWRITE CALLED: %d bytes wrote.\n" RESET,count);
    return count;
}

int mywrite(int fd, char buf[], int nbytes)
{
    char * child;
    int i = 0;
    int start = 0;//start byte within the block we are writing too
    int blk =0; //the blk number we are writing too
    int lbk = 0;
    __le32 i_buf[BLKSIZE / 4]; //indirect buffer made to  store iblock that contain i blocks
    __le32 di_buf[BLKSIZE / 4]; //double indirect buffer
    char wbuf[BLKSIZE];
    char * cp;
    int count = 0;// number of bytes we wrote
    OFT * oftp = running->fd[fd];
    MINODE * mip = oftp->inodeptr;

    //no we check to see if we can even write to the file
    if(oftp->mode == 0)// only in read mode = 0
    {
        printf("\nERROR: FILE IS IN READ MODE CANNOT WRITE\n");
        return -1;
    }

    //if we find run out of space first or if we read the amount of bytes we need
    while (nbytes>0)
    {

        start =  oftp->offset % BLKSIZE;
        lbk =  oftp->offset / BLKSIZE;

        //these if statements find the blk number directly or indirectly that we need to find
        // Direct blocks
        if (lbk < 12)
        {
            if (mip->INODE.i_block[lbk] == 0)    // if no data block yet
            {

                mip->INODE.i_block[lbk] = balloc(mip->dev);// MUST ALLOCATE a block

                // write a block of 0's to blk on disk: OPTIONAL for data block
                //                                      but MUST for I or D blocks
                //everything is set to zero in allocation <==================================
            }

            blk = mip->INODE.i_block[lbk];      // blk should be a disk block now
        }
        // indirect blocks
        else if (lbk >= 12 && lbk < 256 + 12)
        {
            // if the block is empty allocate a new block
            if(mip->INODE.i_block[12]==0)
            {
                mip->INODE.i_block[12] = balloc(running->cwd->dev);
            }

            get_block(mip->dev, mip->INODE.i_block[12], (char *) i_buf);

            if(i_buf[lbk-12] == 0)
            {
                i_buf[lbk-12] = balloc(running->cwd->dev);

                put_block(mip->dev, mip->INODE.i_block[12], (char *) i_buf);

            }
            //get the blk id
            blk = i_buf[lbk-12];

        }
        // double indirect blocks
        else
        {


            if(mip->INODE.i_block[13]==0)// if the double indirect block is empty
            {
                mip->INODE.i_block[13] = balloc(running->cwd->dev);

            }
            //get the double indirect block
            get_block(mip->dev, mip->INODE.i_block[13], (char *) i_buf);

            // we get the block number into read block that we want.
            // (TOTAL BLOCKS - 12) we know it takes atleast 256+13 (total only the index is 12) blocks to get in here.
            //we divide by 256 because each Index 1-10 for ex represents 256 blks
            // we have -1 because the minimum we can get from the division is 1 not 0.

            int indirect_index = ((lbk - 12) / 256) - 1;
            int direct_index = (lbk-12) % 256;

            if(indirect_index == 256)// this clearly means we are out of space
            {
                printf("\nERROR: OUT OF SPACE IN THE FILE\n");
                return -1;
            }

            //if not already allocated allocate
            if(i_buf[indirect_index] == 0)
            {
                i_buf[indirect_index] = balloc(running->cwd->dev);
                put_block(mip->dev, mip->INODE.i_block[13], (char *) i_buf);
            }

            get_block(mip->dev, i_buf[indirect_index], (char *) di_buf);

            //if the indirect block is empty allocate one
            if(di_buf[direct_index] == 0)
            {
                di_buf[direct_index] = balloc(running->cwd->dev);
                put_block(mip->dev, i_buf[indirect_index], (char *)di_buf);
            }

            blk = di_buf[direct_index];


        }

        //
        get_block(mip->dev, blk, wbuf);

        /* copy from startByte to buf[ ], at most remain bytes in this block */
        cp = wbuf + start;

        int writeBytes = nbytes;// try to read as many bytes as possible

        int remain = BLOCK_SIZE - start; // number of bytes remain in readbuf[]


        if(writeBytes > remain)// we check to see if the current amount we are going to write is greater then what
            // what we have left in the blcok
        {
            writeBytes = remain;// if its more then we have left write how much we have left
        }


        memcpy(cp,buf,writeBytes);
        //now we increment our area to copy to aka buf
        buf += writeBytes;
        oftp->offset += writeBytes;
        nbytes -= writeBytes;
        count += writeBytes;
        oftp->inodeptr->INODE.i_size += writeBytes;
        put_block(mip->dev, blk, wbuf);


        // loop back to while to write more .... until nbytes are written */
    }

    printf(BOLDYELLOW"\n\n\tWRITE CALLED: %d bytes wrote.\n\n" RESET,count);
    mip->dirty = 1;
    return count;
}
