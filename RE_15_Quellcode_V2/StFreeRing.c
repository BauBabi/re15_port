u_long StFreeRing(u_long *base)

{
  int iVar1;
  short *psVar2;
  u_long uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar1 = ((int)base - (DAT_800bee74 * 0x20 + DAT_800bbda4) >> 2) / 0x1f8;
  psVar2 = (short *)(iVar1 * 0x20 + DAT_800bbda4);
  if (*psVar2 == 4) {
    iVar4 = (int)psVar2[3];
    iVar6 = 0;
    iVar5 = iVar1;
    if (0 < iVar4) {
      do {
        iVar6 = iVar6 + 1;
        *(undefined2 *)(iVar5 * 0x20 + DAT_800bbda4) = 0;
        iVar5 = iVar6 + iVar1;
      } while (iVar6 < iVar4);
    }
    DAT_800bb4cc = iVar6 + iVar1;
    return 0;
  }
  uVar3 = C_007_OBJ_B0();
  return uVar3;
}
