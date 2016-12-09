/******************************************************************************/
/**********************************findpdir************************************/
/******************************************************************************/

findpdir (char *pathname){//takes pathname, finds its MINODE and then calls
                          //pdir on MINODES->INODE
  int i, ino = 0, dev = running->cwd->dev;
  MINODE *mip = running->cwd;//set mip to cwd in case there is no pathname

  if (pathname[0]!='\0'){   // if a pathname was specified, change to its MINODE
    pathname[strlen(pathname)]='\0';//idk if this is necessary or na
    ino = getino(&dev, pathname);
    if(ino)//if the MINODE exists get it, otherwise print cwd dir
      mip = iget(dev, ino);
  }
  pdir(&(mip->fiji));
 // if(ino)//if iget was called, iput
   // iput(mip);
  // mip points at minode;
  // Each data block of mip->INODE contains entries
  // print the name strings of the entries
}

