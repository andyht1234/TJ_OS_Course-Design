uint64
sys_sigalarm(void)
{
    struct proc* new_proc = myproc();
    int itv;
    uint64 p;
    if(argint(0,&itv)<0)
    {
        return -1;
    }
    if(argaddr(1,&p)<0)
    {
        return -1;
    }
    new_proc->interval=itv;
    new_proc->handler=(void(*)())p;
    new_proc->last_time=0;
    return 0;
}

uint64
sys_sigreturn(void)
{
    struct proc* new1_proc = myproc();
    if(new1_proc->Inalarm)
    {
        new1_proc->Inalarm=0;
        *new1_proc->trapframe = *new1_proc->FrameSave;
        new1_proc->last_time=0;
    }
    return 0;
}

//超时处理test0
if(which_dev==2)
{
    p->last_time++;
    if(p->last_time==p->interval&&p->interval!=0)
    {
        p->trapframe->epc=(uint64)p->handler;
        p->last_time=0;
    }
    yield();
}

//超时处理test1\test2
if(which_dev==2&&p->Inalarm==0)
{
    p->last_time++;
    if(p->Inalarm==0&&p->last_time==p->interval&&p->interval!=0)
    {
        p->Inalarm=1;
        *p->FrameSave = *p->trapframe;
        p->trapframe->epc=(uint64)p->handler;
    }
    yield();
}
