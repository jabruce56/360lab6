/******************************************************************************/
/**********************************search**************************************/
/******************************************************************************/

int search(MINODE *mip, char *pathname){//returns the inode of pathname in mip
  int n = 0, i = 0, mino = 0, pino = 0;
  char *cp, sbuf[BLKSIZE];


  printf("parent path: %s\n", pathname);
  if (!strncmp(pathname, ".", 1))
  {
    return mip->ino;
  }
  // if (!strncmp(pathname, "/", 1))
  // {
  //   return 2;
  // }
  ip = &(mip->fiji);
  for(i=0;i<12;i++)
  {
    printf("i: %d\n", i);
    if(ip->i_block[i] == 0)               //zero block means nothing left to check
    {
      printf("NO MORE direct BLOCKS GO AWAY\n");
      return 0;
    }
    get_block(mip->dev, ip->i_block[i], sbuf);//check next i_block
    dp = (DIR *)sbuf;
    cp = sbuf;

    printf("NAME %s INO %d\n", dp->name, dp->inode);

    while(cp < sbuf+BLKSIZE)
    {
      printf("cancer");
      if(strncmp(dp->name, pathname, strlen(pathname)) == 0) // shouldn't make a diff
      {
        printf("SEARCH FOR %s FOUND %s AT INO %d\n", pathname, dp->name, dp->inode);
        getchar();
        return dp->inode;
      }
      cp += dp->rec_len;                //continue to next
      dp = (DIR *)cp;
    }
  }
  printf("search found nothing with the name %s\n", name);
  return 0;
}



/******************************************************************************/
/***********************************iget***************************************/
/******************************************************************************/

MINODE *iget(int dev, int ino){//takes ino and puts its INODE into memory
  //read from disk, put into minode
  char buf[BLKSIZE];
  int i, j, blk, offset;
  // like this yes?
  //
  MINODE *mip = 0;
  // ahhh, this might be it....
  //mip->INODE = (INODE *)malloc(sizeof(INODE));
  //check if inode is already in memory
  for(i=0;i<NMINODE;i++)
  {
    mip=&minode[i];
    if(mip->refCount>0 &&  mip->dev==dev && mip->ino ==ino){
      mip->refCount++;//if so inc ref count and return
      return mip;
    }
  }

  printf("mounttab[0].iblk: %d\n", mounttab[0].iblk);

  //find inode on disk w/ mailmans

  blk = (ino - 1) / 8 + mounttab[0].iblk;

  offset = (ino - 1) % 8;
  get_block(dev, blk, buf);
  ip = (INODE *)buf+offset;

  printf("blk: %d offset: %d\n", blk, offset);

  //not in memory yet
  // why 100? its same as NMINODE so no reason


  //this should be the only way MINODES are created
  for(i=0;i<NMINODE;i++){
    mip=&minode[i];
    if(mip->refCount==0)
    {
      // printf("HEERERERRERERRER\n");
      // getchar();
        //take first available minode and init
      mip->fiji = *ip;//?almost we need to do some trikery w/pointers
      mip->fiji.i_mode = ip->i_mode;
      mip->fiji.i_uid = ip->i_uid;
      mip->fiji.i_size = ip->i_size;
      mip->fiji.i_atime = ip->i_atime;
      mip->fiji.i_ctime = ip->i_ctime;
      mip->fiji.i_mtime = ip->i_mtime;
      mip->fiji.i_dtime = ip->i_dtime;
      mip->fiji.i_gid = ip->i_gid;
      mip->fiji.i_links_count = ip->i_links_count;
      mip->dev=dev;
      mip->ino=ino;
      mip->dirty=0;
      mip->locked=1;
      if(i == 0)
      {//if its the first MINODE, its root and its very special
        printf("m%d i%d\n",mip->fiji.i_size, ip->i_size);

        //mip->refCount=1;
        mip->mounted=1;//flag if something is mounted on this minode
        mip->mountptr=&mounttab[0];
        mounttab[0].mounted_inode=mip;
      }
      else
      {
        mip->refCount++;
        //mip->mounted=0;//flag if something is mounted on this minode
      }
      for(j=0;j<12;j++){//copy blocks over
        mip->fiji.i_block[j]=ip->i_block[j];
      }
      return mip;
    }
  }
}

/******************************************************************************/
/**********************************getino**************************************/
/******************************************************************************/

int getino(int *dev, char *pathname)
{//returns inode # of a pathname
  int n = 0, i = 0, ino = 0, inostrt = 0;
  MINODE *mip = 0;
  printf("check absolute on path %s\n", pathname);
  if(pathname[0] == '/')        //update dev
  {
    printf("absoult\n");
    dev = root->dev;
  }
  else
  {
    printf("relative\n");
    dev = running->cwd->dev;//input param should be int * so we can change it in here

    printf ("dev %d\n", dev);

  }

  n = tokenize(pathname);       //get tokenized pathname for searching
  printf("begin token checking for n = %d\n", n);
  for(i=0;i<n && name[i];i++){
    printf("TOKEN[%d] %s\n", i, name[i]);
    ino = search(running->cwd, name[i]);//search through all tokens
    if(ino>0)
    {
      printf("found INO %d\n", ino);
      mip = iget(dev, ino);
      mip->refCount++;
    }
    else
    {//if nothing found, return nothing
      printf("--NO INO FOUND--");
      return 0;
    }

  }
  return ino;
}

/******************************************************************************/
/************************************iput**************************************/
/******************************************************************************/

int iput(MINODE *mip){//release inode from memory
  int ino, blk, offset, j;
  char buf[BLKSIZE];
  INODE *inode_temp =0;
  printf("iput time\n");
  mip->refCount--;
  if(mip->refCount == 0 && mip->dirty)
  {
    //if dirty and last reference in memory
    mip->locked = 0;

    //TODO: What if the the buf to write to this inode exceeds 1KB?
    // We win
    // 1024 / 128 = 8
    // Prep the area to be worked on, grab it from memory
    blk = (ino - 1) / 8 + mounttab[0].iblk;

    offset = (ino - 1) % 8;

    get_block(mip->dev, blk, buf);
    inode_temp = (INODE *)buf + offset;
    //inode_temp = mip->INODE;
    inode_temp->i_mode = mip->fiji.i_mode;
    inode_temp->i_uid = mip->fiji.i_uid;
    inode_temp->i_size = mip->fiji.i_size;
    inode_temp->i_atime = mip->fiji.i_atime;
    inode_temp->i_ctime = mip->fiji.i_ctime;
    inode_temp->i_mtime = mip->fiji.i_mtime;
    inode_temp->i_dtime = mip->fiji.i_dtime;
    inode_temp->i_gid = mip->fiji.i_gid;
    inode_temp->i_links_count = mip->fiji.i_links_count;
    // grab the data i_blocks
    for(j = 0; j < 12; j++)
      inode_temp->i_block[j] = mip->fiji.i_block[j];

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

  ip=&(mip->fiji);    //right?            compiler complains it's the only err tho now       //get INODE
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