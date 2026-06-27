/* FUN_8005c970 @ 0x8005c970  (Ghidra headless, raw MIPS overlay) */

void FUN_8005c970(int *param_1,int param_2)

{
  bool bVar1;
  byte bVar2;
  ushort uVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  long lVar10;
  long lVar11;
  int iVar12;
  ushort uVar13;
  int iVar14;
  int local_40;
  int local_3c;
  int local_38;
  
  iVar14 = *(int *)(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20;
  local_40 = *(int *)(iVar14 + 4) - *param_1;
  if (local_40 < 0) {
    local_40 = -local_40;
  }
  local_3c = *(int *)(iVar14 + 8) - param_1[1];
  if (local_3c < 0) {
    local_3c = -local_3c;
  }
  local_38 = *(int *)(iVar14 + 0xc) - param_1[2];
  if (local_38 < 0) {
    local_38 = -local_38;
  }
  lVar10 = SquareRoot0(local_40 * local_40 + local_38 * local_38);
  lVar11 = SquareRoot0(local_40 * local_40 + local_38 * local_38);
  lVar10 = SquareRoot0(local_3c * local_3c + lVar10 * lVar11);
  DAT_800e8988 = lVar10 / 0xfa;
  if (0x7f < DAT_800e8988) {
    DAT_800e8988 = 0x7f;
  }
  sVar6 = FUN_8005cda4(*(undefined4 *)(iVar14 + 4),*(undefined4 *)(iVar14 + 0xc),*param_1,param_1[2]
                      );
  sVar7 = FUN_8005cda4(*(undefined4 *)(iVar14 + 4),*(undefined4 *)(iVar14 + 0xc),
                       *(undefined4 *)(iVar14 + 0x10),*(undefined4 *)(iVar14 + 0x18));
  sVar8 = FUN_8005cda4(0,0,*(undefined4 *)(iVar14 + 4),*(undefined4 *)(iVar14 + 0xc));
  sVar9 = FUN_8005cda4(0,0,*(undefined4 *)(iVar14 + 4),*(undefined4 *)(iVar14 + 0xc));
  sVar4 = (short)((int)sVar6 - (int)sVar8);
  sVar5 = (short)((int)sVar7 - (int)sVar9);
  if ((int)sVar6 - (int)sVar8 < (int)sVar7 - (int)sVar9) {
    DAT_800eae98 = (sVar4 + 0x1000) - sVar5;
  }
  else {
    DAT_800eae98 = sVar4 - sVar5;
  }
  if (((DAT_800eae98 == 0) || (DAT_800eae98 == 0x1000)) || (DAT_800eae98 == 0x800)) {
    DAT_800d7598 = 0x7f;
    DAT_800d759a = 0x7f;
  }
  else {
    bVar1 = (DAT_800eae98 & 0x800) == 0;
    uVar13 = DAT_800eae98 & 0x7ff;
    uVar3 = DAT_800eae98 & 0x400;
    DAT_800eae98 = uVar13;
    if (uVar3 != 0) {
      DAT_800eae98 = 0x800 - uVar13;
    }
    iVar12 = (int)(short)DAT_800eae98;
    if (iVar12 < 0) {
      iVar12 = iVar12 + 7;
    }
    DAT_800eae98 = (ushort)(iVar12 >> 3);
    DAT_800d759a = (ushort)!bVar1 * (ushort)(byte)(&DAT_800a7fb0)[iVar12 >> 3] +
                   (ushort)bVar1 * 0x7f;
    DAT_800d7598 = (ushort)!bVar1 * 0x7f +
                   (ushort)bVar1 * (ushort)(byte)(&DAT_800a7fb0)[iVar12 >> 3];
  }
  DAT_800cbc30 = DAT_800eae98;
  bVar2 = (&UNK_800a8030)[DAT_800e8988];
  if (bVar2 < DAT_800d7598) {
    DAT_800d7598 = DAT_800d7598 - bVar2;
  }
  bVar2 = (&UNK_800a8030)[DAT_800e8988];
  if (bVar2 < DAT_800d759a) {
    DAT_800d759a = DAT_800d759a - bVar2;
  }
  local_40 = *param_1;
  local_3c = param_1[1] + -0x5dc;
  local_38 = param_1[2];
  if ((param_2 != 2) && (iVar14 = FUN_80050858(iVar14 + 4,&local_40,0x8400,1), iVar14 != 0)) {
    DAT_800d7598 = (ushort)(((uint)DAT_800d7598 * 0x41) / 100);
    DAT_800d759a = (ushort)(((uint)DAT_800d759a * 0x41) / 100);
  }
  if (0x7f < DAT_800d7598) {
    DAT_800d7598 = 0x7f;
  }
  if (0x7f < DAT_800d759a) {
    DAT_800d759a = 0x7f;
  }
  return;
}


