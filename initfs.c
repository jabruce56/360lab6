
#include "type.h"
#include "include.h"


int init(){ // Initialize data structures of LEVEL-1:
     int i, j;
     PROC *p;
     //reset global proc and minode
     for(i=0;i<NPROC;i++){
       //if(i==0||i==1){proc[i].uid=i;}
       proc[i].status=FREE;
       for(j=0;j<NFD;j++){proc[i].fd[j]=0;}
       proc[i].next = &proc[i+1];
     }
     for(i=0;i<NMINODE;i++){minode[i].refCount=0;}
     for(i=0;i<NOFT;i++){oft[i].refCount=0;}

     //2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
     //(3). MINODE *root = 0;
     printf("mounting root..\n");
     mount_root();
     printf("mount complete\n");
   }
//
// 4.. Write C code for
//    Write C code for
//          int ino     = getino(int *dev, char *pathname)
//          MINODE *mip = iget(dev, ino)
//
//                        iput(MINDOE *mip)
int mount_root(){  // mount root file system, establish / and CWDs
  int i, ino, fd;
  SUPER *sp;
  MOUNT *mp;
  MINODE *ip;
  char buf[BLKSIZE], *rootdev;

//  open device for RW (get a file descriptor dev for the opened device)
  dev = open("disk", O_RDWR);
  if(dev<0){
    printf("open disk failed\n");
    exit(1);
  }

//  read SUPER block to verify it's an EXT2 FS and save info
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  if(sp->s_magic!=SUPER_MAGIC){
    printf("not a valid ext2 file system\n");
    exit(0);
  }
  printf("valid ext2 fs\n");
  mp = &mntTable[0];
  nblocks = sp->s_blocks_count;
  ninodes = sp->s_inodes_count;
  mp->ninodes=ninodes;
  mp->nblocks=nblocks;

  //save group desc info
  get_block(dev, 2, buf);
  gp = (GD *)buf;
  mp->dev=dev;
  mp->bmap=gp->bg_block_bitmap;
  mp->imap=gp->bg_inode_bitmap;
  mp->iblk=gp->bg_inode_table;

  strcpy(mp->name, "disk");
  strcpy(mp->mount_name, "/");
  printf("bmap=%d  ",   gp->bg_block_bitmap);
  printf("imap=%d  ",   gp->bg_inode_bitmap);
  printf("iblock=%d\n", gp->bg_inode_table);
  root=iget(dev, 2);
  mp->mounted_inode=root;
  root->mptr = mp;
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
    //printf("%d\n", getino(2, "a/b");
    init();
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
