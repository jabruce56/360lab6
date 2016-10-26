//#include "type.h"

char buf[BLKSIZE], name[64];
int fd;

int get_block(int fd, int blk, char buf[ ]){
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}
int put_block(int fd, int blk, char buf[ ]){
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, buf, BLKSIZE);
}
int tokenize(char *pathname){
  char *token, path[128], names[64][64];
  int i = 0, j;
  strcpy(path, pathname);
  token = strtok(path, "/");
  while(token!=NULL&&i<64){
    for(j=0;j<strlen(token);j++){names[i][j]=token[j];}
    name[i]=names[i];
    token = strtok(NULL, "/");
    i++;
  }
  return i;
}
              //http://man7.org/linux/man-pages/man3/basename.3.html
              // path       dirname   basename
              // /usr/lib   /usr      lib
              // /usr/      /         usr
              // usr        .         usr
              // /          /         /
              // .          .         .
              // ..         .         ..


u32 search(MINODE *mip, char *name){
  int n = 0, i = 0, j = 0;
  char *cp, sbuf[BLKSIZE];
  INODE *ip;
  ip = &(mip->INODE);
  printf("\nsearching for %s...\n", name);
  for(i=0;i<12;i++){
    if(ip->i_block[i]==0)
      return 0;
    dp=(DIR *)sbuf;
    cp = sbuf;
    while(cp<sbuf+BLKSIZE){
      for(j=0;j<strlen(name);j++)
        if(dp->name[j]==name[j]){
          if(j==strlen(name)-1){
            printf("found %s at inumber %u\n", name, (u32)dp->inode);
            return dp->inode;
          }
        }
        else{
            break;
        }
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
  printf("search found nothing with the name %s\n", name);
  return 0;
}
u32 getino(int *dev, char *pathname){//returns inode # of a pathname.
  int n, i, ino, inostrt;
  n=tokenize(pathname);
  for(i=0;i<n;i++){
    ino = search(dev, name[i]);
    if(ino==0){
      return 0;
    }
  }
  printf("%s\n", pathname);
  return ino;
}
MINODE *iget(int dev, u32 ino){
  int i;
  MINODE *mip;
  for(i=0;i<NMINODE;i++){
    if(minode[i].refCount>0 &&  minode[i].dev==dev && minode[i].ino ==ino){
      minode[i].refCount++;
      return &minode[i];
    }
  }
  for(i=0;i<MINODE;i++){
    if(minode[i].refCount == 0){
      *mip = &minode[i];
    }
  }
  
}
int iput(MINODE *mip){

}
