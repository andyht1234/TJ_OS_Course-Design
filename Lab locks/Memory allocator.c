void *
steal(int i)
{
    struct run *r;
    for(int j=0;j<NCPU;j++)
    {
        if(j==i)
        continue;
        acquire(&kmem[j].lock);
        r = kmem[j].freelist;
        if(r)
        kmem[j].freelist=r->next;
        release(&kmem[j].lock);
        if(r)
        break;
    }
    return r;
}