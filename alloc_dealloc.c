//#include "global.c"
int tst_bit(char *buf, int bit)
{
  int i, j;

  i = bit / 8;
  j = bit % 8;
  if (buf[i] & (1 << j)){
    return 1;
  }
  return 0;
}

int clr_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] &= ~(1 << j);
  return 0;
}

int set_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] |= (1 << j);
  return 0;
}

int incFreeInodes(int dev)
{
  char buf[BLKSIZE];

  // inc free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf);
}

int decFreeInodes(int dev)
{
  char buf[BLKSIZE];
  // YOU DO IT
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, buf);
}

int incFreeBlocks(int dev)
{
  char buf[BLKSIZE];

  // inc free block count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count++;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, buf);
}

int decFreeBlocks(int dev)
{
  char buf[BLKSIZE];
  // YOU DO IT
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count--;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count--;
  put_block(dev, 2, buf);
}


u32 ialloc(int dev)
{
  int i, ino;
  char buf[BLKSIZE];

  // get inode Bitmap into buf
  get_block(dev, imap, buf);
  printf("imap:\n");
  for (i=0; i < ninodes;i++){
    printf("%d", tst_bit(buf, i));
    if(0==i%10){
      printf("\n");
    }
    if(!tst_bit(buf, i)){
      ino = i+1;
    }
    // printf("inumber %d out of range\n", ino);
    // return;
  }
  printf("\nino = %d\n", ino);

  // get inode bitmap block
  //get_block(dev, imap, buf);
  set_bit(buf, ino-1);

  // write buf back
  put_block(dev, imap, buf);
  printf("bmap b4 balloc:\n");
  for (i=0; i < ninodes;i++){
    printf("%d", tst_bit(buf, i));
    if(0==i%10){
      printf("\n");
    }
  }
  // update free inode count in SUPER and GD
  decFreeInodes(dev);
}

u32 balloc(int dev)
{
  int i, ino;
  char buf[BLKSIZE];

  // get inode Bitmap into buf
  get_block(dev, bmap, buf);
  printf("bmap b4 balloc:\n");
  for (i=0; i < ninodes;i++){
    printf("%d", tst_bit(buf, i));
    if(0==i%10){
      printf("\n");
    }
    if(!tst_bit(buf, i)){
      ino=i+1;
    }
    //  printf("inumber %d out of range\n", ino);
    //  return;
   }

   // get inode bitmap block
   get_block(dev, bmap, buf);
   set_bit(buf, ino-1);

   // write buf back
   put_block(dev, bmap, buf);
   printf("bmap after balloc:\n");
   for (i=0; i < ninodes;i++){
     printf("%d", tst_bit(buf, i));
     if(0==i%10){
       printf("\n");
     }
   }
   // update free inode count in SUPER and GD
   decFreeBlocks(dev);
  // YOU DO IT
}
int idealloc(int dev, int bit){
  int i;
  char buf[BLKSIZE];
  get_block(dev, imap, buf);
  printf("idealloc bit %d\n", bit);
  printf("imap b4 idealloc:\n");
  for (i=0; i < ninodes;i++){
    printf("%d", tst_bit(buf, i));
    if(0==i%10){
      printf("\n");
    }
  }
  clr_bit(buf, bit-1);
  put_block(dev, imap, buf);

  printf("imap after idealloc:\n");
  for (i=0; i < ninodes;i++){
    printf("%d", tst_bit(buf, i));
    if(0==i%10){
      printf("\n");
    }
  }
  incFreeInodes(dev);
}

int bdealloc(int dev, int bit)
{
  int i;
  char buf[BLKSIZE];
  get_block(dev, bmap, buf);
  printf("bdealloc bit %d\n", bit);
  printf("bmap b4 bdealloc:\n");
  for (i=0; i < ninodes;i++){
    printf("%d", tst_bit(buf, i));
    if(0==i%10){
      printf("\n");
    }
  }
  clr_bit(buf, bit-1);
  put_block(dev, bmap, buf);

  printf("bmap after bdealloc:\n");
  for (i=0; i < ninodes;i++){
    printf("%d", tst_bit(buf, i));
    if(0==i%10){
      printf("\n");
    }
  }
  incFreeBlocks(dev);
}

// int balloc(int dev){
//
// }
// int bdealloc(int dev, int bno){
//
// }
