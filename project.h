// -------------------------
// Luke Weber, Jared Meade
// CptS 360 - Final Project
// -------------------------

// Prevent redefintions
#ifndef _PROJECTH_
#define _PROJECTH_

// Import headers
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>

// Personal header
#include "ext2_fs.h"
#include "types.h"

// Custom types
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

// ...
#define BLOCK_SIZE        1024
#define BLKSIZE           1024
#define BITS_PER_BLOCK    (8*BLOCK_SIZE)
#define INODES_PER_BLOCK  (BLOCK_SIZE/sizeof(INODE))

// Block number of EXT2 FS on FD
#define SUPERBLOCK        1
#define GDBLOCK           2
#define BBITMAP           3
#define IBITMAP           4
#define INODEBLOCK        5
#define ROOT_INODE        2

// Default dir and regular file modes
#define DIR_MODE          0040777
#define FILE_MODE         0100644
#define SUPER_MAGIC       0xEF53
#define SUPER_USER        0

// Proc status
#define FREE              0
#define BUSY              1
#define KILLED            2

// Table sizes
#define NMINODES          50
#define NMOUNT            10
#define NPROC             10
#define NFD               10
#define NOFT              50

// Open File Table
typedef struct Oft {
  int   mode;
  int   refCount;
  struct Minode *inodeptr;
  long  offset;
} OFT;

// PROC structure
typedef struct Proc {
  int   uid;
  int   pid;
  int   gid;
  int   ppid;
  int   status;

  struct Minode *cwd;
  OFT   *fd[NFD];

  struct Proc *next;
  struct Proc *parent;
  struct Proc *child;
  struct Proc *sibling;
} PROC;

// In-memory inodes structure
typedef struct Minode{
  INODE INODE;               // disk inode
  int   dev, ino;

  int   refCount;
  int   dirty;
  int   mounted;
  struct Mount *mountptr;
  char     name[128];           // name string of file
} MINODE;

// Mount Table structure
typedef struct Mount{
  int  ninodes;
  int  nblocks;
  int  bmap;
  int  imap;
  int  iblock;
  int  dev, busy;
  struct Minode *mounted_inode;
  char   name[256];
  char   mount_name[64];
} MOUNT;

 // need to put all of function definitions in here and a comment saying what they do. Easier for us as well.
extern MINODE minode[NMINODES];
extern MINODE *root;
extern PROC procs[NPROC];
extern PROC *running;
extern GD    *gp;
extern SUPER *sp;
extern INODE *ip;
extern DIR   *dp;

// these are use in main and are use for breaking up user input
extern char cmdM[64];
extern char pathM[64];
extern char parameterM[64];// break up the command
extern char lineM[256];// input command into this

#endif
