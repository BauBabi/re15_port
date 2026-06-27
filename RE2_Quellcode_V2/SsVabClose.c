/* SsVabClose @ 0x80084ec0  (Ghidra headless, raw MIPS overlay) */

void SsVabClose(short param_1)

{
  int iVar1;
  
  if ((ushort)param_1 < 0x10) {
    iVar1 = (int)param_1;
    if ((&DAT_800dcc68)[iVar1] == '\x01') {
      SpuFree(*(ulong *)(&DAT_800eadf0 + iVar1 * 4));
      (&DAT_800dcc68)[iVar1] = 0;
      DAT_800eade8 = DAT_800eade8 + -1;
    }
  }
  return;
}


