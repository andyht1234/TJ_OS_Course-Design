#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
void prime_chosen(int p[])  //埃氏筛
{
    int prime1,prime2;
    int pp[2];
    close(p[1]);
    if(read(p[0],&prime1,sizeof(prime1)))
    {
        fprintf(1, "prime %d\n", prime1);   //左侧进的为质数（不是的都筛掉了）
        pipe(pp);   //建立新管道，筛下一个数
        if(fork()==0)
        {
            prime_chosen(pp);   //子进程递归到下一个管道
        }
        else
        {
            close(pp[0]);
            while(read(p[0],&prime2,sizeof(prime2)))
            {
                if(prime2%prime1!=0)    //父进程在本管道筛掉本管道质数的倍数
                    write(pp[1],&prime2,sizeof(prime2));  //剩下的写入新数组
            }
            close(pp[1]);
            close(p[0]);
            wait(0);
        }
    }
    exit(0);
}

int main()
{
    int p[2];
    pipe(p);
    if(fork()==0) //子进程负责筛
    {
        prime_chosen(p);
    }
    else  //父进程负责写
    { 
        close(p[0]);
        for(int i=2;i<=35;i++)
        {
            write(p[1],&i,sizeof(i));
        }
        close(p[1]);
        wait(0);//写完等待子进程
    }
    exit(0);
}