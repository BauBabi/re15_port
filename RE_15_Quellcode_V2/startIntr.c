undefined2 * startIntr(void)

{
  undefined2 *puVar1;
  int iVar2;
  
  puVar1 = (undefined2 *)0x0;
  if (DAT_800776f8 == 0) {
    I_MASK = 0;
    I_STAT = 0;
    DMA_DPCR = 0x33333333;
    memclr(&DAT_800776f8,0x41a);
    iVar2 = setjmp(&DAT_80077730);
    if (iVar2 != 0) {
      trapIntr();
    }
    DAT_80077734 = &DAT_80078710;
    HookEntryInt(&DAT_80077730);
    DAT_800776f8 = 1;
    DAT_80078774 = startIntrVSync();
    DAT_80078764 = startIntrDMA();
    _96_remove();
    puVar1 = &DAT_800776f8;
    FUN_8006e468();
  }
  return puVar1;
}
