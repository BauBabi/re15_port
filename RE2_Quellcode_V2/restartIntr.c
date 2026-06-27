/* restartIntr @ 0x800866e0  (Ghidra headless, raw MIPS overlay) */

undefined4 restartIntr(void)

{
  undefined4 uVar1;
  
  if (DAT_800abc44 == 0) {
    HookEntryInt(&DAT_800abc7c);
    DAT_800abc44 = 1;
    *(undefined2 *)PTR_I_MASK_800accd4 = DAT_800abc76;
    *(undefined4 *)PTR_DMA_DPCR_800accd8 = DAT_800abc78;
    FUN_800957b4();
    uVar1 = INTR_OBJ_6D0();
    return uVar1;
  }
  return 0;
}


