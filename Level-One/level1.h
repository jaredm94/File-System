#ifndef _LEVEL1H_
#define _LEVEL1H_

#include "../Utility-Functions/alloc_dealloc.h" // this includes util.h and project.h

/// Premiere functions (done)
int change_dir();
int menu();
int stat_file();
int touch_file();
int chown_file();
int chmod_file();
int list_dir();
int make_dir();
int creat_file();
int rmdir();
int rm_file();
int link();
int symlink();
int unlink();
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


int unlinkCmd();
int touch_fileCmd();
int symlinkCmd();
int rmdirCmd();
int rm_fileCmd();
int make_dirCmd();
int linkCmd();
int creat_fileCmd();
#endif
