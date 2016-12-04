#include "type.h"
#include "include.h"
int nproc=0;
int init(){ // Initialize data structures of LEVEL-1:
     int i, j;
     PROC *p;
     
     //reset global proc, minode, mounttab, and oft
     for(i=0;i<NPROC;i++){
       p=&proc[i];
       p->status=FREE;
       for(j=0;j<NFD;j++)
         p->fd[j]=0;
       p->next=&proc[i+1];
     }
     for(i=0;i<NMINODE;i++){minode[i].refCount=0;}
     for(i=0;i<NMOUNT;i++){mounttab[i].busy=0;}
     for(i=0;i<NOFT;i++){oft[i].refCount=0;}
     
     printf("mounting root..\n");
     mount_root();
     printf("mount complete\n");

     srand(time(NULL));
     if(rand()%5==0)
     {
      printf("starting 'creationism' P0\n");
     }
     else
     {
      printf("starting P0\n");
     }
     p = &proc[0];
     running=&proc[0];
     p->status = RUNNING;
     p->next = &proc[1];
     p->uid = 0;
     p->pid = 0;
     p->ppid = 0;
     p->gid = 0;
     p->parent = p;
     p->sibling = p;
     //p->next?
     p->child = 0;
     p->cwd = root;
     p->cwd->refCount++;

     printf("starting P1\n");
     p = &proc[1];
     p->next = &proc[0];
     p->status = RUNNING;
     p->uid = 2;
     p->pid = 1;
     p->ppid = 0;
     p->gid = 0;
     //these ones too??
    // p->parent = p;
    // p->sibling = p;
    // //p->next?
    // p->child = 0;
     p->cwd = root;
     p->cwd->refCount++;
     nproc=2;
}


int mount_root(){  // mount root file system, establish / and CWDs
  int i, ino, fd;
  SUPER *sp;
  MOUNT *mp;
  MINODE *ip;
  char buf[BLKSIZE], *rootdev;

//  open device for RW (get a file descriptor dev for the opened device)
  dev = open("disk", O_RDWR);
  if(dev<0){
    printf("open disk failed\n");
    exit(1);
  }

//  read SUPER block to verify it's an EXT2 FS and save info
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  if(sp->s_magic!=SUPER_MAGIC){
    printf("not a valid ext2 file system\n");
    exit(0);
  }
  printf("valid ext2 fs\n");
  mp = &mounttab[0];
  nblocks = sp->s_blocks_count;
  ninodes = sp->s_inodes_count;
  mp->ninodes=ninodes;
  mp->nblocks=nblocks;

  //save group desc info
  get_block(dev, 2, buf);
  gp = (GD *)buf;
  mp->dev=dev;
  mp->bmap=gp->bg_block_bitmap;
  mp->imap=gp->bg_inode_bitmap;
  mp->iblk=gp->bg_inode_table;


  strcpy(mp->name, "disk");
  strcpy(mp->mount_name, "/");
  printf("bmap=%d  ",   gp->bg_block_bitmap);
  printf("imap=%d  ",   gp->bg_inode_bitmap);
  printf("iblock=%d\n", gp->bg_inode_table);
  printf("dev=%d\n", dev);
  printf("iget root\n");
  root=iget(dev, 2);
  mp->mounted_inode=root;
  root->mountptr = mp;
  proc[0].cwd=iget(dev,2);
  proc[1].cwd=iget(dev,2);
  printf("root mounted\n");
}




int main()
  {
    char line[256], cname[128], path[128];
    int cmd;
    
    
    init();
    printf("dev=%d\n",dev);
    printf("init complete\n");
    while(1){
      memset(line, '\0', 256);
      memset(cname, '\0', 128);
      memset(path, '\0', 128);
      printf("command: ");
      fgets(line, 256, stdin);
      sscanf(line, "%s %s", cname, path);
      if(!strncmp(cname, "mkdir", 5))
        mk_dir(path);
      else if(!strncmp(cname, "cd", 2))
        ch_dir(path);
      else if(!strncmp(cname, "pwd", 3))
        pwd(running->cwd);
      else if(!strncmp(cname, "ls", 2))
        listdir(path);
        // else if(!strncmp(cname, "mount", 5))
        //   return 4;
        // else if(!strncmp(cname, "umount", 6))
        //   return 5;
        // else if(!strncmp(cname, "creat", 5))
        //   return 6;
        // else if(!strncmp(cname, "rmdir", 5))
        //   return 7;
        // else if(!strncmp(cname, "rm", 2))
        //   return 8;
        // else if(!strncmp(cname, "open", 4))
        //   return 9;
        // else if(!strncmp(cname, "close", 5))
        //   return 10;
        // else if(!strncmp(cname, "read", 4))
        //   return 11;
        // else if(!strncmp(cname, "write", 5))
        //   return 12;
        // else if(!strncmp(cname, "cat", 3))
        //   return 13;
        // else if(!strncmp(cname, "cp", 2))
        //   return 14;
        // else if(!strncmp(cname, "mv", 2))
        //   return 15;
        // else if(!strncmp(cname, "pfd", 3))
        //   return 16;
        // else if(!strncmp(cname, "lseek", 5))
        //   return 17;
        // else if(!strncmp(cname, "rewind", 6))
        //   return 18;
        // else if(!strncmp(cname, "stat", 4))
        //   return 19;
        // else if(!strncmp(cname, "pm", 2))
        //   return 20;
        // else if(!strncmp(cname, "menu", 4))
        //   return 21;
        // else if(!strncmp(cname, "access", 6))
        //   return 22;
        // else if(!strncmp(cname, "chmod", 5))
        //   return 23;
        // else if(!strncmp(cname, "chown", 5))
        //   return 24;
        // else if(!strncmp(cname, "cs", 2))
        //   return 25;
        // else if(!strncmp(cname, "fork", 4))
        //   return 26;
        // else if(!strncmp(cname, "ps", 2))
        //   return 27;
        // else if(!strncmp(cname, "kill", 4))
        //   return 28;
        else if(!strncmp(cname, "quit", 4))
          quit();
        // else if(!strncmp(cname, "touch", 5))
        //   return 30;
        // else if(!strncmp(cname, "sync", 4))
        //   return 31;
        // else if(!strncmp(cname, "link", 4))
        //   return 32;
        // else if(!strncmp(cname, "unlink", 6))
        //   return 33;
        // else if(!strncmp(cname, "symlink", 7))
        //   return 34;
        else
          printf("error: unknown command");
      }
    }



int quit()
{
//      iput all DIRTY minodes before shutdown
  int i;
  srand(time(NULL));
  if(rand()%5==0)
    printf("cleaning up all those dirty inodes ;)...\n");
  else
    printf("closing file system...\n");
  usleep(650);
  for(i=0;i<NMINODE;i++)
    if(minode[i].dirty)
      iput(&minode[i]);
  printf("complete\n");
  exit(0);
}
