char name[64], buf[BLKSIZE];
int fd;

int get_block(int fd, int blk, char buf[ ]){
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}
int put_block(int fd, int blk, char buf[ ]){
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, buf, BLKSIZE);
}
int tokenize(char *pathname){
  char *token, path[128], names[64][64];
  int i = 0, j;
  strcpy(path, pathname);
  token = strtok(path, "/");
  while(token!=NULL&&i<64){
    for(j=0;j<strlen(token);j++){names[i][j]=token[j];}
    name[i]=names[i];
    token = strtok(NULL, "/");
    i++;
  }
  return i;
}
              //http://man7.org/linux/man-pages/man3/basename.3.html
              // path       dirname   basename
              // /usr/lib   /usr      lib
              // /usr/      /         usr
              // usr        .         usr
              // /          /         /
              // .          .         .
              // ..         .         ..


u32 search(INODE *inodePtr, char *name){
  int n = 0, i = 0, j = 0;
  char *str, *cp;
  get_block(fd, inodePtr->i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  printf("\nsearching for %s...\n", name);
  for(i=0;i<12;i++){
      for(j=0;j<strlen(name);j++)
        if(dp->name[j]==name[j]){
          if(j==strlen(name)-1){
            printf("found %s at inumber %u\n", name, (u32)dp->inode);
            return dp->inode;
          }
        }
        else{
            break;
        }
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  printf("search found nothing with the name %s\n", name);
  return 0;
}
u32 getino(int *dev, char *pathname){//returns inode # of a pathname.
  int n;
  n=tokenize(pathname);
  printf("%s\n", pathname);
}
MINODE *iget(int dev, u32 ino){

}
int iput(MINODE *mip){

}
