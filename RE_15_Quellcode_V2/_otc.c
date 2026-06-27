dword _otc(int param_1,dword param_2)

{
  int iVar1;
  
  DMA_DPCR = DMA_DPCR | 0x8000000;
  DMA_OTC_MADR = param_1 + param_2 * 4 + -4;
  DMA_OTC_CHCR = 0x11000002;
  DMA_OTC_BCR = param_2;
  set_alarm();
  do {
    if ((DMA_OTC_CHCR & 0x1000000) == 0) {
      return param_2;
    }
    iVar1 = get_alarm();
  } while (iVar1 == 0);
  return 0xffffffff;
}
