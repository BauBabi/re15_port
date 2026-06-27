/* FUN_8006d550 @ 0x8006d550  (Ghidra headless, raw MIPS overlay) */

void FUN_8006d550(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = 0;
  iVar4 = DAT_800d5c01 * 8;
  iVar2 = 7;
  do {
    if ((&DAT_800d4b68)[iVar4 + iVar2] != -1) {
      iVar3 = iVar3 + 1;
    }
    bVar1 = iVar2 != 0;
    iVar2 = iVar2 + -1;
  } while (bVar1);
  if (iVar3 != 0) {
    if (param_1 == 0) {
      do {
        DAT_800d5c02 = DAT_800d5c02 + 1;
        if ('\a' < (char)DAT_800d5c02) {
          DAT_800d5c02 = 0;
        }
      } while ((&DAT_800d4b68)[iVar4 + (char)DAT_800d5c02] == -1);
    }
    else {
      do {
        DAT_800d5c02 = DAT_800d5c02 - 1;
        if ((int)((uint)DAT_800d5c02 << 0x18) < 0) {
          DAT_800d5c02 = 7;
        }
      } while ((&DAT_800d4b68)[iVar4 + (char)DAT_800d5c02] == -1);
    }
  }
  return;
}


