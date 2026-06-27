/* stopIntr @ 0x80086634  (Ghidra headless, raw MIPS overlay) */

undefined2 * stopIntr(void)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined2 *puVar3;
  
  puVar3 = (undefined2 *)0x0;
  if (DAT_800abc44 != 0) {
    FUN_800957a4();
    puVar2 = PTR_I_MASK_800accd4;
    puVar1 = PTR_I_STAT_800accd0;
    DAT_800abc76 = *(undefined2 *)PTR_I_MASK_800accd4;
    DAT_800abc78 = *(undefined4 *)PTR_DMA_DPCR_800accd8;
    *(undefined2 *)PTR_I_MASK_800accd4 = 0;
    *(undefined2 *)puVar1 = *(undefined2 *)puVar2;
    *(uint *)PTR_DMA_DPCR_800accd8 = *(uint *)PTR_DMA_DPCR_800accd8 & 0x77777777;
    ResetEntryInt();
    DAT_800abc44 = 0;
    puVar3 = &DAT_800abc44;
  }
  return puVar3;
}


