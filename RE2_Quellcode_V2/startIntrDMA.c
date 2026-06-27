/* startIntrDMA @ 0x800868bc  (Ghidra headless, raw MIPS overlay) */

code * startIntrDMA(void)

{
  memclr(&DAT_800acd0c,8);
  *(undefined4 *)PTR_DMA_DICR_800acd08 = 0;
  InterruptCallback(3,trapIntrDMA);
  return setIntrDMA;
}


