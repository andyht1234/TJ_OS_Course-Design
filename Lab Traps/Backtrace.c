void backtrace()
{
   uint64 fp=r_fp();
   uint64 up=PGROUNDUP(fp),down=PGROUNDDOWN(fp);
   printf("backtrace:\n");
   while(fp>=down && fp<up)
   {	
      printf("%p\n",*((uint64*)(fp-8)));
      fp=*((uint64*)(fp-16));
   }
}