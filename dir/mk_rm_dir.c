/******************************************************************************/
/***********************************kmkdir*************************************/
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
  //ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  // set to current time
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

  // deep copy the buf.
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
  printf("test\n");
  // 5-4. enter_child(pmip, ino, basename); which enters
  // (ino, basename) as a DIR entry to the parent INODE;
}

/******************************************************************************/
/************************************mk_dir************************************/
/******************************************************************************/

mk_dir(char *pathname)
{
  char temp[128], *dname, *bname;
  int pino, d=0, b=0;
  MINODE *pmip;

  strcpy(temp, pathname);                   //save pathname for basename
  dname = dirname(temp);                    //dirname is the parent dir name
  bname = basename(pathname);               //basename is the new dir name

  printf("%s %s\n", dname, bname);

  if(pathname[0]=='/')                      //if starting at root
  {
    dev=root->dev;                          //update dev
   // if(!search(dname, root))                //search root for dirname
    //{
      //printf("dirname does not exist\n");   //return if not found
      //return 0;
    //}
  }
  else                                      //else its somewhere else
  {
    dev = running->cwd->dev;
    //if(!search(dname, running->cwd))        //search for dirname in cwd
    //{
      //printf("dirname does not exist\n");   //return if not found
      //return 0;
  //  }
  }

  pino=getino(&dev, dname);                 //get parent ino
  pmip=iget(dev,dname);                     //get parent into memory

  //if(search(pmip, bname))                   //check that basename does not exist
  //{
    //printf("basename already exists\n");    //return if bname found
    //return 0;
//  }

  kmkdir(pmip, bname);                      //handles init of new dir
  pmip->dirty=1;                            //mark it dirty
  iput(pmip);                               //write it to image
}

/******************************************************************************/
/*********************************enter_child**********************************/
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

    // Short circuit evaluation
    if(flag == 1)
    {
      // No current block that can hold the child directory, so allocate a new\
         block
      newblock = balloc(pmip->dev);
      // index i is pointing to an empty data block so now add the newly \
         allocated data block to the parent's i_block
      pipn->i_block[i] = newblock;
      // standard 1KB to the current size of the parent minode
      pipn->i_size += BLKSIZE;
      // get this new EMPTY block so now we can insert the dir entry
      get_block(pmip->dev, newblock, buf);

      cp = buf;
      dp = (DIR *)cp;
    }

    // dp and cp point to the location where new dir should be placed
    dp->inode = ino;
    dp->name_len = strlen(basename);
    dp->rec_len = (int)(buf + BLKSIZE) - (int)(dp);
    dp->file_type = 0;
    strcpy(dp->name, basename);
    put_block(pmip->dev, newblock, buf);
}

/******************************************************************************/
/*************************************rmdir************************************/
/******************************************************************************/

int rmdir(char *path)
{
  char parent[64], child[64], temp_name[64];
  int pino, delino;
  int dev, i;
  MINODE *pip, *searchmino, *deli;

  if(!path)
  {
    return;
  }

  // Assume path is relative unless the if statement below is proc'd
  dev = running->cwd->dev;

  if(path[0] == '/')
    dev = root->dev;

  strcpy(temp_name, path);
  strcpy(parent, dirname(temp_name));
  //strcpy(temp_name, pathname);



}
