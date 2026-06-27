undefined2 * stopIntr(void)

{
  undefined2 *puVar1;
  
  puVar1 = (undefined2 *)0x0;
  if (DAT_800776f8 != 0) {
    FUN_8006e170();
    DAT_8007772a = I_MASK;
    DAT_8007772c = DMA_DPCR;
    I_MASK = 0;
    I_STAT = 0;
    DMA_DPCR = DMA_DPCR & 0x77777777;
    ResetEntryInt();
    DAT_800776f8 = 0;
    puVar1 = &DAT_800776f8;
  }
  return puVar1;
}
