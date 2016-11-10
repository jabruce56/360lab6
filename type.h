/*************** type.h file ******************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;

//ext2 block numbers
#define SUPERBLOCK        1
#define GDBLOCK           2
#define ROOT_INODE        2

//dir and file modes and ext2 magic number
#define DIR_MODE    0040777
#define FILE_MODE   0100644
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


typedef struct minode{//in memory inodes
  INODE        *INODE;
  int          dev, ino;
  int          refCount;
  int          dirty;
  int          mounted;
  struct mount *mountptr;
  struct proc  *next;
  struct proc  *parent;
  struct proc  *child;
  struct proc  *sibling;
}MINODE;

typedef struct mount{
  int    dev;
  int    nblocks, ninodes;
  int    bmap, imap, iblk;
  MINODE *mounted_inode;
  char   name[64];
  char   mount_name[64];
}MOUNT;

typedef struct oft{//open file table
  int    mode;
  int    refCount;
  MINODE *mptr;
  int    offset;
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          ppid;
  int          status;
  int          uid, gid;
  MINODE       *cwd;
  OFT          *fd[NFD];
}PROC;
