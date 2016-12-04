char *name[64];

/******************************************************************************/

int get_block(int fd, int blk, char buf[ ]){
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

/******************************************************************************/

int put_block(int fd, int blk, char buf[ ]){
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, buf, BLKSIZE);
}

/******************************************************************************/
//tokenize pathname into name[]
int tokenize(char *pathname){
  char *token, path[128], names[64][64];
  int i = 0, j;
  strcpy(path, pathname);
  token = strtok(path, "/");
  while(token!=NULL&&i<64){
    strcpy(names[i], token);
    name[i]=names[i];
    token = strtok(NULL, "/");
    i++;
  }
  return i;
}


/******************************************************************************/

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

/******************************************************************************/

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

/******************************************************************************/

u32 getino(int *dev, char *pathname){//returns inode # of a pathname
  int n, i, ino, inostrt;
  //change dev if necessary
  if(pathname[0]=='/')
    dev = root->dev;
  else
    dev = running->cwd->dev;
  
  n=tokenize(pathname);
  for(i=0;i<n;i++){
    ino = search(running->cwd->INODE, name[i]);//search takes INODE
    if(ino==0)//if nothing found, return nothing
      return 0;
  }
  return ino;
}

/******************************************************************************/

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

/******************************************************************************/

int iput(MINODE *mip){//release inode from memory
  int ino, blk, offset;
  char *buf[BLKSIZE];
  if(--mip->refCount == 0 && mip->dirty)
  {
    //if dirty and last reference in memory
    mip->locked = 0;

    // TODO: What if the the buf to write to this inode exceeds 1KB?
    // We win
    memcpy(buf, mip->INODE, BLKSIZE);
    put_block(mip->dev, blk, buf);//write back
  }
  else if(mip->refCount > 0 || !mip->dirty)
    return 0;
}

/******************************************************************************/

