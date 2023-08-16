// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"
#define  BUCKETSIZE 13
#define  BUFFERSIZE 5
extern uint ticks;
struct {
  struct spinlock lock;
  struct buf buf[BUFFERSIZE];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
} bcache[BUCKETSIZE];

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

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
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

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");
 
  int i=hash(b->blockno);
  acquire(&bcache[i].lock);
  b->refcnt--;
  release(&bcache[i].lock);
  releasesleep(&b->lock);
}

void
bpin(struct buf *b) {
  int i=hash(b->blockno);
  acquire(&bcache[i].lock);
  b->refcnt++;
  release(&bcache[i].lock);
}

void
bunpin(struct buf *b) {
  int i=hash(b->blockno);
  acquire(&bcache[i].lock);
  b->refcnt--;
  release(&bcache[i].lock);
}

int
hash(uint blockno)
{
    return blockno % BUCKETSIZE;
}
