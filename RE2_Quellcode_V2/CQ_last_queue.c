/* CQ_last_queue @ 0x80089bd4  (Ghidra headless, raw MIPS overlay) */

undefined4 * CQ_last_queue(void)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = (undefined4 *)0x0;
  if (DAT_800c3e80 < 8) {
    iVar2 = DAT_800c3e78 + DAT_800c3e80;
    if (7 < iVar2) {
      iVar2 = iVar2 + -8;
    }
    puVar1 = &DAT_800c3db8 + iVar2 * 6;
  }
  return puVar1;
}


