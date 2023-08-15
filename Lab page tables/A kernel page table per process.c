//k_kvminit
pagetable_t
k_kvminit()
{
  pagetable_t k_pagetable = (pagetable_t) kalloc();
  memset(k_pagetable, 0, PGSIZE);
  k_kvmmap(k_pagetable, UART0, UART0, PGSIZE, PTE_R | PTE_W);
  k_kvmmap(k_pagetable, VIRTIO0, VIRTIO0, PGSIZE, PTE_R | PTE_W);
  k_kvmmap(k_pagetable, CLINT, CLINT, 0x10000, PTE_R | PTE_W);
  k_kvmmap(k_pagetable, PLIC, PLIC, 0x400000, PTE_R | PTE_W);
  k_kvmmap(k_pagetable, KERNBASE, KERNBASE, (uint64)etext-KERNBASE, PTE_R | PTE_X);
  k_kvmmap(k_pagetable, (uint64)etext, (uint64)etext, PHYSTOP-(uint64)etext, PTE_R | PTE_W);
  k_kvmmap(k_pagetable, TRAMPOLINE, (uint64)trampoline, PGSIZE, PTE_R | PTE_X);
  return k_pagetable;
}

//k_kvmmap
void
k_kvmmap(pagetable_t k_pagetable, uint64 va, uint64 pa, uint64 sz, int perm)
{
  if(mappages(k_pagetable, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

//k_freewalk
void
k_freewalk(pagetable_t pagetable)
{
  for(int i = 0; i < 512; i++)
  {
    pte_t pte = pagetable[i];
    if(pte & PTE_V)     //pte有效
    {
        pagetable[i]=0;
        if((pte & (PTE_R|PTE_W|PTE_X)) == 0)    //不是叶子结点
        {
            uint64 child = PTE2PA(pte);
            k_freewalk((pagetable_t)child);     //递归释放
        }
    } 
  }
  kfree((void*)pagetable);
}
