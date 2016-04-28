#include "level2.h"

/*
HOW TO mv (rename)
mv src dest:

1. verify src exists; get its INODE in ==> you already know its dev
2. check whether src is on the same dev as src
              CASE 1: same dev:
3. Hard link dst with src (i.e. same INODE number)
4. unlink src (i.e. rm src name from its parent directory and reduce INODE's
               link count by 1).
              CASE 2: not the same dev:
3. cp src to dst
4. unlink src
*/

int mvCmd()
{

char *src = pathM;
char *sdest = parameterM;
mv(src,sdest);
}

int mv(char *src,char *sdest)
{
    /****PARAMETERS****/
   // char *src = pathM;
    /****PARAMETERS****/

    int sino, dino, dev;
    MINODE *smip, *dmip;


    // Get dev (src and dest must be on same device -- except if we do level III)
    if(src[0] == '/') dev = root->dev;
    else dev = running->cwd->dev;

    // Verify source exists
    sino = getino(dev, src);
    if (sino < 0) return EXIT_FAILURE;

    printf("\tSource exists!\n");

    // Hard link dest with src
    //strcpy(parameterM, sdest); // Second param
    link(src,sdest);
   // list_dir("/Y");
    printf("\tNow unlinking src...\n");

    // Unlink source
    unlink(src,sdest);

    printf(YELLOW "\t...MV(%s, %s)" RESET ": Linked destination to source and unlinked source\n",
           src, sdest);

    return EXIT_SUCCESS;
}
