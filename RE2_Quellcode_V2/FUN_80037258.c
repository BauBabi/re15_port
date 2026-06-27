/* FUN_80037258 @ 0x80037258  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80037258(uint *param_1,uint *param_2)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  
  if (((*param_1 | *param_2) & 0x40) == 0) {
    if ((*param_1 & *param_2 & 0x200000) != 0) {
      return 0;
    }
    iVar8 = param_2[0x21] - param_1[0x21];
    iVar7 = param_2[0x23] - param_1[0x23];
    iVar6 = (uint)(ushort)param_1[0x24] + (uint)(ushort)param_2[0x24];
    iVar5 = (uint)*(ushort *)((int)param_1 + 0x92) + (uint)*(ushort *)((int)param_2 + 0x92);
    if ((uint)(iVar6 * 2) < (uint)(iVar8 + iVar6)) {
      return 0;
    }
    if ((uint)(iVar5 * 2) < (uint)(iVar7 + iVar5)) {
      return 0;
    }
    iVar3 = (uint)*(ushort *)((int)param_1 + 0x9e) + (uint)*(ushort *)((int)param_2 + 0x9e);
    bVar1 = (param_2[0x22] - param_1[0x22]) + iVar3 <= (uint)(iVar3 * 2);
    if (bVar1) {
      iVar9 = iVar8 * iVar5;
      if ((uint)((((int)*(short *)((int)param_2 + 0x46) + (int)(short)param_2[0x26]) -
                 ((int)*(short *)((int)param_1 + 0x46) + (int)(short)param_1[0x26])) + iVar3) <=
          (uint)(iVar3 * 2) == bVar1 || !bVar1) {
        iVar3 = iVar7 * iVar6;
        if (iVar9 < 0) {
          iVar9 = -iVar9;
        }
        if (iVar3 < 0) {
          iVar3 = -iVar3;
        }
        if (iVar3 < iVar9) {
          uVar4 = param_2[0xe];
          if (iVar8 < 0) {
            uVar2 = (param_1[0xe] - 1) - iVar6;
          }
          else {
            uVar2 = param_1[0xe] + 1 + iVar6;
          }
          param_2[0xe] = uVar2;
          param_2[0x21] = param_2[0x21] + (param_2[0xe] - uVar4);
        }
        else {
          uVar4 = param_2[0x10];
          if (iVar7 < 0) {
            uVar2 = (param_1[0x10] - 1) - iVar5;
          }
          else {
            uVar2 = param_1[0x10] + 1 + iVar5;
          }
          param_2[0x10] = uVar2;
          param_2[0x23] = param_2[0x23] + (param_2[0x10] - uVar4);
        }
      }
      else {
        iVar5 = (param_1[0xf] + (int)(short)param_1[0x26]) - (int)(short)param_2[0x26];
        if ((int)(param_2[0x22] - param_1[0x22]) < 0) {
          param_2[0xf] = (iVar5 + -1) - iVar3;
        }
        else {
          param_2[0xf] = iVar5 + 1 + iVar3;
        }
      }
      return 1;
    }
  }
  return 0;
}


