/* FUN_80042368 @ 0x80042368  (Ghidra headless, raw MIPS overlay) */

void FUN_80042368(uint *param_1,int param_2,ushort param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  *param_1 = *param_1 & 0x1fffffff;
  iVar3 = (int)*(short *)(&UNK_800a6dcc +
                         (char)(&DAT_800a6e68)[(byte)param_1[2]] * 2 + (short)param_3 * 8) +
          param_1[0xf];
  if (((short)param_3 == 0xd) && ((byte)param_1[2] == 0xe)) {
    iVar3 = (int)DAT_800a6e34 + param_1[0xf];
  }
  iVar1 = (int)((uint)param_3 << 0x10) >> 0xd;
  iVar3 = iVar3 * *(short *)((int)param_1 + 0x1de) >> 0xc;
  if (param_2 < iVar3 + *(short *)(&DAT_800a6dc8 + iVar1)) {
    uVar2 = *param_1;
    uVar4 = 0x80000000;
  }
  else {
    if (iVar3 + *(short *)(&DAT_800a6dca + iVar1) < param_2) {
      uVar2 = *param_1 | 0x20000000;
      goto LAB_80042468;
    }
    uVar2 = *param_1;
    uVar4 = 0x40000000;
  }
  uVar2 = uVar2 | uVar4;
LAB_80042468:
  *param_1 = uVar2;
  return;
}