int findname(MINODE *parent, int myino, char *myname){
  int n = 0, i = 0, j = 0;
  char *cp, buf[BLKSIZE];
  ip = parent->INODE;
  for(i=0;i<12;i++)
  {
    if(ip->i_block[i]==0)
    {
      return -1;
    }
    get_block(dev, ip->i_block[i], buf);
    dp=(DIR *)buf;
    cp = buf;
    while(dp->rec_len)
    {
      if(dp->inode==myino)
      {
        if(myino==2)
        {
          strcpy(myname,"/");
        }
        else
        {
          strcpy(myname, dp->name);
        }
      }
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
  printf("search found nothing with the inode %s\n", myino);
  return 0;
}

/******************************************************************************/

int findino(MINODE *mip, int *myino, int *parentino){
  char *cp, buf[BLKSIZE];
  
  ip=mip->INODE;                      //get INODE
  get_block(dev, ip->i_block[0], buf);//move to first data block for . and ..
                                    
  dp=(DIR *)buf;                      //set myino to the inode of .
  cp=buf;
  *myino=dp->inode;
  
  cp+=dp->rec_len;                    //set parentino to the inode of ..
  dp=(DIR *)cp;
  *parentino=dp->inode;
}

/******************************************************************************/

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

/******************************************************************************/

ch_dir (char *pathname){
  MINODE *mip;
  int ino;
  if(pathname[0]=='\0' ||           //if no pathname give
    (pathname[0]=='/' &&            //or if the first char is /
      strlen(pathname)==1))         //and its the only char
  {                                 //cd to root
      dev = root->dev;              //update dev
      iput(running->cwd);           
      running->cwd=root;            //set to root
  }
  else
  {                                 //cd to pathname
    ino = getino(&dev, pathname);   //gets inode # from pathname
    if(ino)                         //if ino found
    {
      iput(running->cwd);           
      running->cwd=iget(dev, ino);  
    }
    else
      printf("invalid pathname for cd\n");

  }
}

/******************************************************************************/

pwd (MINODE *mip){
  MINODE *parent;
  char mname[128], temp[128], pname[128];
  int myino=0, parentino=1, i =0;
  if (mip->ino==2)                    //if root just print / and return
  {
    printf("/\n");
    return 0;
  }
  memset(mname, '\0', 128);
  memset(temp, '\0', 128);
  memset(pname, '\0', 128);
  while(mip->ino>2)                  //loop back through each dir until root
  {                                   //and append along the way
    findino(mip, &myino, &parentino); //returns ino and parent ino (. and ..)
    parent = iget(dev, parentino);    //get the parent inode into memory
    findname(parent, myino, mname);   //returns mname, the dirname of myino in 
                                      //parent
    strcpy(temp, "/");
    strcat(temp, mname);              //temp to help append to front of string
    strcat(temp, pname);
    strcpy(pname, temp);
    
    mip=parent;                       //continue up parent
  }
  printf("%s\n", pname);              //print when done
}

/******************************************************************************/

kmkdir(MINODE *pmip, char *path)
{
  MINODE *mip;
  int ino = 0, blk = 0, i = 0;
  INODE *ip = 0;
  char buf[BLKSIZE];
  char *cp;


  // 5-1. allocate an INODE and a disk block:
  ino = ialloc(pmip->dev);
  blk = balloc(pmip->dev);
  mip = iget(pmip->dev,ino); // load INODE into an minode

  // set alias of the memory inode
  ip = &(mip->INODE);

  // KEEP IN MIND running IS A GLOBAL PROC POINTER
  ip->i_mode = DIR_MODE;		  // Leading 040 (DIR type) 755 (access permissions)
  ip->i_uid  = running->uid;	// Owner uid from PROC
  ip->i_gid  = running->gid;	// Group Id from PROC
  ip->i_size = BLKSIZE;		// Size in bytes (1024)
  ip->i_links_count = 2;	    // For . (alias of curr dir) and .. (parent dir)
  ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  // set to current time
  ip->i_blocks = 2;           // Size is 1024, each block 'chunk' is 512 bytes
                              // So we need 2 blocks (2*512 = 1024)

  ip->i_block[0] = blk;       // DIR has one direct data block (which was
                              // allocated by balloc)

  // Initialize remaining (skip first direct block) data blocks to 0
  for(i = 1;i < 15;i++)
  {
    ip->i_block[i] = 0;
  }

  // Mark our memory inode as dirty so then we can write it to the image file
  mip->dirty = 1;
  // Now write it
  iput(mip);

  //make . and ..
  //enter_child

  // Open up this data block we just allocated so we can write the actual data
  get_block(pmip->dev, blk, buf);

  // deep copy the buf. y? because fuck you that's why
  cp = buf;
  // Type cast to dir entry (because that's the type of data we want to write
  // to the buf when all is said and done)
  dp = (DIR *)cp;
  // From the newly created inode (this is the "." alias in a directory (a ref
  // to its self))
  dp->inode = ino;
  dp->name_len = strlen(".");
  // The rec_len is 12 bytes ... why?
  dp->rec_len = 4 *((11 + dp->name_len) / 4);
  dp->file_type = 0;
  strcpy(dp->name, ".");

  // Now create the parent alias (..) to the parent directory
  cp += dp->rec_len;
  dp = (DIR *)cp;
  // Parent inode #
  dp->inode = pmip->ino;
  dp->name_len = strlen("..");
  // Take up the the remaining length of the record because this is the last\
     entry to make while creating a whole new DIR
  dp->rec_len = 1012;
  dp->file_type = 0;
  strcpy(dp->name, "..");

  // Write this i_block[0] back into memory, we have created the directory
  put_block(pmip->dev, blk, buf);

  // 5-4. enter_child(pmip, ino, basename); which enters
  // (ino, basename) as a DIR entry to the parent INODE;
}

/******************************************************************************/

mk_dir(char *pathname)
{
  char temp[128], *dname, *bname;
  int pino, d=0, b=0;
  MINODE *pmip;
  strcpy(temp, pathname);
  dname = dirname(temp);
  bname = basename(pathname);
  if(pathname[0]=='/')
  {
    dev=root->dev;
    if(!search(dname, root))
      d=1;
  }
  else
  {
    dev = running->cwd->dev;
    if(!search(dname, running->cwd))
      d=1;
  }
  if(d = 0)
  {
    printf("dirname does not exist\n");
    return 0;
  }
  pino=getino(&dev, dname);
  pmip=iget(dev,dname);
  if(search(pmip, bname))
  {
    printf("basename already exists\n");
    return 0;
  }
  kmkdir(pmip, bname);
  pmip->dirty=1;//mark it dirty
  iput(pmip);//write it to image
}
// blaze it
/******************************************************************************/

enter_child(MINODE *pmip, int ino, char* basename)
{
  int i = 0, remain = 0, newblock = 0, flag = 0;
  INODE *ip, /* the alias to our parent node Frodo!*/*pipn = &pmip->INODE;
  char buf[BLKSIZE];
  char *cp;

  // Iterate through any current directories in the parent inode
  for (; i < 12 /* Only direct blocks for now */; i++)
  {
    if(pipn->i_block[i] == 0)
    {
      // Found an empty block that we can add our new dir entry that is the \
         child to this parent directory
      flag = 1;
      break;
    }

    // else iterate over the i_blocks that are occupied (by other dirs/files)
    get_block(pmip->dev, pipn->i_block[i], buf);
    // type cast the buf to a dir entry offset
    dp = (DIR *)buf;
    cp = buf;
    
    // Just like in lab 3 when we had to print dir entries, instead we just \
       iterate over the dir entries (based off of their variable size)
    while(cp + dp->rec_len < buf + BLKSIZE)
    {
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
    
    // our directory pointer is on the last dir entry of this block
    // here we need to check if we have an 'ideal' length
    remain = dp->rec_len - (4 *((11 + strlen(dp->name)) / 4));
    if(remain >= (4 *((11 + strlen(basename)) / 4)))
    {
      // trim last entry's rec_length to ideal_length
      dp->rec_len = 4 * ((11 + strlen(dp->name)) / 4);
      
      // I don't know if this segement is necessary 
      cp += dp->rec_len;
      dp = (DIR *)cp;
      
      // If we are here this means that an existing data block has enough space
      // to insert the child directory so we will
      newblock = pipn->i_block[i];
      break;
      // enter new entry as last entry with rec_len = remain
    }
  }
    
    // Short circuit evalua
    if(flag == 1)
    {
      // No current block that can hold the child directory, so allocate a new\
         block
      newblock = balloc(pip->dev);
      // index i is pointing to an empty data block so now add the data block
      // to 
      pipn->i_block[i] = newblock;
       
    }

  

}
