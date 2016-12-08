
/******************************************************************************/
/************************************kcreat************************************/
/******************************************************************************/

kcreat(MINODE *pmip, char *path)
{
  MINODE *mip;
  int ino = 0, blk = 0, i = 0;
  INODE *ip = 0;
  char buf[BLKSIZE];
  char *cp;
  ino = ialloc(pmip->dev);
  mip = iget(pmip->dev,ino);

}

/******************************************************************************/
/************************************mycreat***********************************/
/******************************************************************************/

mycreat(char *pathname)
{
  //same as mkdir but INODE.i_mode is file type and permissions set to 0644
  //no data block is allocated, file size starts as 0
  //do not increment parent INODES links_count
  char temp[128], *dname, *bname;
  int pino, d=0, b=0;
  MINODE *pmip;

  strcpy(temp, pathname);                   //save pathname for basename
  dname = dirname(temp);                    //dirname is the parent dir name
  bname = basename(pathname);               //basename is the new dir name
}