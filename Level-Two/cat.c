#include "level2.h"

int cat()
{
    /****PARAMETER****/
    char * filepath = pathM;
    /****PARAMETER****/

    char temp[2][126];
    char mybuf[1024];// load the file into this
    int n = 0;
    int fd = 0;
    int count = 0;
    fd = open_file(filepath,0); //open file

    if(fd == -1 )
    {
        printf("\nERROR: OPEN FAILED.\n");
        return -1;
    }

    while( n = myread(fd, mybuf,1024))
    {
        mybuf[n] = 0;// for a nullterminated string maybe find my printf
        printf(BOLDRED"%s"RESET, mybuf); //LAZY WAY TO DO CAT FIX LATER
        count += n;
    }

    /* discard all characters up to and including newline */

    printf(BOLDRED"\n\tCat printed: %d\n"RESET,count);

//clear the buffer.

    close_file(filepath);

    return 1;
}
