/******************************************************************************/
/**********************************get_block***********************************/
/******************************************************************************/

int get_block(int fd, int blk, char buf[ ]){
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

/******************************************************************************/
/**********************************put_block***********************************/
/******************************************************************************/

int put_block(int fd, int blk, char buf[ ]){
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, buf, BLKSIZE);
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
  // TODO: return the FreeBlock count
  return gp->bg_free_blocks_count;
}

int decFreeBlocks(int dev)
{
  char buf[BLKSIZE];
  //dec super
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count--;
  put_block(dev, 1, buf);
  //dec group desc
  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count--;
  put_block(dev, 2, buf);
  // TODO: return the FreeBlock count
  return gp->bg_free_blocks_count;
}


int balloc(int dev)
{
  int i = 0;
  char buf[BLKSIZE];
  // get inode Bitmap into buf
  get_block(dev, bmap, buf);
  for(;i < BLKSIZE; i++)
  {
    if(tst_bit(buf, i) == 0)
    {
      set_bit(buf, i);
      // update free inode count in SUPER and GD
      decFreeBlocks(dev);
      put_block(dev, bmap, buf);
      // We now have one more block on the map
      return i+1;
    }
  }

  // what if we have no unoccupied blocks?
  printf("oh shit waddup no more blocks\nPANIC\n");
  return -1;

}

int bdealloc(int dev, int bit)
{
  int i;
  char buf[BLKSIZE];
  //get block bitmap
  get_block(dev, bmap, buf);
  //set bit to 0 and write back
  clr_bit(buf, bit);
  put_block(dev, bmap, buf);
  return incFreeBlocks(dev);
}