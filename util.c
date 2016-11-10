//#include "type.h"
//int fd;
char *name[64];

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
  //!!!!!!!!! probably doesnt follow a path of more than 1!!!!!!!but idk havent tested
  int n, i, ino, inostrt;
  n=tokenize(pathname);
  for(i=0;i<n;i++){
    ino = search(dev, name[i]);
    if(ino==0)
      return 0;
  }
  printf("%s\n", pathname);
  return ino;
}
MINODE *iget(int dev, u32 ino){
  //read from disk, put into minode
  char buf[BLKSIZE];
  int i, blk, offset;
  MINODE *mip;
  INODE *ip;

  //find inode on disk w/ mailmans
  blk=(ino-1)/8+mounttab[0].iblk;
  offset=(ino-1)%8;
  get_block(dev, blk, buf);
  ip=(INODE *)buf+offset;

  //check if inode is already in memory
  for(i=0;i<100;i++){
    mip=&minode[i];
    if(mip->refCount>0 &&  mip->dev==dev && mip->ino ==ino){
      mip->refCount++;//if so inc ref count and return
      return &mip;
    }
  }

  //not in memory yet
  for(i=0;i<100;i++){
    mip=&minode[i];
    if(mip->refCount==0){//take first available minode
      mip->INODE=ip;
      mip->dev=dev;
      mip->ino=ino;
      mip->refCount=1;
      mip->dirty=0;
      mip->mounted=0;
      mip->mountptr=0;
      mip->next=0;
      mip->parent=0;
      mip->child=0;
      mip->sibling=0;
      return mip;
    }
  }
}
int iput(MINODE *mip){//release inode from memory
  int ino, blk, offset;
  INODE *ip;
  char *buf[BLKSIZE];

  if(--mip->refCount==0&&mip->dirty){//if dirty and last reference in memory
    memcpy(buf, mip->INODE, BLKSIZE);
    put_block(mip->dev, blk, buf);//write back
  }
  else if(mip->refCount>0||!mip->dirty)
    return 0;
}
