code * startIntrDMA(void)

{
  memclr(&DAT_80078798,8);
  DMA_DICR = 0;
  InterruptCallback(3,trapIntrDMA);
  return setIntrDMA;
}
