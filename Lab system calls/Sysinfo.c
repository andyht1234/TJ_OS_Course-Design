//freemem
uint64
freemem(void)
{
    struct run *r;
    uint64 num=0; //空闲内存数量
    acquire(&kmem.lock);
    r=kmem.freelist;
    while(r)  
    {
        num++;
        r=r->next;  //遍历单链表
    }
    release(&kmem.lock);
    return num*PGSIZE;
}

//nproc
uint64
nproc(void)
{
    struct proc *p;
    uint64 num=0;   //统计进程数
    for(p=proc;p<&proc[NPROC];p++)
    {
        acquire(&p->lock);
        if (p->state != UNUSED)
        {
            num++;
        }
        release(&p->lock);
    }
    return num;
}

//sys_sysinfo
uint64
sys_sysinfo(void)
{
    struct proc *p=myproc();
    uint64 addr;
    if(argaddr(0,&addr)<0)  //用户空间是否空闲，并获取地址
    {
        return -1;
    }
    struct sysinfo s_info;
    s_info.freemem=freemem();
    s_info.nproc=nproc();
    if(copyout(p->pagetable,addr,(char *)&s_info,sizeof(s_info))<0)     //复制回用户空间
    {
        return -1;
    }
    return 0;
}