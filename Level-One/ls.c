#include "level1.h"

// Our version of LS. Gets the needed i node then goes to its i blocks and prints its directory entries
int list_dir()
{
    char *input = pathM;
    
    int ino, dev = running->cwd->dev;
    MINODE *mip = running->cwd;

    printf(YELLOW "\t...LIST_DIR(%s)" RESET ": Listing entries...\n", input);

    if (strlen(input)>0)    // if we have a path name find it it else use cwd
    {
        //if (input[0]=='/')// why do we do this <=======
        //{dev = root->dev;}
        ino = getino(dev, input);
        MINODE *mip = iget(dev, ino);
        print_dir_entries(mip);
        iput(mip);
        //iput now since theres nothing refrencing this? <=============
    }
    else
    {
        print_dir_entries(mip);
    }

    // Update directory access time
    //touch_file(input);

    return 0;
}
