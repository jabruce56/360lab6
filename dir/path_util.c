/******************************************************************************/
/**********************************tokenize************************************/
/******************************************************************************/

int tokenize(char *pathname){//tokenize pathname into name[]
  char *token, path[128], names[64][64];
  int i = 0, j;
  strcpy(path, pathname);
  token = strtok(path, "/");//strtok to parse
  while(token!=NULL && i<64)//dont accept pathname of longer than 64 tokens
  {
    strcpy(names[i], token);//copy token into global name[i] for later use
    name[i]=names[i];
    token = strtok(NULL, "/");
    i++;
  }
  return i;                 //return number of items for use
}




/******************************************************************************/
/**********************************search**************************************/
/******************************************************************************/

u32 search(MINODE *mip, char *pathname){//returns the inode of pathname in mip
  int n = 0, i = 0, j = 0;
  char *cp, sbuf[BLKSIZE];
  ip = &(mip->INODE);
  for(i=0;i<12;i++){
    if(ip->i_block[i]==0)               //zero block means nothing left to check
      return 0;
    get_block(dev, ip->i_block[i], sbuf);//check next i_block
    dp=(DIR *)sbuf;
    cp = sbuf;
    while(dp->rec_len)
    {
      for(j=0;j<strlen(pathname);j++)   //check each char in pathname with
      {                                 //dp->name
        if(dp->name[j]==pathname[j])
        {
          if(j==strlen(pathname)-1)
          {
            return dp->inode;           //if the pathname matches return ino
          }
        }
        else{
          break;                        //if any char does not match, break
        }
      }
      cp += dp->rec_len;                //continue to next
      dp = (DIR *)cp;
    }
  }
  printf("search found nothing with the name %s\n", name);
  return 0;
}


/******************************************************************************/
/**********************************findname************************************/
/******************************************************************************/

int findname(MINODE *parent, int myino, char *myname){//find name in parent
  int n = 0, i = 0, j = 0;                            //from myino
  char *cp, buf[BLKSIZE];
  ip = parent->INODE;                     //search through parent dir for myino
  for(i=0;i<12;i++)
  {
    if(ip->i_block[i]==0)                 //zero i_block means nothing left
    {
      return 0;
    }
    get_block(dev, ip->i_block[i], buf);  //get next i_block
    dp=(DIR *)buf;
    cp = buf;
    while(dp->rec_len)
    {
      if(dp->inode==myino)                //if myino matches any
      {
        // if(myino==2)                   //!!!!probably useless
        // {
        //   strcpy(myname,"/");
        // }
        // else
        // {
        strcpy(myname, dp->name);         //copy its name
        // }
      }
      cp += dp->rec_len;                  //else continue
      dp = (DIR *)cp;
    }
  }

  //ruh roh nothing found
  printf("search found nothing with the inode %s\n", myino);
  return 0;
}
