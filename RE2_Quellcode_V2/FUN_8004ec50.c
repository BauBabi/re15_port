/* FUN_8004ec50 @ 0x8004ec50  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8004ec50(int param_1,int param_2,short *param_3)

{
  long lVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  uVar3 = (uint)((ushort)param_3[2] >> 1);
  iVar6 = *(int *)(param_1 + 0x84) - ((int)*param_3 + uVar3);
  iVar5 = *(int *)(param_1 + 0x8c) - ((int)param_3[1] + uVar3);
  lVar1 = SquareRoot0(iVar6 * iVar6 + iVar5 * iVar5);
  iVar4 = (uVar3 + param_2) - lVar1;
  if (iVar4 < 1) {
    uVar2 = 0;
  }
  else {
    iVar6 = (iVar6 + 8) * iVar4;
    if (lVar1 == 0) {
      trap(0x1c00);
    }
    if ((lVar1 == -1) && (iVar6 == -0x80000000)) {
      trap(0x1800);
    }
    iVar4 = (iVar5 + 8) * iVar4;
    if (lVar1 == 0) {
      trap(0x1c00);
    }
    if ((lVar1 == -1) && (iVar4 == -0x80000000)) {
      trap(0x1800);
    }
    uVar2 = 1;
    *(int *)(param_1 + 0x84) = *(int *)(param_1 + 0x84) + iVar6 / lVar1;
    *(int *)(param_1 + 0x8c) = *(int *)(param_1 + 0x8c) + iVar4 / lVar1;
  }
  return uVar2;
}


