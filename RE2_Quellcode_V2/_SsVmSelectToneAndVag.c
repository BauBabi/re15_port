/* _SsVmSelectToneAndVag @ 0x80084778  (Ghidra headless, raw MIPS overlay) */

byte _SsVmSelectToneAndVag(int param_1,int param_2)

{
  int iVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  
  bVar4 = 0;
  bVar2 = 0;
  bVar3 = bVar4;
  if (DAT_800dcc30 != 0) {
    do {
      iVar1 = ((uint)DAT_800dcc37 * 0x10 + (uint)bVar2) * 0x20 + DAT_800d784c;
      bVar4 = bVar3;
      if ((*(byte *)(iVar1 + 6) <= DAT_800dcc32) && (DAT_800dcc32 <= *(byte *)(iVar1 + 7))) {
        bVar4 = bVar3 + 1;
        *(char *)(param_2 + (uint)bVar3) = (char)*(undefined2 *)(iVar1 + 0x16);
        *(byte *)(param_1 + (uint)bVar3) = bVar2;
      }
      bVar2 = bVar2 + 1;
      bVar3 = bVar4;
    } while (bVar2 < DAT_800dcc30);
  }
  return bVar4;
}


