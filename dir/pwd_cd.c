/******************************************************************************/
/************************************pdir**************************************/
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
/*************************************pwd**************************************/
/******************************************************************************/
/* during testing
cd dir1
ls -> output is fine
pwd -> output is fine

then ls again but
ls -> output empty
pwd -> output is '/'

*/
pwd (MINODE *mip){
  MINODE *parent, *tmip;
  char mname[128], temp[128], pname[128];
  int myino=0, parentino=1, i =0;
  if (mip->ino==2)                    //if root just print / and return
  {
    printf("/\n");
    return 0;
  }

  tmip=mip;
  memset(mname, '\0', 128);
  memset(temp, '\0', 128);
  memset(pname, '\0', 128);

  while(tmip->ino>2)                  //loop back through each dir until root
  {                                   //and append along the way
    findino(tmip, &myino, &parentino); //returns ino and parent ino (. and ..)
    parent = iget(dev, parentino);    //get the parent inode into memory
    findname(parent, myino, mname);   //returns mname, the dirname of myino in
                                      //parent
    strcpy(temp, "/");
    strcat(temp, mname);              //temp to help append to front of string
    strcat(temp, pname);
    strcpy(pname, temp);

    tmip=parent;                       //continue up parent
  }

  printf("%s\n", pname);              //print when done
}


/******************************************************************************/
/***********************************ch_dir*************************************/
/******************************************************************************/

ch_dir (char *pathname){//!!!! not working 100%?
  MINODE *mip, *omip;
  int ino;
  omip = running->cwd;
  if(pathname[0]=='\0'              //if no pathname give
  || (pathname[0]=='/'              //or if the first char is /
  && strlen(pathname)==1))          //and its the only char
  {                                 //cd to root
      dev = root->dev;              //update dev
      printf("chroot\n");
                  //set to root
      mip = iget(root->dev, 2);
      running->cwd = mip;
      iput(omip);
  }
  else
  {                                 //cd to pathname
    if(pathname[0]=='/')
    {
      dev = root->dev;
      pathname=&pathname[1];
      mip = iget(dev, 2);
      running->cwd = mip;
      iput(omip);

    }
    ino = getino(&dev, pathname);   //gets inode # from pathname
    if(ino)                         //if ino found
    {
      mip = iget(dev, ino);
      running->cwd = mip;
      iput(omip);
    }
    else
      printf("invalid pathname for cd\n");

  }
}