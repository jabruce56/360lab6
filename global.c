#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "ext2.h"
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

MINODE minode[NMINODE];
MINODE *root;

// The process that is running
PROC   proc[NPROC], *running;

MOUNT  mounttab[NMOUNT];
OFT    oft[NOFT];

int dev;
int nblocks;
int ninodes;
int bmap;
int imap;
int inode_start;
