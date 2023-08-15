int
e1000_transmit(struct mbuf *m)
{
    acquire(&e1000_lock);       //上锁
    int idx=regs[E1000_TDT];    //读取E1000_TDT控制寄存器
    if((tx_ring[idx].status&E1000_TXD_STAT_DD)==0)      //TX环索引,检查环是否溢出
    {
        release(&e1000_lock);
        return -1;
    }
    if(tx_mbufs[idx])
        mbuffree(tx_mbufs[idx]);    //释放从该描述符传输的最后一个mbuf
    tx_mbufs[idx] = m;      //描述符
    tx_ring[idx].length = m->len;   
    tx_ring[idx].addr = (uint64)m->head;
    tx_ring[idx].cmd = E1000_TXD_CMD_RS | E1000_TXD_CMD_EOP;
    regs[E1000_TDT] = (idx + 1) % TX_RING_SIZE;     //更新环位置
    release(&e1000_lock);
    return 0;
}

static void
e1000_recv(void)
{
  int idx = regs[E1000_RDT];
  idx = (idx + 1) % RX_RING_SIZE;
  while(rx_ring[idx].status & E1000_RXD_STAT_DD)        //等待接收数据包（如果有）所在的环索引,检查新数据包是否可用
  {
    rx_mbufs[idx]->len = rx_ring[idx].length;
    net_rx(rx_mbufs[idx]);
    rx_mbufs[idx] = mbufalloc(0);       //分配一个新的mbuf
    rx_ring[idx].addr = (uint64)rx_mbufs[idx]->head;
    rx_ring[idx].status = 0;
    idx = (idx + 1) % RX_RING_SIZE;
  }
  regs[E1000_RDT] = (idx - 1) % RX_RING_SIZE;       //更新为最后处理的环描述符的索引
}