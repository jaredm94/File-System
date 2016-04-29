#include "level2.h"

int readCmd()
{
    /****PARAMETERS****/
    int fd = atoi(pathM);
    int bytes = atoi(parameterM);
    /****PARAMETERS****/

    char * buf = malloc(bytes);
    int i =0;
    int count =0;

    while(i=myread(fd,buf,bytes))
    {
         if(bytes<=0) break;
        count += i;
        bytes -= i;
        printf("%s",buf);
    }

    printf(BOLDYELLOW"\n\tREAD CALLED: %d bytes read.\n" RESET,count);
    return count;
}

int myread(int fd, char *buf, int nbytes)
{
    char * child;
    int i = 0;
    int start = 0;
    int lbk = 0;
    int blk = 0;// the block we want to read from
    __le32 i_buf[BLKSIZE / 4]; //indirect buffer made to  store iblock that contain i blocks
    __le32 di_buf[BLKSIZE / 4]; //double indirect buffer
    char readbuf[BLKSIZE];
    char * cp;
    int count = 0;// keep track of how many bytes we read
    OFT * oftp = running->fd[fd];
    int avil = oftp->inodeptr->INODE.i_size - oftp->offset; // the amount of the file left to read
    MINODE * mip = oftp->inodeptr;

    // if the file isnt in read mode return and quit
    if(oftp->mode == 1 || oftp->mode == 3)
    {
        printf("\ERROR: FILE IS NOT IN A MODE THAT ALLOWS READING.\n");
        return -1;
    }

    //if we find run out of space first or if we read the amount of bytes we need
    while (nbytes && avil)
    {

        start =  oftp->offset % BLKSIZE;
        lbk =  oftp->offset / BLKSIZE;

        //these if statements find the blk number directly or indirectly that we need to find
        // Direct blocks
        if (lbk < 12)
        {
            blk = mip->INODE.i_block[lbk];
        }
        // indirect blocks
        else if (lbk >= 12 && lbk < 256 + 12)
        {
            get_block(mip->dev, mip->INODE.i_block[12], (char *) i_buf);
            blk = i_buf[lbk - 12]; // subtract the first 12 blocks we are and the thirteenth block
        }
        // double indirect blocks
        else
        {
            get_block(mip->dev, mip->INODE.i_block[13], (char *) i_buf);
            // we get the block number into read block that we want.
            // (TOTAL BLOCKS - 12) we know it takes atleast 256+13 (total only the index is 12) blocks to get in here.
            //we divide by 256 because each Index 1-10 for ex represents 256 blks
            // we have -1 because the minimum we can get from the division is 1 not 0.

            int indirect_index = ((lbk - 12) / 256) - 1;
            int direct_index = (lbk-12) % 256;

            get_block(mip->dev, i_buf[indirect_index], (char *) di_buf);

            blk = di_buf[direct_index];

        }


        get_block(mip->dev, blk, readbuf);

        // copy from startByte to buf[ ], at most remain bytes in this block
        cp = readbuf + start;

        int readBytes = nbytes;// try to read as many bytes as possibleŔ

        int remain = BLKSIZE- start; // number of bytes remain in readbuf[]

        if(readBytes > avil)// if we dont have many bytes left in the file
        {
            readBytes = avil;
        }

        if(readBytes > remain)// we check to see if thecurrent amount we are going to read is greater then what
            // what we have in read bytes if it isnt only read the remainder of space left in the block
        {
            readBytes = remain;
        }

        memcpy(buf,cp,readBytes);
        //now we increment our area to copy to aka buf
        buf += readBytes;
        oftp->offset += readBytes;
        avil -= readBytes;
        nbytes -= readBytes;
        count += readBytes;

        /*while (remain > 0)
        {
            *cq++ = *cp++;           // copy byte into buf[]
            oftp->offset++;
            count++;
            avil--;
            nbytes--;
            remain--;
            if (nbytes <= 0 || avil <= 0) break;
        }*/

        // if one data block is not enough, loop back to OUTER while for more ...
    }

    //printf(BOLDYELLOW"\n\tREAD CALLED: %d bytes read.\n" RESET,count);
    mip->dirty = 1;
    return count;
}



