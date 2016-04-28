#include "level1.h"
int menu()
{
//    printf(BOLDWHITE "\tnunununununununununununununununununununu\n");
//    printf("\tu MENU                                 n\n");
//    printf("\tnunununununununununununununununununununu\n");
//    printf(BOLDWHITE"\tu\n");
//    printf(BOLDWHITE"\tu" BOLDYELLOW "\tmkdir\trmdir\tls\t\n");
//    printf(BOLDWHITE"\tn" BOLDYELLOW "\tcd\tpwd\tcreat\t\n");
//    printf(BOLDWHITE"\tu" BOLDYELLOW "\tlink\tunlink\tsyslink\t\n");
//    printf(BOLDWHITE"\tn" BOLDYELLOW "\tstat\tchmod\ttouch\t\n");
//    printf(BOLDWHITE"\tu" BOLDYELLOW "\trm\tsave\treload\t\n");
//    printf(BOLDWHITE"\tn" BOLDYELLOW "\tquit\n");
//    printf(BOLDWHITE"\tu\n");
//    printf(BOLDWHITE "\tnunununununununununununununununununununu\n\n" RESET);

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

    return 1;
}
