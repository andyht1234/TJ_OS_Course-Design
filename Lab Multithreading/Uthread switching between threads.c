//thread_create()
t->context.ra = (uint64)func;
t->context.sp = (uint64)&t->stack[STACK_SIZE-1];

//thread_schedule()
thread_switch((uint64)&t->context, (uint64)&next_thread->context);