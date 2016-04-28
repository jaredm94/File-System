#ifndef ALLOC_DEALLOC_H
#define ALLOC-DEALLOC_H

#include "../project.h"
#include "util.h"
/************ INODES FUNCTIONS ************/
int decFreeInodes(int dev);// decrease the number of available i nodes
int ialloc(int dev);// allocate a new inode on the bitmap and subtract -1 from the number of free nodes
int incFreeInodes(int dev);// increase the number of available i nodes
int idelloc(int dev,int ino);// dellocate a inode  on the bitmap and add +1 t0 the number of free nodes
/************ BLOCK FUNCTIONS ************/
int decFreeBlock(int dev);// decrease the number of available inodes and block #'s'
int balloc(int dev);// allocate a new block on the bitmap and subtract -8 from the number of free nodes
int incFreeBlock(int dev);// increase the number of available inodes and block #'s'
int bdelloc(int dev,int blk);// dellocate a inode  on the bitmap and add +1 t0 the number of free nodes


#endif
