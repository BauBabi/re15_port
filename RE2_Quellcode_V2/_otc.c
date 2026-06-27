/* _otc @ 0x80091b2c  (Ghidra headless, raw MIPS overlay) */

int _otc(int param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  
  *(uint *)PTR_DMA_DPCR_800b27f0 = *(uint *)PTR_DMA_DPCR_800b27f0 | 0x8000000;
  *(undefined4 *)PTR_DMA_OTC_CHCR_800b27ec = 0;
  *(int *)PTR_DMA_OTC_MADR_800b27e4 = param_1 + param_2 * 4 + -4;
  *(int *)PTR_DMA_OTC_BCR_800b27e8 = param_2;
  *(undefined4 *)PTR_DMA_OTC_CHCR_800b27ec = 0x11000002;
  set_alarm();
  uVar1 = *(uint *)PTR_DMA_OTC_CHCR_800b27ec;
  while( true ) {
    if ((uVar1 & 0x1000000) == 0) {
      return param_2;
    }
    iVar2 = get_alarm();
    if (iVar2 != 0) break;
    uVar1 = *(uint *)PTR_DMA_OTC_CHCR_800b27ec;
  }
  return -1;
}


