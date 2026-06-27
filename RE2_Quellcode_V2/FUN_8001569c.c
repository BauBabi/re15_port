/* FUN_8001569c @ 0x8001569c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001569c(int param_1,uint param_2,int param_3)

{
  short sVar1;
  uint uVar2;
  
  if (param_3 << 0x10 < 0) {
    param_3 = -((param_3 << 0x10) >> 0x10);
    param_2 = param_2 + 0x800 & 0xfff;
  }
  uVar2 = param_3 + (param_2 - *(ushort *)(param_1 + 0x76)) & 0xfff;
  sVar1 = *(ushort *)(param_1 + 0x76) - (short)param_3;
  if ((int)uVar2 < (param_3 << 0x10) >> 0xf) {
    *(short *)(param_1 + 0x76) = (short)param_2;
  }
  else {
    *(short *)(param_1 + 0x76) = sVar1;
    if (uVar2 < 0x801) {
      *(short *)(param_1 + 0x76) = sVar1 + (short)param_3 * 2;
    }
  }
  return;
}


