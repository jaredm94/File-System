#ifndef _LEVEL1H_
#define _LEVEL1H_

#include "../Utility-Functions/alloc_dealloc.h" // this includes util.h and project.h

// TODO: Preface each function file with a custom, technical explaination of how it works (for presentation)

/// Premiere functions (done)
int change_dir(char *pathname);
int menu();
int stat_file(char *pathname);
int touch_file(char *pathname);
int chown_file(int newOwner, char *pathname);
int ch_mod(char * path);
int chmod_file(int newMode, char *pathname);
int list_dir(char * input);
int make_dir(char *path);
int creat_file(char *pathname);
int rmdir(char *pathname);
int rm_file(char *pathname);
int link(char *old_pathname);
int symlink(char *old_pathname);
int unlink(char *pathname); /// TODO --------------
int pwd();

/// Helper Functions
int mycreatfile(MINODE *pip, char *name);
int mylinkfile(MINODE *pip, int old_ino, char *nwame);
int mymakedir(MINODE *pip, char *name);
int enter_name(MINODE *pip, int myino, char *myname);
int isFDBusy(MINODE *file);
int isDirEmpty(MINODE *dir);
int rm_child(MINODE *parent, char *name);
int mysymfile(MINODE *pip, char *old_name, char *name);

/// Add implementation files
/*
#include "cd.c"
#include "ls.c"
#include "pwd.c"
#include "touch.c"
#include "stat.c"
#include "menu.c"
#include "mkdir.c"
#include "chown.c"
#include "chmod.c"
#include "creat.c"
#include "link.c"
#include "unlink.c"
#include "symlink.c"
#include "rmdir.c"
#include "rm.c"
*/
#endif
