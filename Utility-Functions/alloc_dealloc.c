#include "alloc_dealloc.h"


extern GD    *gp;
extern SUPER *sp;
extern INODE *ip;
extern DIR   *dp;

// ask about alloc i node and getting a full block
// ask about dirty and refcount < 0

/************ INODES FUNCTIONS ************/
int decFreeInodes(int dev)// decrease the number of available i nodes
{
  char buf[BLKSIZE];

  // dec free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, buf);
}

int ialloc(int dev)// allocate a new inode on the bitmap and subtract -1 from the number of free nodes
{
  int  i;
  char buf[BLKSIZE];
  int imap;// start for the i map
  int ninodes;// number of inodes

  //Set the number of inodes
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  ninodes=sp->s_inodes_count;

   // Read group descriptor 0
    get_block(dev, 2, buf);
    gp = (GD *) buf;

    // Inode bitmap
    imap = gp->bg_inode_bitmap;


  // read inode_bitmap block
  get_block(dev, imap, buf);

  for (i=0; i < ninodes; i++){
    if (tst_bit(buf, i)==0){// if you found a free i node
       set_bit(buf,i);// set the bit
       decFreeInodes(dev);// subtract from the number of free blocks

       put_block(dev, imap , buf);// put the new map back

       return i+1;
    }
  }
  printf("ialloc(): no more free inodes\n");
  return 0;
}


int incFreeInodes(int dev)// increase the number of available i nodes
{
  char buf[BLKSIZE];

  // dec free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf);
}

int idelloc(int dev,int ino)// dellocate a inode  on the bitmap and add +1 t0 the number of free nodes
{
  int  i;
  char buf[BLKSIZE];
  int imap;// start for the i map
  int ninodes;// number of inodes

  //set the ninodes
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  ninodes=sp->s_inodes_count;

   // Read group descriptor 0
    get_block(dev, 2, buf);
    gp = (GD *) buf;

    // Inode bitmap
    imap = gp->bg_inode_bitmap;

  // read inode_bitmap block
  get_block(dev, imap, buf);

  // test the inode to see if its already empty if not set it. ino starts at 1 thats why -1.
    if (tst_bit(buf, ino-1)==1){
       clr_bit(buf,ino-1);// set the bit
       incFreeInodes(dev);// subtract from the number of free blocks
       put_block(dev, imap , buf);// put the new map back
       return ino;
    }

  printf("ialloc(): no more free inodes\n");
  return 0;
}

/************ END OF INODE FUNCTIONS ************/




/************ BLOCK FUNCTIONS ************/


int decFreeBlock(int dev)// decrease the number of available inodes and block #'s'
{
  char buf[BLKSIZE];

  // dec free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count-=1;
  sp->s_free_inodes_count-=8;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count-=1;
  gp->bg_free_inodes_count-=8;
  put_block(dev, 2, buf);
}

int balloc(int dev)// allocate a new block on the bitmap and subtract -8 from the number of free nodes
{
  int  i;
  char buf[BLKSIZE];
  int bmap;// start for the i map
  int nblocks;// number of inodes

  //set the blockcount
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  nblocks=sp->s_blocks_count;


   // Read group descriptor 0
    get_block(dev, 2, buf);
    gp = (GD *) buf;

    // Block map
    bmap = gp->bg_block_bitmap;


  // read block_bitmap block
  get_block(dev, bmap, buf);

  for (i=0; i < nblocks; i++){
    if (tst_bit(buf, i)==0){// if you found a free i node
       set_bit(buf,i);// set the bit
       decFreeBlock(dev);// subtract from the number of free blocks
       put_block(dev, bmap , buf);// put the new map back

       // clear the block of any previous memory.
       memset(buf,0,BLKSIZE);
       put_block(dev,i+1,buf);

       return i+1;
    }
  }
  printf("Balloc(): no more free inodes\n");
  return 0;
}


int incFreeBlock(int dev)// increase the number of available inodes and block #'s'
{
  char buf[BLKSIZE];

  // dec free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count+=1;
  sp->s_free_inodes_count+=8;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count+=1;
  gp->bg_free_inodes_count+=8;
  put_block(dev, 2, buf);
}

int bdelloc(int dev,int blk)// dellocate a inode  on the bitmap and add +1 t0 the number of free nodes
{
  int  i;
  char buf[BLKSIZE];
  int bmap;// start for the i map
  int nblocks;// number of inodes

  //set the blockcount
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  nblocks=sp->s_blocks_count;

   // Read group descriptor 0
    get_block(dev, 2, buf);
    gp = (GD *) buf;

    // block bitmap
    bmap = gp->bg_block_bitmap;

  // read block_bitmap block
  get_block(dev, bmap, buf);

  // test the block bitmap to see if its already empty if not set it.
    if (tst_bit(buf, blk-1)==1){
       clr_bit(buf,blk-1);// set the bit minus 1 because blocks start at 1 and 0 is its equal in the bitmap array
       incFreeBlock(dev);// subtract from the number of free blocks
       put_block(dev, bmap , buf);// put the new map back

       //now we set the memory to zero in the block
       memset(buf,0,BLKSIZE);// reset everything for safety reasons.
       put_block(dev,blk,buf);

       return blk;
    }

  printf("ialloc(): no more free inodes\n");
  return 0;
}



/************ END OF BLOCK FUNCTIONS ************/

