//mmap
uint64
sys_mmap(void)
{
    uint64 addr;
    int length,prot,flags,fd,offset;
    struct file *f;
    if (argaddr(0, &addr) < 0 || argint(1, &length) < 0 || argint(2, &prot) < 0 || argint(3, &flags) < 0 || argfd(4, &fd, &f) < 0 || argint(5, &offset) < 0)    //读寄存器
    return -1;
    if (!f->readable && (prot & PROT_READ))     //读写权限检测
    return -1;
    if(!f->writable && (prot & PROT_WRITE) && flags==MAP_SHARED)
    return -1;
    struct proc *p=myproc();
    length = PGROUNDUP(length);
    if(p->sz+length > MAXVA)       //长度检测
    return -1;
    for (int i = 0; i < VMA_SIZE; i++) 
    {
        struct VMA *vma = &p->vma[i];
        if (vma->valid == 0) 
        {
            vma->valid = 1;
            vma->addr = p->sz;
            p->sz += length;        //增加进程大小
            vma->length = length;
            vma->prot = prot;
            vma->flags = flags;
            vma->fd = fd;
            vma->f = f;
            filedup(f);         //增加文件的引用计数
            vma->offset = offset;
            return vma->addr;
        }
    }
    return -1;
}

//usertrap
else if(r_scause() == 13 || r_scause() == 15)
{
    uint64 va = r_stval();
    if(va>=p->sz || va<=p->trapframe->sp)   //检测va是否在堆中
    {
        p->killed=1;
    }
    else
    {
        int found=0;
        for (int i=0;i<VMA_SIZE;i++)
        {
            struct VMA *vma = &p->vma[i];
            if(vma->valid&&va >= vma->addr&&va<vma->addr+vma->length)       //找到要分配内存的vma
            {
                va = PGROUNDDOWN(va);
                uint64 pa = (uint64)kalloc();
                if (pa == 0) 
                break;
                memset((void *)pa, 0, PGSIZE);
                ilock(vma->f->ip);      //lock/unlock传递给readi的索引结点
                readi(vma->f->ip, 0, pa, va-vma->addr+vma->offset, PGSIZE);
                iunlock(vma->f->ip);
                int flags = PTE_U;      //设置权限
                if (vma->prot & PROT_READ)
                flags |= PTE_R;
                if (vma->prot & PROT_WRITE)
                flags |= PTE_W;
                if (vma->prot & PROT_EXEC)
                flags |= PTE_X;
                if(mappages(p->pagetable, va, PGSIZE, pa, flags) != 0)     //建立映射
                {
                    kfree((void*)pa);
                    break;
                }
                found=1;
                break;
            }
        }
        if(found==0)
        p->killed=1;
    }
}

//munmap
uint64
sys_munmap(void)
{
    uint64 addr;
    int length;
    if (argaddr(0, &addr) < 0 || argint(1, &length) < 0)        //读寄存器
    return -1;
    struct proc* p = myproc();
    addr = PGROUNDDOWN(addr);
    length = PGROUNDUP(length);
    for (int i=0;i<VMA_SIZE;i++)
    {
        struct VMA *vma = &p->vma[i];
        if(vma->valid&&addr >= vma->addr&&addr+length<=vma->addr+vma->length)
        {
            if(addr != vma->addr)
            return -1;
            vma->addr += length;
            vma->length -= length;
            if(vma->flags & MAP_SHARED)         //只写回程序实际修改的MAP_SHARED页面
                filewrite(vma->f, addr, length);
            uvmunmap(p->pagetable, addr, length/PGSIZE, 1);        //取消映射指定页面
            return 0;
        }
    }
    return -1;
}
