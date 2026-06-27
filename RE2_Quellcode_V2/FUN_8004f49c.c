/* FUN_8004f49c @ 0x8004f49c  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8004f49c(uint *param_1,int param_2,short *param_3)

{
  short sVar1;
  ushort uVar2;
  ushort uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  
  uVar3 = (ushort)param_3[5] >> 4;
  if ((uVar3 & 2) == 0) {
    if ((ushort)param_3[3] - 500 < param_1[0x23] - (param_3[1] + 0xfa)) {
      return 0;
    }
    sVar1 = *param_3;
    uVar7 = param_1[0x21];
    uVar2 = param_3[2];
  }
  else {
    if ((ushort)param_3[2] - 500 < param_1[0x21] - (*param_3 + 0xfa)) {
      return 0;
    }
    sVar1 = param_3[1];
    uVar7 = param_1[0x23];
    uVar2 = param_3[3];
  }
  uVar4 = 0;
  if (uVar7 - (sVar1 + -0x28a) <= uVar2 + 0x514) {
    iVar5 = FUN_80050f80(param_3);
    iVar6 = FUN_80050fc8(param_3);
    *(undefined1 *)((int)param_1 + 5) = 7;
    if ((char)param_1[1] == '\x04') {
      *(undefined1 *)((int)param_1 + 5) = 0x1a;
    }
    *(short *)((int)param_1 + 0x15a) = -1 - (short)((iVar5 - iVar6) / 0x708);
    *(undefined1 *)((int)param_1 + 6) = 0;
    *(undefined1 *)((int)param_1 + 0x16a) = 0;
    if ((uVar3 & 2) == 0) {
      if ((int)(param_1[0x21] - (int)*param_3) < (int)(uint)((ushort)param_3[2] >> 1)) {
        *(undefined2 *)(param_1 + 0x56) = 0;
        if ((uVar3 & 1) != 0) {
          *(undefined1 *)((int)param_1 + 0x16a) = 1;
        }
        uVar7 = *param_3 - param_2;
      }
      else {
        *(undefined2 *)(param_1 + 0x56) = 0x800;
        if ((uVar3 & 1) == 0) {
          *(undefined1 *)((int)param_1 + 0x16a) = 1;
        }
        uVar7 = (int)*param_3 + (uint)(ushort)param_3[2] + param_2;
      }
      param_1[0x21] = uVar7;
    }
    else if ((int)(param_1[0x23] - (int)param_3[1]) < (int)(uint)((ushort)param_3[3] >> 1)) {
      *(undefined2 *)(param_1 + 0x56) = 0xc00;
      if ((uVar3 & 1) != 0) {
        *(undefined1 *)((int)param_1 + 0x16a) = 1;
      }
      param_1[0x23] = param_3[1] - param_2;
    }
    else {
      *(undefined2 *)(param_1 + 0x56) = 0x400;
      if ((uVar3 & 1) == 0) {
        *(undefined1 *)((int)param_1 + 0x16a) = 1;
      }
      param_1[0x23] = (int)param_3[1] + (uint)(ushort)param_3[3] + param_2;
    }
    *param_1 = *param_1 | 8;
    uVar4 = 1;
    DAT_800cfbd8 = DAT_800cfbd8 | 0x40;
  }
  return uVar4;
}


