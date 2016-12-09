/******************************************************************************/
/**********************************tokenize************************************/
/******************************************************************************/

int tokenize(char *pathname){//tokenize pathname into name[]
  char *token, path[128];
  int i = 0, j;
  strcpy(path, pathname);
  printf("tokenize path: %s\n", path);
  // prematurely increment to see what happens for now

  if(path[0] == '/')
  // {
  //   // tokenize on the firse '/' will be missed because there is no text
  //   // BEFORE it, that's what strtok does, it searches the string until the
  //   // delimeter is found
  //   strcpy(name[i], "/");
  //   i++;
  // }
  {
    i++;
    name[0] = (char *)malloc(sizeof(char)*(2));
    strcpy(name[0], "/");
  }
  printf("begin token\n");
  token = strtok(path, "/");//strtok to parse
  while(token)
  {
    name[i] = (char *)malloc(sizeof(char)*(strlen(token)+1));

    strcpy(name[i], token);//copy token into globals.h var name
    i++;
    token = strtok(NULL, "/");

  }
  for(j=0;j<i;j++)
  {
    printf("***IN NAME[%d]: %s***\n", j, name[j]);
  }

  return i;                 //return number of items for use
}

/******************************************************************************/
/**********************************findname************************************/
/******************************************************************************/

int findname(MINODE *parent, int myino, char *myname){//find name in parent
  int n = 0, i = 0, j = 0;                            //from myino
  char *cp, buf[BLKSIZE];
  ip = &(parent->fiji);                     //search through parent dir for myino
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
