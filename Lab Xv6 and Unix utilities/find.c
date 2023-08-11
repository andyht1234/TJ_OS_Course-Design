#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
char* filename(char *path)  //参照ls.c中的fmtname，找文件名
{
    char *p;
    for(p=path+strlen(path);p>=path&&*p!='/';p--)
      ;
    p++;
    return p;
}

void find(char *path,char *target)  //根据ls函数修改
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type)
    {
        case T_FILE:
          if (strcmp(filename(path), target) == 0)  //比较文件名
          {
              printf("%s\n", path);
          }
          break;

        case T_DIR:
          if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
          {
              printf("find: path too long\n");
              break;
          }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if(de.inum == 0)
            continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)  //跳过.和..
            { 
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            find(buf,target);  //递归查找
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3) 
    {
        fprintf(2, "error(need 3 args)\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}