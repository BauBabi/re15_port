/* FUN_8003567c @ 0x8003567c  (Ghidra headless, raw MIPS overlay) */

void FUN_8003567c(uint *param_1,uint param_2)

{
  uint uVar1;
  
  param_1[0x44] = 0;
  if (((*param_1 & 8) == 0) && (param_1[0x79] == 0)) {
    *param_1 = *param_1 & 0xffffbfff;
    uVar1 = FUN_8004c1bc(param_1 + 0x21,(short)param_1[0x24],
                         1 << (*(byte *)((int)param_1 + 0x106) & 0x1f),param_2 | 0x1000000);
    param_1[0x44] = uVar1;
    param_1[0x45] = DAT_800cfaf0;
  }
  *(short *)(param_1 + 0x46) = (short)param_1[0x21];
  *(short *)((int)param_1 + 0x11a) = (short)param_1[0x23];
  return;
}


