/* FUN_8004c900 @ 0x8004c900  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8004c900(int param_1,undefined4 param_2,short *param_3)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  byte bVar4;
  uint uVar5;
  int iVar6;
  
  uVar5 = (uint)*(ushort *)(param_1 + 0x90) * 2 + (uint)(ushort)param_3[2];
  uVar2 = ((int)*(short *)(param_1 + 0x118) + (uint)*(ushort *)(param_1 + 0x90)) - (int)*param_3;
  uVar3 = ((int)*(short *)(param_1 + 0x11a) + (uint)*(ushort *)(param_1 + 0x92)) - (int)param_3[1];
  bVar4 = (uVar5 <= uVar2) << 1 |
          (uint)*(ushort *)(param_1 + 0x92) * 2 + (uint)(ushort)param_3[3] <= uVar3;
  if ((uVar2 == 0xffffffff) || (uVar2 == uVar5 + 1)) {
    bVar4 = 2;
  }
  if ((uVar3 == 0xffffffff) ||
     (uVar3 == (uint)(ushort)param_3[3] + (uint)*(ushort *)(param_1 + 0x92) * 2 + 1)) {
    bVar4 = 1;
  }
  if (bVar4 == 0) {
    uVar1 = FUN_8004c858(param_1,param_2);
  }
  else {
    if ((bVar4 & 2) != 0) {
      iVar6 = (uint)*(ushort *)(param_1 + 0x90) + (uint)((ushort)param_3[2] >> 1) + 1;
      if (-1 < *(int *)(param_1 + 0x84) - (int)*(short *)(param_1 + 0x118)) {
        iVar6 = -iVar6;
      }
      *(uint *)(param_1 + 0x84) = (int)*param_3 + (uint)((ushort)param_3[2] >> 1) + iVar6;
    }
    if ((bVar4 & 1) != 0) {
      iVar6 = (uint)*(ushort *)(param_1 + 0x92) + (uint)((ushort)param_3[3] >> 1) + 1;
      if (-1 < *(int *)(param_1 + 0x8c) - (int)*(short *)(param_1 + 0x11a)) {
        iVar6 = -iVar6;
      }
      *(uint *)(param_1 + 0x8c) = (int)param_3[1] + (uint)((ushort)param_3[3] >> 1) + iVar6;
    }
    uVar1 = 1;
  }
  return uVar1;
}


