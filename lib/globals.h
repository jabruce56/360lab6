// Standard clibs that we can get away with cuz swag
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include "ext2.h"

// typecasting and defining custom variables/structs
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
// What is the size of an INODE in an ext2 file system?\
   128 bytes
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;


// MACROS

//ext2 block numbers
#define SUPERBLOCK        1
#define GDBLOCK           2
#define ROOT_INODE        2

//dir and file modes and ext2 magic number
#define DIR_MODE    0040755
#define FILE_MODE   0100644
//#define SLINK_MODE  0120??? where did this come from?
#define SUPER_MAGIC  0xEF53
#define SUPER_USER        0


//proc status
#define FREE              0
#define READY             1
#define RUNNING           2

#define BLKSIZE        1024

//number of ext2 fs structures
#define NMINODE         100
#define NFD              16
#define NPROC            10
#define NOFT            100
#define NMOUNT           10


//ext2 structures
typedef struct minode{ // RAM memory inodes
  INODE         fiji;//yes
  int           dev, ino;
  int           refCount;
  int           dirty;
  int           mounted;
  int           locked;
  struct mount  *mountptr;
}MINODE;

typedef struct mount{  // mount struct for keeping track of disks mounted
  int           dev;
  int           nblocks, ninodes;
  int           bmap, imap, iblk;
  int           busy;
  MINODE        *mounted_inode;
  char          name[64];
  char          mount_name[64];
}MOUNT;

typedef struct oft{ //open file table
  int           mode;
  int           refCount;
  MINODE        *mptr;
  int           offset;
}OFT;

typedef struct proc{ //process structure
  int           pid;
  int           ppid;
  int           status;
  int           uid, gid;
  struct proc   *next;
  struct proc   *parent;
  struct proc   *child;             //first child
  struct proc   *sibling;           //list of other children
  MINODE        *cwd;
  OFT           *fd[NFD];
}PROC;






// global variables, since we can get away with this
SUPER *sp; // super block (of root device)
GD    *gp; // group descriptor (of root device)
INODE *ip; // generic Inode pointer to be a temp inode when inspecting
DIR   *dp; // generic directory pointer to be a temp dir when inspecting

// number of total RAM nodes that we can have out at any given point in time
MINODE minode[NMINODE];
// there can only be one (the original disk that we mount)
MINODE *root;

// Max # of procs allowed and a pointer to the current process (running)
PROC   proc[NPROC], *running;


// Keep track of what disks are mounted within this program
MOUNT  mounttab[NMOUNT];
// Keep track of any/all files that are open under this program
OFT    oft[NOFT];


// The unique file descriptor given to our "root" disk device
int dev;
// Number of blocks of our disk (should be 1440 i.e. 1.44 MB)
int nblocks;
// Number of inodes of our disk (should be ) //idk he assumes 360 on page 36
//ninodes <= (s_inodes_per_group * # block groups)
//we get it currently from s_inodes_count
int ninodes;
int bmap;
int imap;
int inode_start;
char *name[64];


// Core functions for the POS-EXT2FS


#include "../blk_ino_io/bit.c"
#include "../blk_ino_io/blk.c"
#include "../blk_ino_io/ino.c"

#include "../dir/pwd_cd.c"
#include "../dir/path_util.c"
#include "../dir/misc_util.c"
#include "../dir/mk_rm_dir.c"

#include "../file/create_rm.c"
#include "../file/link.c"
#include "../file/open_cp_mv_close.c"
#include "../file/r_w_cat.c"

