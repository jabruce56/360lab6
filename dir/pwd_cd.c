/******************************************************************************/
/************************************pdir**************************************/
/******************************************************************************/

pdir(INODE *tip){           //takes INODE prints dir entries
  struct stat nstat;
  char buf[BLKSIZE], *cp;
  int i;
  DIR *dp;

  for(i=0;i<=12;i++)        //go through all direct blocks
  {
    if(tip->i_block[i]==0)  //if block = 0 nothing left, return
      return 0;
    get_block(dev, tip->i_block[i], buf);//read the block
    dp = (DIR *)buf;
    cp = buf;
    while(cp<buf+BLKSIZE)
    {                       //while there are dir entries, print them out
      printf("%3d %6d %6s\n", dp->inode, dp->rec_len, dp->name);
      cp += dp->rec_len;    //advance to next dir entry
      dp = (DIR *)cp;
    }
  }
}

/******************************************************************************/
/*************************************pwd**************************************/
/******************************************************************************/

pwd (){
  MINODE *parent, *mip;
  char mname[128], temp[128], pname[128];
  int myino=0, parentino=1;
  mip=iget(running->cwd->dev, running->cwd->ino);
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
/***********************************ch_dir*************************************/
/******************************************************************************/

ch_dir (char *pathname){//!!!! not working 100%?
  MINODE *mip;
  int ino=0;
  printf("incd\n");
  if(pathname[0]=='\0'              //if no pathname give
  || (pathname[0]=='/'              //or if the first char is /
  && strlen(pathname)==1))          //and its the only char
  {                                 //cd to root
                                    //set to root
      //mip = iget(root->dev, 2);
      running->cwd = root;
      //iput(omip);
  }
  else
  {                                 //cd to pathname
    printf("cd pathname\n");
    if(pathname[0]=='/')
    {
      mip = root;
      //iput(omip);

    }
    else
    {
      mip=running->cwd;
    }
    ino = getino(mip->dev, pathname);   //gets inode # from pathname
    if(ino)                         //if ino found
    {
      printf("found ino %d %s\n", ino, pathname);
      mip = iget(mip->dev, ino);
      printf("mip %d\n", mip->ino);
      //iput(omip);
    }
    else
    {
      printf("invalid pathname for cd\n");
    }
    printf("set running\n");
    running->cwd = mip;
  }
}