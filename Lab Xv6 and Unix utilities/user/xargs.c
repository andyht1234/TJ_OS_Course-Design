#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
int main(int argc, char *argv[])
{
    char *new_argv[MAXARG];
    char buf[512];
    int len=0,arg_idx=0;
    for (int i = 1; i < argc; i++)      //去除xargs
        new_argv[arg_idx++] = argv[i];
    while((len=read(0,buf,sizeof(buf)))>0)  //读取前命令
    {
        if(fork()==0)  //开启新进程
        {
            char* arg = (char *)malloc(sizeof(buf));   //储存每一行的前命令
            int idx = 0;
             for(int i=0;i<len;i++)
             {
                 if(buf[i]==' '||buf[i]=='\n')  //遇到空格和换行即跳过
                 {
                     arg[idx]=0;
                     idx = 0;
                     new_argv[arg_idx++]=arg;
                     free(arg);     //一行读完就释放空间
                     arg = (char *)malloc(sizeof(buf));
                 }
                 else
                 {
                     arg[idx++]=buf[i];  //遇到正确命令及添加
                 }

             }
             new_argv[arg_idx] = 0;
             exec(argv[1],new_argv);   //切换为执行新命令
        }
        else
        {
            wait(0);    //等待全部进程完成
        }
    }
    exit(0);
}