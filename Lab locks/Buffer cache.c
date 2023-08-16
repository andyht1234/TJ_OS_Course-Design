//binit
void
binit(void)
{
    struct buf *b;
    for(int i=0;i<BUCKETSIZE;i++) 
    {  
      	for(b = bcache[i].buf; b < bcache[i].buf+BUFFERSIZE; b++)
        { 
            initsleeplock(&b->lock, "buffer");
        }
        initlock(&bcache[i].lock, "bcache.bucket");
    }
}

//hash
int
hash(uint blockno)
{
    return blockno % BUCKETSIZE;
}

//bget
static struct buf*
bget(uint dev, uint blockno)
{
    struct buf *b;
    struct buf *min_b=0;
    int i=hash(blockno);
    uint min_time=0xffffffff;     //LRU找最小时间
    acquire(&bcache[i].lock);       //避免竞争
    for(b = bcache[i].buf; b < bcache[i].buf+BUFFERSIZE; b++)
    {  
        if(b->dev==dev && b->blockno == blockno)      //判断是否命中
        {    
            b->refcnt++;
            b->lastuse = ticks;
            release(&bcache[i].lock);
            acquiresleep(&b->lock);
            return b;
        }
        if(b->refcnt==0 && b->lastuse<min_time)     //LRU算法
        {
	        min_time=b->lastuse;
	        min_b=b;
        }
    }
    b=min_b;//进行块替换
    if(b==0)
    {
        panic("bget: no buffers");
    }
    b->dev = dev;
    b->blockno = blockno;
    b->lastuse = ticks;
    b->valid = 0;
    b->refcnt = 1;
    release(&bcache[i].lock);
    acquiresleep(&b->lock);
    return b;
}
