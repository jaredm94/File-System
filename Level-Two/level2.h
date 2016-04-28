#ifndef _LEVEL2H_
#define _LEVEL2H_

#include "../Utility-Functions/alloc_dealloc.h"
/*
int cp(char *src);
int cat(char *filepath);
int close_file(char * pathname);
int mv(char * src);
int open_file(char *pathname,int user_mode);
*/
int pfd();
int writeCmd();
int readCmd();
int openCmd();
int mv();
int _lseek();
int cp();
int close_file();//not for main
int cat();
int close_M();// the close for main
int mvCmd();
int cpCmd();
#endif // _LEVEL2H_
