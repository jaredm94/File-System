#ifndef UTIL_H
#define UTIL_H
#include "../project.h"

// need to put all of function definitions in here and a comment saying what they do. Easier for us as well.

int getabsname(MINODE *mip, char name[]);

int get_block(int fd, int blk, char buf[ ]);
int put_block(int fd, int blk, char buf[ ]);
int tst_bit(char *buf, int bit);// test to see whether a bit is "on" == 1
int set_bit(char *buf, int bit);// set a bit
int clr_bit(char *buf, int bit);// clear a bit
// Uses name[] to search from the root of fd (file descriptor) to find the inode from the given pathname and
// returns memory location of the of the inode.
int getino(int fd, char *name);
/*
This function searches the data blocks of a DIR inode (inside an Minode[])
for name. You may assume a DIR has only 12 DIRECT data blocks. Return its inode number
*/
// Note: Will search for child name in given mip
int search(MINODE *mip, char *name);

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
MINODE *iget(int dev, int ino);

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
int iput(MINODE *mip);
/*
   Given the parent DIR (MINODE pointer) and myino, this function finds
   the name string of myino in the parent's data block. This is the SAME
   as SEARCH() by myino, then copy its name string into myname[ ].
*/
int findmyname(MINODE *mip, int myino, char myname[]);
/*
 For a DIR Minode, extract the inode numbers of . and ..
  Read in 0th data block. The inumbers are in the first two dir entries.
  */
int findino(MINODE *mip, int *myino,int *parentino);
int tokenize(char path[],char returnval[][128],char * tok);// takes a path and a token and tokenizes the string based on the token.
int print_dir_entries(MINODE *mip);

#endif
