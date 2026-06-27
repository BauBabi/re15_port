void FUN_80045a64(int *param_1)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  short sVar4;
  int iVar5;
  int iVar6;
  long lVar7;
  long lVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  
  iVar12 = DAT_800b0fe4 * 0x20 + *(int *)(DAT_800ac778 + 0x24);
  iVar5 = *(int *)(iVar12 + 4) - *param_1;
  if (iVar5 < 0) {
    iVar5 = -iVar5;
  }
  iVar6 = *(int *)(iVar12 + 0xc) - param_1[2];
  if (iVar6 < 0) {
    iVar6 = -iVar6;
  }
  iVar6 = iVar5 * iVar5 + iVar6 * iVar6;
  iVar5 = *(int *)(iVar12 + 8) - param_1[1];
  if (iVar5 < 0) {
    iVar5 = -iVar5;
  }
  lVar7 = SquareRoot0(iVar6);
  lVar8 = SquareRoot0(iVar6);
  iVar5 = *(int *)(iVar12 + 8) - iVar5;
  uVar9 = SquareRoot0(iVar5 * iVar5 + lVar7 * lVar8);
  uVar9 = uVar9 / 0xfa;
  if (0x7f < uVar9) {
    uVar9 = 0x7f;
  }
  iVar5 = FUN_80045d6c(*(undefined4 *)(iVar12 + 4),*(undefined4 *)(iVar12 + 0xc),*param_1,param_1[2]
                      );
  iVar6 = FUN_80045d6c(*(undefined4 *)(iVar12 + 4),*(undefined4 *)(iVar12 + 0xc),
                       *(undefined4 *)(iVar12 + 0x10),*(undefined4 *)(iVar12 + 0x18));
  iVar10 = FUN_80045d6c(0,0,*(undefined4 *)(iVar12 + 4),*(undefined4 *)(iVar12 + 0xc));
  iVar5 = iVar5 - iVar10;
  iVar12 = FUN_80045d6c(0,0,*(undefined4 *)(iVar12 + 4),*(undefined4 *)(iVar12 + 0xc));
  iVar6 = iVar6 - iVar12;
  if (iVar5 * 0x10000 >> 0x10 < iVar6 * 0x10000 >> 0x10) {
    uVar11 = (iVar5 + 0x1000) - iVar6;
  }
  else {
    uVar11 = iVar5 - iVar6;
  }
  sVar4 = (short)uVar11;
  if (((sVar4 == 0) || (sVar4 == 0x1000)) || (sVar4 == 0x800)) {
    DAT_800b2826 = 0x89;
    DAT_800b2824 = 0x89;
  }
  else {
    bVar1 = (uVar11 & 0x800) == 0;
    bVar2 = !bVar1;
    bVar3 = bVar2;
    if ((uVar11 - 0x401 & 0xffff) < 0x7ff) {
      bVar3 = bVar1;
    }
    sVar4 = (short)((uVar11 & 0x3ff) >> 3);
    if (bVar3) {
      sVar4 = 0x89 - sVar4;
    }
    DAT_800b2826 = (short)bVar2 * (ushort)(byte)(&DAT_80074728)[sVar4] + bVar1 * 0x89;
    DAT_800b2824 = bVar2 * 0x89 + (short)bVar1 * (ushort)(byte)(&DAT_80074728)[sVar4];
  }
  DAT_800b2824 = DAT_800b2824 - (byte)(&UNK_800747a8)[uVar9] & 0x7f;
  DAT_800b2826 = DAT_800b2826 - (byte)(&UNK_800747a8)[uVar9] & 0x7f;
  return;
}
