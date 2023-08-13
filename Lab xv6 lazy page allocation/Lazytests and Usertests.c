uint64 lazyalloction(struct proc * p, uint64 va)
{
    if(va >= p->sz || va < PGROUNDUP(p->trapframe->sp))
    {
        return 0;
    }
    uint64 a=PGROUNDDOWN(va);   //从一页开始
    uint64 pa=(uint64)kalloc();     //分配一页物理桢
    if(pa==0)
    {
        return 0;
    }
    memset((void*)pa,0,PGSIZE);       //PGSIZE=4096
    if(mappages(p->pagetable,a,PGSIZE,pa,PTE_W|PTE_R|PTE_U)!=0)    //PTE是否被占用
    {
        kfree((void*)pa);
        return 0;
    }
    return pa;
}