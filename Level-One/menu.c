#include "level1.h"
int menu()
{
    printf(RESET WHITE
           "\n"
           "\t----\t----\t----\t----\t----\n" RESET BOLDWHITE
           "\tMENU" RESET WHITE "\tmkdir\trmdir\tls\t----\n"
           "\t----\tcd\tpwd\tcreat\t----\n"
           "\t----\tlink\tunlink\tsymlink\t----\n"
           "\t----\tstat\tchmod\ttouch\t----\n"
           "\t----\trm\tmv\tcat\t----\n"
           "\t----\tpfd\tquit\t\t----\n"
           "\t----\t----\t----\t----\t----\n"
           "\n" RESET);

    return EXIT_SUCCESS;
}
