undefined4 restartIntr(void)

{
  undefined4 uVar1;
  
  if (DAT_800776f8 == 0) {
    HookEntryInt(&DAT_80077730);
    DAT_800776f8 = 1;
    I_MASK = DAT_8007772a;
    DMA_DPCR = DAT_8007772c;
    FUN_8006e468();
    uVar1 = INTR_OBJ_6D0();
    return uVar1;
  }
  return 0;
}
