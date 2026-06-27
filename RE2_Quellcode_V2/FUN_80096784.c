/* FUN_80096784 @ 0x80096784  (Ghidra headless, raw MIPS overlay) */

void FUN_80096784(void)

{
  undefined **ppuVar1;
  int iVar2;
  
  ppuVar1 = &PTR_LAB_800c3a20 + DAT_800c39e0;
  DAT_800c39e0 = DAT_800c39e0 + 1;
  iVar2 = (*(code *)*ppuVar1)();
  if (iVar2 < 0) {
    (*DAT_800c399c)();
  }
  else {
    if (DAT_800c39e0 != 0) {
      FUN_80098754(0x3c);
      iVar2 = FUN_80096c58();
      if (iVar2 == 0) {
        (*DAT_800c399c)(0xfffffffd);
      }
    }
    if (4 < DAT_800c39e0) {
      DAT_800c39e0 = DAT_800c39e0 + -1;
    }
  }
  return;
}


