/* DS_read_cbdata @ 0x8008c534  (Ghidra headless, raw MIPS overlay) */

void DS_read_cbdata(void)

{
  int iVar1;
  undefined4 uVar2;
  
  DAT_800ad308 = DAT_800ad308 + DAT_800ad304 * 4;
  DAT_800ad30c = DAT_800ad30c + -1;
  iVar1 = VSync(-1);
  if (DAT_800ad31c + 0x4b0 < iVar1) {
    DAT_800ad30c = -1;
  }
  if ((DAT_800ad30c == 0) || (iVar1 = VSync(-1), DAT_800ad31c + 0x4b0 < iVar1)) {
    DsReadBreak();
    if (DAT_800ad328 != (code *)0x0) {
      uVar2 = 2;
      if (DAT_800ad30c < 0) {
        uVar2 = 5;
      }
      (*DAT_800ad328)(uVar2,DAT_800ad314);
    }
  }
  return;
}


