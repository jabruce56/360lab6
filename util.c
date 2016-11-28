//#include "type.h"
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
  //printf("tokenizepath=%s\n", path);
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

pdir(INODE *tip){
  struct stat nstat;
  char *buf[BLKSIZE];
  int i, j;
  DIR *dp;
  char *cp;
  for(i=0;i<=12;i++)
  {
    if(tip->i_block[i]==0)
      return 0;
    get_block(dev, tip->i_block[i], buf);
    dp = (DIR *)buf;
    cp = buf;
    while(*cp){
      printf("%3d %6d %6s\n", dp->inode, dp->rec_len, dp->name);
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
}
u32 search(MINODE *mip, char *pathname){
  int n = 0, i = 0, j = 0;
  char *cp, sbuf[BLKSIZE];
  ip = &(mip->INODE);
  for(i=0;i<12;i++){
    if(ip->i_block[i]==0)
      return 0;
    get_block(dev, ip->i_block[i], sbuf);
    dp=(DIR *)sbuf;
    cp = sbuf;
    while(*cp){
      for(j=0;j<strlen(pathname);j++){
        if(dp->name[j]==pathname[j]){
          if(j==strlen(pathname)-1){
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
u32 getino(int *dev, char *pathname){//returns inode # of a pathname
  int n, i, ino, inostrt;
  if(pathname[0]=='/')
    dev = root->dev;
  else
    dev = running->cwd->dev;
  n=tokenize(pathname);
  for(i=0;i<n;i++){
    ino = search(running->cwd->INODE, name[i]);
    if(ino==0)
      return 0;
    //printf("found:%s at:%d\n", name[i], ino);
  }
  return ino;
}
MINODE *iget(int dev, u32 ino){
  //read from disk, put into minode
  char buf[BLKSIZE];
  int i, blk, offset;
  MINODE *mip;
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
        printf("m%d i%d\n",mip->INODE->i_size, ip->i_size);
        mip->dev=dev;
        mip->ino=ino;
        mip->refCount=1;
        mip->dirty=0;
        mip->mounted=1;//flag if something is mounted on this minode
        mip->locked=1;
        mip->mountptr=&mounttab[0];
        mounttab[0].mounted_inode=mip;

      }
      mip->INODE=ip;
      mip->dev=dev;
      mip->ino=ino;
      mip->refCount=1;
      mip->dirty=0;
      mip->mounted=0;//flag if something is mounted on this minode
      mip->locked=1;
      mip->mountptr=0;
      return mip;
    }
  }
}
int iput(MINODE *mip){//release inode from memory
  int ino, blk, offset;
  char *buf[BLKSIZE];
  if(--mip->refCount==0&&mip->dirty){//if dirty and last reference in memory
    mip->locked=0;
    memcpy(buf, mip->INODE, BLKSIZE);
    put_block(mip->dev, blk, buf);//write back
  }
  else if(mip->refCount>0||!mip->dirty)
    return 0;
}
int findname(MINODE *parent, int myino, char *myname){
  int n = 0, i = 0, j = 0;
  char *cp, buf[BLKSIZE];
  ip = parent->INODE;
  for(i=0;i<12;i++){
    if(ip->i_block[i]==0)
      return -1;
    get_block(dev, ip->i_block[i], buf);
    dp=(DIR *)buf;
    cp = buf;
    while(*cp){
      if(dp->inode==myino){
        if(myino==2){
          strcpy(myname,"/");
        }
        else
          strcpy(myname, dp->name);
        // for(i=0;i<dp->name_len;i++){
        //   printf("%c", dp->name[i]);
        // }
      }
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
  printf("search found nothing with the inode %s\n", myino);
  return 0;
}
int findino(MINODE *mip, int *myino, int *parentino){
  char *cp, buf[BLKSIZE];
  ip=mip->INODE;
  get_block(dev, ip->i_block[0], buf);
  dp=(DIR *)buf;
  cp=buf;
  *myino=dp->inode;
  //printf("%d\n",dp->inode);
  cp+=dp->rec_len;
  dp=(DIR *)cp;
  *parentino=dp->inode;
  //printf("%d\n",dp->inode);
}
listdir (char *pathname){
  int i, ino = 0, dev = running->cwd->dev;
  MINODE *mip = running->cwd;

  if (pathname[0]!='\0'){   // ls pathname:
    pathname[strlen(pathname)]='\0';
    ino = getino(&dev, pathname);
    if(ino)
      mip = iget(dev, ino);
  }
  pdir(mip->INODE);
  // mip points at minode;
  // Each data block of mip->INODE contains entries
  // print the name strings of the entries
}
ch_dir (char *pathname){
  MINODE *mip;
  int ino;
  if(pathname[0]=='\0'||
    (pathname[0]=='/'&&strlen(pathname)==1)){//cd to root;
      dev = root->dev;
      iput(running->cwd);
      running->cwd=root;

  }
  else if(pathname[0]=='/'&&strlen(pathname)>1){
    //go to root first
    dev = root->dev;
    iput(running->cwd);
    running->cwd=root;
    ino=getino(&dev, &pathname[1]);
    if(ino)
      running->cwd=iget(dev, ino);
  }
  else{//cd to pathname;
    ino = getino(&dev, pathname);
    //printf("switching to inode %d\n", ino);
    if(ino){
      running->cwd=iget(dev, ino);
    }
    else
      printf("invalid pathname for cd\n");

  }
}
pwd (MINODE *mip){
  MINODE *parent;
  char mname[128], temp[128], pname[128];
  int myino=0, parentino=1;
  memset(mname, '\0', 128);
  memset(temp, '\0', 128);
  memset(pname, '\0', 128);
  while(1){
    findino(mip, &myino, &parentino);
    parent = iget(dev, parentino);
    findname(parent, myino, mname);
    strcpy(temp, mname);
    strcat(temp, pname);
    strcpy(pname, temp);
    if(!strncmp(mname, "/", 1))
      break;
    mip=parent;
  }
  printf("%s\n", pname);
}
kmkdir(MINODE *pmip, char *path){
  MINODE *mip;
  int ino, blk, i;
  // 5-1. allocate an INODE and a disk block:
  ino = ialloc(dev);
  blk = balloc(dev);
  mip = iget(dev,ino); // load INODE into an minode
  mip->INODE->i_block[0]=blk;
  for(i=1;i<12;i++){mip->INODE->i_block[i]=0;}
  mip->dirty=1;
  iput(mip);
  //make . and ..
  //enter_child

  // 5-2. initialize mip->INODE as a DIR INODE;
  // mip ->INODE.i_block[0] = blk; other i_block[ ] are 0;
  // mark minode modified (dirty);
  // iput(mip); // write INODE back to disk
  // 5-3. make data block 0 of INODE to contain . and .. entries;

  // write to disk block blk.
  // 5-4. enter_child(pmip, ino, basename); which enters
  // (ino, basename) as a DIR entry to the parent INODE;
}
mk_dir(char *pathname){
  char temp[128], *dname, *bname;
  int pino, d=0, b=0;
  MINODE *pmip;
  strcpy(temp, pathname);
  dname=dirname(temp);
  bname=basename(pathname);
  if(pathname[0]=='/'){
    dev=root->dev;
    if(!search(dname, root))
      d=1;
  }
  else{
    dev = running->cwd->dev;
    if(!search(dname, running->cwd))
      d=1;
  }
  if(d=0){
    printf("dirname does not exist\n");
    return 0;
  }
  pino=getino(&dev, dname);
  pmip=iget(dev,dname);
  if(search(pmip, bname)){
    printf("basename already exists\n");
    return 0;
  }
  kmkdir(pmip, bname);


}
