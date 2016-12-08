/******************************************************************************/
/**********************************getino**************************************/
/******************************************************************************/

u32 getino(int *dev, char *pathname){//returns inode # of a pathname
  int n, i, ino, inostrt;
  if(pathname[0]=='/')        //update dev
    dev = root->dev;
  else
    dev = running->cwd->dev;

  n=tokenize(pathname);       //get tokenized pathname for searching
  for(i=0;i<n;i++){
    ino = search(running->cwd->INODE, name[i]);//search through all tokens
    if(ino==0)                //if nothing found, return nothing
      return 0;
  }
  return ino;
}

/******************************************************************************/
/***********************************iget***************************************/
/******************************************************************************/

MINODE *iget(int dev, u32 ino){//takes ino and puts its INODE into memory
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

/******************************************************************************/
/************************************iput**************************************/
/******************************************************************************/

int iput(MINODE *mip){//release inode from memory
  int ino, blk, offset;
  char *buf[BLKSIZE];
  if(--mip->refCount == 0 && mip->dirty)
  {
    //if dirty and last reference in memory
    mip->locked = 0;

    //TODO: What if the the buf to write to this inode exceeds 1KB?
    // We win
    memcpy(buf, mip->INODE, BLKSIZE);
    put_block(mip->dev, blk, buf);//write back
  }
  else if(mip->refCount > 0 || !mip->dirty)
    return 0;
}

/******************************************************************************/
/**********************************findino*************************************/
/******************************************************************************/

int findino(MINODE *mip, int *myino, int *parentino){//find myino and parent ino
  char *cp, buf[BLKSIZE];                            //from mip (. and ..)

  ip=mip->INODE;                      //get INODE
  get_block(dev, ip->i_block[0], buf);//move to first data block for . and ..

  dp=(DIR *)buf;                      //set myino to the inode of .
  cp=buf;
  *myino=dp->inode;

  cp+=dp->rec_len;                    //set parentino to the inode of ..
  dp=(DIR *)cp;
  *parentino=dp->inode;
}



int incFreeInodes(int dev)
{
  char buf[BLKSIZE];

  // inc free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf);

  // TODO: return the FreeInode count
  return gp->bg_free_inodes_count;
}

int decFreeInodes(int dev)
{
  char buf[BLKSIZE];
  //dec super
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, buf);
  //dec group d
  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, buf);
  // TODO: return the FreeInode count
  return gp->bg_free_inodes_count;
}


int ialloc(int dev)
{
  int i, ino;
  char buf[BLKSIZE];
  // get inode bitmap
  get_block(dev, imap, buf);
  set_bit(buf, (ino));
  put_block(dev, imap, buf);
  // update free inode count in SUPER and GD
  return decFreeInodes(dev);
}


int idealloc(int dev, int bit){
  int i = 0;
  char buf[BLKSIZE];
  //get inode bitmap
  get_block(dev, imap, buf);
  //set bit to 0 and write back
  clr_bit(buf, bit);
  put_block(dev, imap, buf);
  return incFreeInodes(dev);
}