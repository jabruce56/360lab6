/******************************************************************************/
/**************************************mv**************************************/
/******************************************************************************/

/******************************************************************************/
/*************************************open*************************************/
/******************************************************************************/

int myopen(char *file, int flags)
{
// 1. get file's minode:
// ino = getino(&dev, file);
// if (ino==0 && O_CREAT){
// creat(file); ino = getino(&dev, file);
// }
// mip = iget(dev, ino);
// 2. check file INODE's access permission;
// for non-special file, check for incompatible open modes;
// 3. allocate an openTable entry;
// initialize openTable entries;
// set byteOffset = 0 for R|W|RW; set to file size for APPEND mode;
// 4. Search for a FREE fd[ ] entry with the lowest index fd in PROC;
// let fd[fd]point to the openTable entry;
// 5. unlock minode;
// return fd as the file descriptor;
}

/******************************************************************************/
/************************************close*************************************/
/******************************************************************************/

int myclose(int fd)
{
// (1). check fd is a valid opened file descriptor;
// (2). if (PROC's fd[fd] != 0){
// (3). if (openTable's mode == READ/WRITE PIPE)
// return close_pipe(fd); // close pipe descriptor;
// (4). if (--refCount == 0){ // if last process using this OFT
// lock(minodeptr);
// iput(minode); // release minode
// }
// }
// (5). clear fd[fd] = 0; // clear fd[fd] to 0
// (6). return SUCCESS;
}



/******************************************************************************/
/**************************************cp**************************************/
/******************************************************************************/