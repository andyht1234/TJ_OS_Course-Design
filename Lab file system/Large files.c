//bmap
static uint
bmap(struct inode *ip, uint bn)
{
    uint addr, *a;
    struct buf *bp;
    if(bn < NDIRECT){
        if((addr = ip->addrs[bn]) == 0)
        ip->addrs[bn] = addr = balloc(ip->dev);
        return addr;
    }
    bn -= NDIRECT;
    if(bn < NINDIRECT_1)
    {
        if((addr = ip->addrs[NDIRECT]) == 0)
          ip->addrs[NDIRECT] = addr = balloc(ip->dev);
        bp = bread(ip->dev, addr);
        a = (uint*)bp->data;
        if((addr = a[bn]) == 0){
          a[bn] = addr = balloc(ip->dev);
          log_write(bp);
        }
        brelse(bp);
        return addr;
    }
    bn-=NINDIRECT_1;
    if(bn < NINDIRECT_2)
    {
        if((addr = ip->addrs[NDIRECT+1]) == 0)
          ip->addrs[NDIRECT+1] = addr = balloc(ip->dev);
        bp = bread(ip->dev, addr);
        a = (uint*)bp->data;
        if((addr = a[bn/NINDIRECT_1]) == 0){    //level1索引
          a[bn/NINDIRECT_1] = addr = balloc(ip->dev);
          log_write(bp);
        }
        brelse(bp);
        bp = bread(ip->dev, addr);
        a = (uint*)bp->data;
        bn%=NINDIRECT_1;    //level2索引
        if((addr = a[bn]) == 0){
          a[bn] = addr = balloc(ip->dev);
          log_write(bp);
        }
        brelse(bp);
        return addr;
    }
    panic("bmap: out of range");
}

//itrunc
void
itrunc(struct inode *ip)
{
  int i, j,k;
  struct buf *bp,*bp2;
  uint *a,*b;

  for(i = 0; i < NDIRECT; i++){
    if(ip->addrs[i]){
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  if(ip->addrs[NDIRECT]){
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint*)bp->data;
    for(j = 0; j < NINDIRECT_1; j++){
      if(a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  if(ip->addrs[NDIRECT+1])      //二级映射
  {
    bp = bread(ip->dev, ip->addrs[NDIRECT+1]);
    a = (uint*)bp->data;
    for(j = 0; j < NINDIRECT_1; j++)
    {
        if(a[j])
        {
            bp2=bread(ip->dev,a[j]);
            b=(uint*)bp2->data;
            for(k = 0; k < NINDIRECT_1; k++)       //增加一层循环
                if(b[k])
                    bfree(ip->dev, b[k]);
            brelse(bp2);
            bfree(ip->dev,a[j]);
        }
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT+1]);
    ip->addrs[NDIRECT+1] = 0;
  }

  ip->size = 0;
  iupdate(ip);
}
