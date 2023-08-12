#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main() 
{
    int p1[2], p2[2];
    char buf[1]={"a"};//模拟管道缓冲区
    pipe(p1);
    pipe(p2);
    int pid=fork(),pd;
    if (pid == 0) 
    {
        pd = getpid();//获取当前进程pid
        close(p1[1]); 
        close(p2[0]); 
        read(p1[0], buf, 1);
        close(p1[0]);
        fprintf(1, "%d: received ping\n", pd);
        write(p2[1], buf, 1);
        close(p2[1]);
    } 
    else 
    { 
        pd = getpid();
        close(p1[0]);
        close(p2[1]); 
        write(p1[1], buf, 1);
        close(p1[1]);
        read(p2[0], buf, 1);
        fprintf(1, "%d: received pong\n", pd);
        close(p2[0]);
    }
    exit(0);
}