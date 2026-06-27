/* FUN_80038818 @ 0x80038818  (Ghidra headless, raw MIPS overlay) */

void FUN_80038818(uint *param_1,uint *param_2,undefined4 param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  byte bVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  if (((*param_1 & 0x40) == 0) && ((*param_2 & 2) == 0)) {
    iVar6 = param_2[0x21] - param_1[0x21];
    iVar3 = param_2[0x23] - param_1[0x23];
    iVar7 = -(uint)(ushort)((ushort)param_1[0x24] >> 1);
    iVar5 = iVar7 + (uint)(ushort)param_2[0x24];
    iVar7 = iVar7 + (uint)*(ushort *)((int)param_2 + 0x92);
    bVar4 = (uint)(iVar5 * 2) < (uint)(iVar6 + iVar5);
    if ((uint)(iVar7 * 2) < (uint)(iVar3 + iVar7)) {
      bVar4 = bVar4 | 2;
    }
    if (bVar4 != 0) {
      if ((bVar4 & 1) != 0) {
        iVar1 = iVar6;
        if (iVar6 < 0) {
          iVar1 = -iVar6;
        }
        uVar2 = param_4;
        if ((uint)(iVar1 - iVar5) < param_4) {
          uVar2 = iVar1 - iVar5;
        }
        if (-1 < iVar6) {
          uVar2 = -uVar2;
        }
        param_1[0xe] = param_1[0xe] - uVar2;
        param_1[0x21] = param_1[0x21] - uVar2;
      }
      if ((bVar4 & 2) != 0) {
        iVar5 = iVar3;
        if (iVar3 < 0) {
          iVar5 = -iVar3;
        }
        if ((uint)(iVar5 - iVar7) < param_4) {
          param_4 = iVar5 - iVar7;
        }
        if (-1 < iVar3) {
          param_4 = -param_4;
        }
        param_1[0x10] = param_1[0x10] - param_4;
        param_1[0x23] = param_1[0x23] - param_4;
      }
    }
  }
  return;
}


