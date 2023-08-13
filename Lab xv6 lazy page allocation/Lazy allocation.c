else if(r_scause()==13||r_scause()==15)  //13 is read,15 is write
{
    uint64 va=PGROUNDDOWN(r_stval());   //从一页开始
    uint64 pa=(uint64)kalloc();     //分配一页物理桢
    printf("page fault:%p\n", r_stval());  //输出pagefault
    if(pa==0)
    {
        p->killed=1;
    }
    memset((void*)pa,0,PGSIZE);       //PGSIZE=4096
    if(mappages(p->pagetable,va,PGSIZE,pa,PTE_W|PTE_R|PTE_U)!=0)    //PTE是否被占用
    {
        p->killed=1;
    }
}