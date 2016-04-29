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

int cat();

int close_M();
int close_file(char * pathname);
int close_help(int fd);

int cpCmd();
int cp(char * src, char * dest);


int _lseek();

int mvCmd();
int mv(char *src,char *sdest);

int open_file(char * pathname, int user_mode);
int openCmd();

int pfd();

int readCmd();
int myread(int fd, char *buf, int nbytes);

int mywrite(int fd, char buf[], int nbytes);
int writeCmd();

#endif // _LEVEL2H_
