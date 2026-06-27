undefined4 FUN_8002b5d0(int param_1,int *param_2,uint param_3)

{
  ushort uVar1;
  ushort uVar2;
  short sVar3;
  long lVar4;
  int iVar5;
  int iVar6;
  ushort uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  short *psVar12;
  undefined4 uVar13;
  
  iVar11 = *(int *)(param_1 + 0x78);
  psVar12 = *(short **)(param_1 + 0x7c);
  uVar8 = (uint)*(ushort *)(iVar11 + 6);
  if (*(short *)(iVar11 + 6) == *(short *)(iVar11 + 10)) goto LAB_8002b6fc;
  lVar4 = ratan2(param_2[2] - ((int)psVar12[2] + *(int *)(param_1 + 0x3c)),
                 *param_2 - ((int)*psVar12 + *(int *)(param_1 + 0x34)));
  uVar8 = lVar4 - (uint)*(ushort *)(param_1 + 0x6a);
  uVar7 = (ushort)uVar8 & 0xfff;
  if ((uVar8 & 0xc00) == 0xc00) {
    sVar3 = 0x1000;
LAB_8002b694:
    uVar7 = sVar3 - uVar7;
  }
  else if ((uVar8 & 0x800) == 0) {
    sVar3 = 0x800;
    if ((uVar8 & 0x400) != 0) goto LAB_8002b694;
  }
  else {
    uVar7 = uVar7 - 0x800;
  }
  uVar1 = *(ushort *)(iVar11 + 10);
  uVar10 = (uint)*(ushort *)(iVar11 + 6);
  iVar5 = rsin((int)(short)uVar7);
  uVar8 = uVar10 + ((int)((uVar1 - uVar10) * iVar5) >> 0xc);
  if (uVar1 < uVar10) {
    uVar1 = *(ushort *)(iVar11 + 6);
    uVar2 = *(ushort *)(iVar11 + 10);
    iVar5 = rcos((int)(short)uVar7);
    uVar8 = (uint)uVar2 + ((int)(((uint)uVar1 - (uint)uVar2) * iVar5) >> 0xc);
  }
LAB_8002b6fc:
  iVar9 = (uVar8 & 0xffff) + (param_3 & 0xffff);
  iVar5 = *param_2 - ((int)*psVar12 + *(int *)(param_1 + 0x34));
  uVar13 = 0;
  if ((uint)(iVar5 + iVar9) <= (uint)(iVar9 * 2)) {
    iVar6 = param_2[2] - ((int)psVar12[2] + *(int *)(param_1 + 0x3c));
    uVar13 = 0;
    if ((uint)(iVar6 + iVar9) <= (uint)(iVar9 * 2)) {
      lVar4 = SquareRoot0(iVar5 * iVar5 + iVar6 * iVar6);
      uVar13 = 0;
      if (((0 < iVar9 - lVar4) &&
          (iVar5 = (param_3 & 0xffff) + (uint)*(ushort *)(iVar11 + 8),
          iVar11 = param_2[1] - ((int)psVar12[1] + *(int *)(param_1 + 0x38)), uVar13 = 0,
          -iVar5 < iVar11)) && (uVar13 = 0, iVar11 < iVar5)) {
        uVar13 = 1;
      }
    }
  }
  return uVar13;
}
