void 
vmprint(pagetable_t pagetable) 
{
  printf("page table %p\n", pagetable);
  vmprint_tree(pagetable, 1);
}

void
vmprint_tree(pagetable_t pagetable,int depth)
{
    char *dep[]={"","..",".. ..",".. .. .."};
    for(int i=0;i<512;i++)
    {
        pte_t pte=pagetable[i];
        if(pte&PTE_V)
        {
            printf("%s%d: pte %p pa %p\n",dep[depth],i,pte,PTE2PA(pte));
            if((pte&(PTE_R|PTE_W|PTE_X))==0)
            {
                uint64 child=PTE2PA(pte);
                vmprint_tree((pagetable_t)child,depth+1);
            }
        }
    }
}