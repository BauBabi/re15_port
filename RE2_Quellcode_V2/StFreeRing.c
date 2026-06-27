/* StFreeRing @ 0x80088c8c  (Ghidra headless, raw MIPS overlay) */

u_long StFreeRing(u_long *base)

{
  int iVar1;
  u_long uVar2;
  int iVar3;
  int iVar4;
  short *psVar5;
  int iVar6;
  
  iVar1 = ((int)base - (DAT_800ead9c + DAT_800eae9c * 0x20) >> 2) / 0x1f8;
  psVar5 = (short *)(DAT_800ead9c + iVar1 * 0x20);
  if (*psVar5 == 4) {
    iVar3 = (int)psVar5[3];
    iVar6 = 0;
    if (0 < iVar3) {
      do {
        iVar4 = iVar6 + iVar1;
        iVar6 = iVar6 + 1;
        *(undefined2 *)(DAT_800ead9c + iVar4 * 0x20) = 0;
      } while (iVar6 < iVar3);
    }
    DAT_800ea234 = iVar6 + iVar1;
    return 0;
  }
  uVar2 = C_007_OBJ_AC();
  return uVar2;
}


