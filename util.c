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
  printf("tokenizepath=%s\n", path);
  token = strtok(path, "/");
  while(token!=NULL&&i<64){
    strcpy(names[i], token);
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

dir(INODE *ip){
  char *buf[BLKSIZE];
  int i;
  DIR *dp;
  char *cp;

  printf("inumber rec_len path\n");
  for(i=0;i<=12;i++)
  {
    if(ip->i_block[i]==0)
      return 0;
    get_block(dev, ip->i_block[i], buf);
    dp = (DIR *)buf;
    cp = buf;
    while(*cp){
      printf("%5d ", dp->inode);
      printf("%6d ", dp->rec_len);
      printf("%6s\n", dp->name);
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
}
u32 search(MINODE *mip, char *pathname){
  int n = 0, i = 0, j = 0;
  char *cp, sbuf[BLKSIZE];
  INODE *ip;
  ip = &(mip->INODE);
  printf("searching for %s...\n", pathname);
  for(i=0;i<12;i++){
    if(ip->i_block[i]==0)
      return 0;
    get_block(dev, ip->i_block[i], sbuf);
    dp=(DIR *)sbuf;
    cp = sbuf;
    while(*cp){
      printf("%5d ", dp->inode);
      printf("%6d ", dp->rec_len);
      printf("%6s\n", dp->name);
      for(j=0;j<strlen(pathname);j++){
        if(dp->name[j]==pathname[j]){
          if(j==strlen(pathname)-1){
            printf("found %s at inumber %u\n", pathname, (u32)dp->inode);
            return dp->inode;
          }
        }
        else{
            break;
        }
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
  if(pathname[0]=='/')
    dev = root->dev;
  else
    dev = running->cwd->dev;
  n=tokenize(pathname);
  for(i=0;i<n;i++){
    printf("search for '%s'\n", name[i]);
    ino = search(running->cwd->INODE, name[i]);
    if(ino==0)
      return 0;
  }
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
      return mip;
    }
  }

  //not in memory yet
  for(i=0;i<100;i++){
    mip=&minode[i];
    if(mip->refCount==0){//take first available minode and init
      if(i==0){
        mip->INODE=ip;
        mip->dev=dev;
        mip->ino=ino;
        mip->refCount=1;
        mip->dirty=0;
        mip->mounted=1;//flag if something is mounted on this minode
        mip->mountptr=&mounttab[0];
        mounttab[0].mounted_inode=mip;
        mip->next=0;
        mip->parent=0;
        mip->child=0;
        mip->sibling=0;
      }
      mip->INODE=ip;
      mip->dev=dev;
      mip->ino=ino;
      mip->refCount=1;
      mip->dirty=0;
      mip->mounted=0;//flag if something is mounted on this minode
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
