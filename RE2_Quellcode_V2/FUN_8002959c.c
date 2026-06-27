/* FUN_8002959c @ 0x8002959c  (Ghidra headless, raw MIPS overlay) */

void FUN_8002959c(int param_1,undefined4 param_2,int param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(param_3 + (uint)*(byte *)(param_1 + 0x14c) * 4);
  if ((param_4 & 0xffff0000) == 0) {
    uVar1 = (uint)*(byte *)(param_1 + 0x14d);
  }
  else {
    uVar1 = ((uVar2 & 0xffff) - (uint)*(byte *)(param_1 + 0x14d)) - 1;
  }
  *(uint *)(param_1 + 0x178) = param_3 + (uVar2 >> 0x10) + uVar1 * 4;
  FUN_80029614(param_2);
  return;
}


