#include "level2.h"


int close_M()
{

int fd = atoi(pathM);
close_help(fd);


}


int close_file(char * pathname)
{
    /****PARAMETER****/
    //char * pathname=pathM;
    /****PARAMETER****/

    char * child;

    int i = 0;

    int ino = getino(running->cwd->dev,pathname);
    MINODE * hold = iget(running->cwd->dev, ino);

    for(i=0; i < NFD; i++)
    {
        if(running->fd[i]->inodeptr==hold)
        {
            close_help(i);
        }
    }

    iput(hold);// close the MINODE reference.

}


int close_help(int fd)
{


    if(fd >= NFD)
    {
        return -1;
    }

//3. The following code segments should be fairly obvious:
    OFT * oftp = running->fd[fd];
    //running->fd[fd] = 0;
    oftp->refCount--;
    if (oftp->refCount > 0) return 0;


    // last user of this OFT entry ==> dispose of the Minode[]
    MINODE * mip = oftp->inodeptr;
    iput(mip);

    //reset all of the members of the open file
    oftp->mode = 0 ;
    oftp->offset = 0;
    oftp->inodeptr = 0;

    return 0;
}
