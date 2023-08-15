int
k_uvmcopy(pagetable_t old, pagetable_t new, uint64 oldsz,uint64 newsz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  for(i = PGROUNDUP(oldsz); i < newsz; i += PGSIZE)
  {
    if((pte = walk(old, i, 0)) == 0)
      panic("uvmcopy: pte should exist");
    if((*pte & PTE_V) == 0)
      panic("uvmcopy: page not present");
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte)&(~PTE_U);       //将PTE_U标记位置0，内核才能访问用户态页表
    if(mappages(new, i, PGSIZE, pa, flags) != 0)
    {
      goto err;
    }
  }
  return 0;

 err:
  uvmunmap(new, oldsz, (i-oldsz)/PGSIZE, 0);    //不能释放用户态页表物理空间
  return -1;
}
