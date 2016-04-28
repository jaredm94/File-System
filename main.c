// -------------------------
// Luke Weber, Jared Meade
// CptS 360 - Final Project
// -------------------------

// Project header
#include "Level-One/level1.h"
#include "Level-Two/level2.h"

// Globals
MINODE minode[NMINODES];
MINODE *root = 0;
PROC procs[NPROC];
PROC *running;
GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp;

char cmdM[64],pathM[64],parameterM[64];// break up the command

char lineM[256];// input command into this

char *cmdList[] = {
    "menu", "cd", "chmod", "create",
    "link", "ls", "mkdir", "rm", "rmdir", "stat", "symlink",
    "touch","unlink","cat", "cp","mv", "pwd", "pfd","write",
    "read","open","lseek","close"
};
int (*fptr[])() = {
    menu,change_dir,chmod_file,creat_fileCmd,linkCmd,list_dir,make_dirCmd,rm_fileCmd,rmdirCmd,
    stat_file,symlinkCmd,touch_fileCmd,unlinkCmd,cat,cpCmd,mvCmd,pwd,pfd,writeCmd,readCmd,openCmd,
    _lseek,close_M,0
};

char *disk = "diskimage", *seperator = "--------------------------------------------------------";
char buf[1024];

/// STARTUP FUNCTIONS

// Setup project
int init()
{
    // Initialize processes
    procs[0].uid=0;
    procs[0].cwd=0;
    procs[1].uid=1;
    procs[1].cwd=0;

    /// TODO: Fix that we're allocating more memory than we need by allocating all the FDs
    /// Setup open file table
    int i = 0;
    while(i < NFD)
    {
        // Do for both processes
        int j;
        for (j = 0; j < 2; j++)
        {
            // Create new struct
            procs[j].fd[i] = malloc(sizeof(OFT));
            // Setup members
            procs[j].fd[i]->inodeptr = 0;
            procs[j].fd[i]->mode = 0;
            procs[j].fd[i]->offset = 0;
            procs[j].fd[i]->refCount = 0;
        }
        // Note: Fixed segfaults with the above struct array initialization

        i++;
    }

    printf("\t...First and second process OFT initiated with %d entries\n", NFD);

    // Set inodes with no references
    i = 0;
    while(i < NMINODES)
    {
        minode[i].refCount=0;
        i++;
    }

    printf("\t...%d minodes initiated with refCount = 0\n", NMINODES);

    // No root
    root = 0;

    printf("\t...root = 0\n");

    // Set running process
    running = &procs[0];

    printf("\t...Set running process as procs[0]\n");

    return 1;
}

// Mount root file system, establish / and CWDs
int mount_root()
{
    // Attempt to open disk for read/write
    int fd = open(disk, O_RDWR);

    if (fd < 0)
    {
        printf("\t...Open failed :(\n");
        exit(1);
    }
    printf("\t...Open successful (with fd = %d)\n", fd);

    // Read super block
    get_block(fd, 1, buf);
    sp = (SUPER *) buf;

    printf("\t...Got superblock\n");

    // Check if EXT2
    if (sp->s_magic != 0xEF53)
    {
        printf("\t...Not an EXT2 file system\n");
        exit(1);
    }
    printf("\t...EXT2 file system found\n");

    // Root inode
    root = iget(fd, 2); // running->fd instead of dev

    // Processes current working directory
    procs[0].cwd = iget(fd, 2);
    procs[1].cwd = iget(fd, 2);

    return 1;
}

int findCmd(char *command)
{
    int i = 0;
    while(*fptr[i])
    {
        if (strcmp(command, cmdList[i])==0)
            return i;
        i++;
    }
    return -1;
}

// Driver
int main(int argc, char *argv[])
{
    int cmdindex;
    char tok[3][128];// just for the write
    char *seperator = "--------------------------------------------------------";

    printf("%s\n", seperator);
    printf(BOLDWHITE "\n[1] INITIATE\n" RESET);
    init();
    printf(BOLDWHITE "\n[2] MOUNT ROOT\n" RESET);
    mount_root();

    // Array of command functions
    printf(BOLDWHITE "\n[3] TERMINAL\n" RESET);

    // Print menu
    menu();

    while(1)
    {
        char absname[256] = {0};
        getabsname(running->cwd, absname);
        strcpy(running->cwd->name, absname);

        // Get user input
        printf(BOLDBLUE "\t[P%d %s]$ " RESET, running->pid, running->cwd->name);

        memset(lineM,0,256);
        memset(cmdM,0,64);
        memset(pathM,0,64);
        memset(parameterM,0,64);
        fgets(lineM, 256, stdin);
        lineM[strlen(lineM) - 1] = 0;// get rid of newline

        if (lineM[0] == 0) continue;

        sscanf(lineM, "%s %s %s", cmdM, pathM, parameterM);


        cmdindex = findCmd(cmdM);

        //if we are writing a string we need to sscanf differently
        if(cmdindex == 18)
        {
            tokenize(lineM,tok,"\"");
            strcpy(parameterM,tok[1]);

        }

        if(cmdindex != -1)
        {
            (*fptr[cmdindex])(pathM);
        }
        else
        {
            printf(RED"ERROR: INCORRECT COMMAND.\n" RESET);
        }
        printf("\n");
    }
    printf("%s\n", seperator);
    return 0;
}
