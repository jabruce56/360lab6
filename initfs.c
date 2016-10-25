#include <stdio.h>
#include <stdlib.h>

#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "type.h"
#include "util.c"

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

int dev;
int nblocks;
int ninodes;
int bmap;
int imap;
int inode_start;

int init(){ // Initialize data structures of LEVEL-1:
     int i;
     //(1). 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
     for(i=0;i<NPROC;i++){
       if(i==0||i==1){proc[i].uid=i;}
       proc[i].cwd=0;
     }

     //(2). MINODE minode[100]; all with refCount=0
     for(i=0;i<NMINODE;i++){minode[i].refCount=0;}

     //(3). MINODE *root = 0;
     root = 0;
   }
//
// 4.. Write C code for
//    Write C code for
//          int ino     = getino(int *dev, char *pathname)
//          MINODE *mip = iget(dev, ino)
//
//                        iput(MINDOE *mip)
int mount_root(){  // mount root file system, establish / and CWDs
//
//  open device for RW (get a file descriptor dev for the opened device)
//  read SUPER block to verify it's an EXT2 FS
//
//  root = iget(dev, 2);    /* get root inode */
//
//  Let cwd of both P0 and P1 point at the root minode (refCount=3)
//      P0.cwd = iget(dev, 2);
//      P1.cwd = iget(dev, 2);
}
// 6. ls [pathname] command:
//    {
//       int ino, dev = running->cwd->dev;
//       MINODE *mip = running->cwd;
//
//       if (pathname){   // ls pathname:
//           if (pathname[0]=='/')
//              dev = root->dev;
//           ino         = getino(&dev, pathname);
//           MINODE *mip = iget(dev, ino);
//       }
//       // mip points at minode;
//       // Each data block of mip->INODE contains DIR entries
//       // print the name strings of the DIR entries
//    }
//
// 7. cd(char *pathname)
//    {
//       if (no pathname)
//          cd to root;
//       else
//          cd to pathname;
//    }
//
int main()
  {
    getino(2, "/a/b/c/d");
     //init();
    //  mount_root();
//      // ask for a command string, e.g. ls pathname
//      ls(pathname);
//      // ask for a command string, e.g. cd pathname
//      cd(pathname);
//      // ask for a command string, e.g. stat pathname
//      stat(pathname, &mystat); // struct stat mystat; print mystat information
  }
//
// 9.int quit()
//   {
//       iput all DIRTY minodes before shutdown
//   }
