/* FUN_80036ef8 @ 0x80036ef8  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80036ef8(uint *param_1,uint *param_2,int param_3)

{
  bool bVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  
  if ((*param_1 & 0x40) != 0) {
    return 0;
  }
  if ((*param_2 & 2) != 0) {
    return 0;
  }
  iVar7 = (uint)*(ushort *)((int)param_1 + 0x9a) + (uint)(ushort)param_2[0x24];
  iVar6 = (uint)*(ushort *)((int)param_1 + 0x9a) + (uint)*(ushort *)((int)param_2 + 0x92);
  iVar9 = param_2[0x21] - param_1[0x21];
  iVar8 = param_2[0x23] - param_1[0x23];
  if ((uint)(iVar7 * 2) < (uint)(iVar9 + iVar7)) {
    return 0;
  }
  if ((uint)(iVar6 * 2) < (uint)(iVar8 + iVar6)) {
    return 0;
  }
  iVar5 = param_2[0x22] - param_1[0x22];
  iVar4 = (uint)*(ushort *)((int)param_1 + 0x9e) + (uint)*(ushort *)((int)param_2 + 0x9e);
  uVar10 = iVar4 * 2;
  bVar1 = (uint)(iVar5 + iVar4) < uVar10;
  if (!bVar1) {
    return 0;
  }
  if (param_3 == 2) {
    return 1;
  }
  *(char *)((int)param_1 + 10) = (char)((int)(param_2 + 0x1ffcbf37) * -0x41041041 >> 3);
  *(char *)((int)param_2 + 0xd) = (char)param_1[3];
  bVar2 = (uint)(((int)(short)param_2[0x13] - (int)(short)param_1[0x13]) + iVar4) < uVar10;
  DAT_800c3b38 = (uint)bVar2;
  if (bVar2 != bVar1 && bVar1) {
    if (param_3 != 0) {
      if (iVar5 < 0) {
        iVar4 = -iVar4;
      }
      param_2[0xf] = (param_1[0x22] - (int)(short)param_2[0x26]) + iVar4;
      param_2[0x22] = param_2[0xf] + (int)(short)param_2[0x26];
      return 1;
    }
    iVar6 = param_2[0x22] - (int)(short)param_1[0x26];
    if (iVar5 < 0) {
      uVar10 = iVar6 + 1 + iVar4;
    }
    else {
      uVar10 = iVar6 - iVar4;
    }
    param_1[0xf] = uVar10;
    param_1[0x22] = param_1[0xf] + (int)(short)param_1[0x26];
    return 1;
  }
  uVar10 = param_2[0x21];
  if ((uint)(iVar7 * 2) < (uVar10 - (int)*(short *)((int)param_1 + 0x4a)) + iVar7) {
    uVar3 = param_1[0x21];
    iVar8 = iVar7 + 1;
    iVar9 = uVar3 - (int)*(short *)((int)param_1 + 0x4a);
    if (param_3 == 0) {
      iVar8 = -iVar8;
      if (iVar9 < 0) {
LAB_800370e0:
        iVar8 = iVar7 + 1;
      }
      iVar6 = (uVar10 + iVar8) - uVar3;
LAB_800370e8:
      param_1[0x21] = param_1[0x21] + iVar6;
      param_1[0xe] = param_1[0xe] + iVar6;
      return 1;
    }
    iVar6 = uVar3 + iVar8;
    if (iVar9 < 0) {
      iVar6 = uVar3 - iVar8;
    }
LAB_800371ac:
    param_2[0x21] = param_2[0x21] + (iVar6 - uVar10);
    param_2[0xe] = param_2[0xe] + (iVar6 - uVar10);
  }
  else {
    uVar3 = param_2[0x23];
    iVar4 = iVar9 * iVar6;
    if ((uint)(iVar6 * 2) < (uVar3 - (int)*(short *)((int)param_1 + 0x4e)) + iVar6) {
      uVar10 = param_1[0x23];
      iVar9 = iVar6 + 1;
      iVar8 = uVar10 - (int)*(short *)((int)param_1 + 0x4e);
      if (param_3 == 0) {
        if (-1 < iVar8) {
          iVar6 = (uVar3 - iVar9) - uVar10;
          goto LAB_80037234;
        }
LAB_8003722c:
        iVar7 = iVar6 + 1;
LAB_80037230:
        iVar6 = (uVar3 + iVar7) - uVar10;
LAB_80037234:
        param_1[0x23] = param_1[0x23] + iVar6;
        param_1[0x10] = param_1[0x10] + iVar6;
        return 1;
      }
      iVar7 = uVar10 + iVar9;
      if (iVar8 < 0) {
        iVar7 = uVar10 - iVar9;
      }
    }
    else {
      iVar5 = iVar8 * iVar7;
      if (iVar4 < 0) {
        iVar4 = -iVar4;
      }
      if (iVar5 < 0) {
        iVar5 = -iVar5;
      }
      if (iVar5 < iVar4) {
        iVar6 = iVar7 + 1;
        if (param_3 == 0) {
          uVar3 = param_1[0x21];
          if (-1 < iVar9) {
            iVar6 = (uVar10 - iVar6) - uVar3;
            goto LAB_800370e8;
          }
          goto LAB_800370e0;
        }
        if (iVar9 < 0) {
          iVar6 = param_1[0x21] - iVar6;
        }
        else {
          iVar6 = param_1[0x21] + iVar6;
        }
        goto LAB_800371ac;
      }
      iVar7 = iVar6 + 1;
      if (param_3 == 0) {
        uVar10 = param_1[0x23];
        iVar7 = -iVar7;
        if (iVar8 < 0) goto LAB_8003722c;
        goto LAB_80037230;
      }
      if (iVar8 < 0) {
        iVar7 = param_1[0x23] - iVar7;
      }
      else {
        iVar7 = param_1[0x23] + iVar7;
      }
    }
    param_2[0x23] = param_2[0x23] + (iVar7 - uVar3);
    param_2[0x10] = param_2[0x10] + (iVar7 - uVar3);
  }
  return 1;
}


