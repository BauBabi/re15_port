undefined4 FUN_8002cfd4(uint *param_1,uint *param_2)

{
  int iVar1;
  byte bVar2;
  uint uVar3;
  byte bVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  if (((*param_1 & 0x40) == 0) && ((*param_2 & 2) == 0)) {
    uVar6 = param_2[0xd];
    iVar8 = (int)*(short *)param_1[0x1f];
    iVar10 = uVar6 - (iVar8 + param_1[0xd]);
    iVar7 = (int)((short *)param_1[0x1f])[2];
    uVar3 = (uint)(*(ushort *)(param_1[0x1e] + 6) >> 1);
    iVar5 = *(ushort *)(param_2[0x1e] + 6) - uVar3;
    iVar1 = *(ushort *)(param_2[0x1e] + 10) - uVar3;
    iVar9 = param_2[0xf] - (iVar7 + param_1[0xf]);
    bVar4 = (uint)(iVar5 * 2) < (uint)(iVar10 + iVar5);
    if ((uint)(iVar1 * 2) < (uint)(iVar9 + iVar1)) {
      bVar4 = bVar4 | 2;
    }
    if (bVar4 != 0) {
      bVar2 = (uint)(iVar5 * 2) < (uVar6 - ((short)param_1[0x10] + iVar8)) + iVar5;
      if ((uint)(iVar1 * 2) < (param_2[0xf] - ((short)param_1[0x11] + iVar7)) + iVar1) {
        bVar2 = bVar2 | 2;
      }
      bVar4 = (bVar2 ^ bVar4) & bVar4;
      if ((bVar4 & 1) != 0) {
        iVar7 = -iVar5;
        if (iVar10 < 0) {
          iVar7 = iVar5;
        }
        param_1[0xd] = uVar6 + iVar7;
      }
      if ((bVar4 & 2) != 0) {
        if (-1 < iVar9) {
          iVar1 = -iVar1;
        }
        param_1[0xf] = param_2[0xf] + iVar1;
      }
    }
  }
  return 0;
}
