#include "global.c"
int tst_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;  j = bit % 8;
  return (buf[i] & (1 << j));
}
int ialloc(int dev){
  int i, nino;
  char buf[BLKSIZE];
  for(i=0;i<NMOUNT;i++){
    if(dev==mntTable[i].dev){
      get_block(dev, mntTable[i].imap, buf);
      for(nino=0;nino<100;nino++){
        if(!tst_bit(buf, nino)){
          mntTable[i].imap|=1<<nino;
          mntTable[i].ninodes++;
          ninodes++;
          return nino;
        }
      }
    }
  }
  return -1;
}
// int idealloc(int dev, int ino){
//
// }
// int balloc(int dev){
//
// }
// int bdealloc(int dev, int bno){
//
// }
