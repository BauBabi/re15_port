/* FUN_80034ba8 @ 0x80034ba8  (Ghidra headless, raw MIPS overlay) */

void FUN_80034ba8(void *param_1)

{
  int iVar1;
  
  if (DAT_800dcca0 < 0) {
    FUN_8002c1a0(0x200,0,7,1);
    FUN_8002c2b0(0,0x400,0xffffff,0);
    *(undefined4 *)((int)param_1 + 4) = 0x2c808080;
  }
  else {
    iVar1 = DAT_800dcca0 + -2;
    DAT_800dcca0 = (short)iVar1;
    DAT_800dcca2 = (short)(iVar1 * 0x10000 >> 0x12);
    FUN_8002c1a0(0x200,0,6,1);
    FUN_8002c2b0(0,0x400,0xffffff,0);
    *(short *)((int)param_1 + 8) = 0x36 - DAT_800dcca0;
    *(short *)((int)param_1 + 10) = 0x30 - DAT_800dcca2;
    *(short *)((int)param_1 + 0x10) = DAT_800dcca0 + 0x109;
    *(short *)((int)param_1 + 0x12) = 0x30 - DAT_800dcca2;
    *(short *)((int)param_1 + 0x18) = 0x36 - DAT_800dcca0;
    *(short *)((int)param_1 + 0x1a) = DAT_800dcca2 + 0x57;
    *(short *)((int)param_1 + 0x20) = DAT_800dcca0 + 0x109;
    *(short *)((int)param_1 + 0x22) = DAT_800dcca2 + 0x57;
  }
  AddPrim((void *)(DAT_800cc228 + 0x14),param_1);
  return;
}


