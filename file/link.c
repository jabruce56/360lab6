/******************************************************************************/
/*************************************link*************************************/
/******************************************************************************/

link(char *old_file, char *new_file)
{
  int oino, nino, pino, odev, ndev, pdev;
  MINODE *omip, *nmip, *pmip;
  char *ndirname, *nbasename;
  // 1. // verify old_file exists and is not DIR;
  strcpy(ndirname, new_file);
  strcpy(nbasename, new_file);
  ndirname=dirname(ndirname);
  nbasename=basename(nbasename);
  oino = getino(&odev, old_file);
  omip = iget(odev, oino);
  if(omip->fiji.i_mode==DIR_MODE) // check file type (cannot be DIR)
  {
    printf("error: attempting to link dir\n");
    return 0;
  }
  nino=getino(&ndev, new_file); //check that new_file does not exist
  if(!nino)
  {
    printf("error: file already exists\n");
    return 0;
  }
  if(ndev!=odev)// ndev of dirname(newfile) must be same as odev
  {
    printf("error: devices do not match\n");
  }
  pino=getino(&pdev, ndirname);
  //mcreat(ndirname); // 3. // creat entry in new_parent DIR with same ino
  pmip = iget(&pdev, pino);
  //enter_name(pmip, omip->ino, basename(new_file));
  omip->fiji.i_links_count++;
  omip->dirty = 1;
  iput(omip);
  iput(pmip);
}

/******************************************************************************/
/************************************unlink************************************/
/******************************************************************************/

unlink(char *filename)
{
  int ino;
  MINODE *mip;
  char *dname, *bname;
  // 1. get filenmae's minode:
  ino=getino(&dev, filename);
  mip = iget(dev, ino);
  // if(mip->INODE->i_mode==SLINK_MODE || mip->INODE->i_mode==FILE_MODE)// check it's a REG or SLINK file
  // {
  //   printf("probably an error to unlink a file or slink");
  //   return 0;
  // }

  // 2. // remove basename from parent DIR
  // rm_child(pmip, mip->ino, basename);
  // pmip->dirty = 1;
  // iput(pmip);
  // 3. // decrement INODE's link_count
  // mip->INODE.i_links_count--;
  // if (mip->INODE.i_links_count > 0){
  // mip->dirty = 1; iput(mip);
  // }
  // 4. if (!SLINK file) // assume:SLINK file has no data block
  // truncate(mip); // deallocate all data blocks
  // deallocate INODE;
  // iput(mip);
}

/******************************************************************************/
/***********************************symlink************************************/
/******************************************************************************/

symlink(old_file, new_file)
{
  // 1. check: old_file must exist and new_file not yet exist;
  // 2. create new_file; change new_file to SLINK type;
  // 3. // assume length of old_file name <= 60 chars
  // store old_file name in newfile's INODE.i_block[ ] area.
  // mark new_file's minode dirty;
  // iput(new_file's minode);
  // 4. mark new_file parent minode dirty;
  // put(new_file's parent minode);
}
