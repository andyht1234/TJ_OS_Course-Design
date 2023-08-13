static void 
barrier()
{
  pthread_mutex_lock(&bstate.barrier_mutex);    //拦住先到达的进程
  bstate.nthread++;     //点进程数+1
  if (bstate.nthread < nthread)     //判断是否所有进程都到了
  {
    pthread_cond_wait(&bstate.barrier_cond, &bstate.barrier_mutex);     //未全到，等待
  } 
  else  //都到了唤醒，开启新一轮
  {
    bstate.nthread = 0;
    bstate.round++;
    pthread_cond_broadcast(&bstate.barrier_cond);
  }
  pthread_mutex_unlock(&bstate.barrier_mutex); 
}
