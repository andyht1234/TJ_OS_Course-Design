//symlink
uint64
symlink(void)
{
    char path[MAXPATH], target[MAXPATH];
    struct inode *ip;
    if(argstr(0, target, MAXPATH)<0||argstr(1, path, MAXPATH)<0)        //读取参数
    return -1;
    begin_op();
    if((ip = namei(path))!= 0)      //软连接是否已存在
    {
        end_op();
        return -1;
    }   
    if((ip = create(path, T_SYMLINK, 0, 0))==0)       //创建inode
    {
        end_op();
        return -1;
    }
    if(writei(ip, 0, (uint64)target, 0, MAXPATH) < MAXPATH)         //在inode中存储符号链接目标路径
    {
        iunlockput(ip);
        end_op();
        return -1;
    }
    iunlockput(ip);
    end_op();
    return 0;
}

//sys_open
int depth=0;
while (ip->type == T_SYMLINK && !(omode & O_NOFOLLOW)) 
{
    if(depth >= 10)  //默认死循环
    {
        iunlockput(ip);
        end_op();
        return -1;
    }
    if(readi(ip, 0, (uint64)path, 0, MAXPATH)<MAXPATH)  //读出目标路径
    {
        iunlockput(ip);
        end_op();
        return -1;
    }
    iunlockput(ip);
    if((ip = namei(path)) == 0)     //根据名称找到inode，检查软连接是否存在
    {   
        end_op();
        return -1;
    }
    ilock(ip);
    depth++;
}