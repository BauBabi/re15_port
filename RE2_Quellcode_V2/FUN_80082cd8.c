/* FUN_80082cd8 @ 0x80082cd8  (Ghidra headless, raw MIPS overlay) */

int FUN_80082cd8(short param_1,short param_2,short param_3,uint param_4)

{
  uint uVar1;
  byte bVar2;
  int iVar3;
  
  bVar2 = 0;
  iVar3 = 0;
  if (DAT_800d7854 != 0) {
    do {
      uVar1 = (uint)bVar2;
      if (((((int)(short)(&DAT_800cbcde)[uVar1 * 0x1b] == (param_4 & 0xffff)) &&
           ((&DAT_800cbce4)[uVar1 * 0x1b] == param_3)) && ((&DAT_800cbce0)[uVar1 * 0x1b] == param_1)
          ) && ((&DAT_800cbce8)[uVar1 * 0x1b] == param_2)) {
        if ((&DAT_800cbcd0)[uVar1 * 0x1b] == 0xff) {
          iVar3 = iVar3 + 1;
          FUN_80083638();
        }
        else {
          DAT_800dcc4a = (ushort)bVar2;
          FUN_80083690(0);
          iVar3 = iVar3 + 1;
        }
      }
      bVar2 = bVar2 + 1;
    } while (bVar2 < DAT_800d7854);
  }
  return iVar3;
}


