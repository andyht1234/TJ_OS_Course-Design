uint64
cowloc(pagetable_t pagetable,uint64 va)
{
    va = PGROUNDDOWN(va);
    if(va>=MAXVA)   
    {
        return -1; 
    }
    pte_t *pte=walk(pagetable, va, 0);// 返回的是最后一级页表的PTE
    if(pte==0)  //检测PTE是否失效
    {
        return -1; 
    }
    if ((*pte & PTE_V) == 0 || (*pte & PTE_U) == 0) 
    {
        return -1; 
    }
    uint64 pa_old=PTE2PA(*pte);
    if (pa_old == 0) 
    {
        return -1;
    }
    uint64 flags=PTE_FLAGS(*pte);
    if(flags & PTE_COW) 
    {
        uint64 pa_new=(uint64)kalloc(); //分配新页面
        if (pa_new == 0) 
        {
            return -1;
        }
        memmove((void *)pa_new, (void *)pa_old, PGSIZE);      //页面复制
        uvmunmap(pagetable, va, 1, 1);
        flags=(flags|PTE_W)&~PTE_COW;   //将PTE_W复原
        if(mappages(pagetable, va, PGSIZE, pa_new, flags) != 0)
        {
            kfree((void *)pa_new);
            return -1;
        }
    }
    return 0;
}
