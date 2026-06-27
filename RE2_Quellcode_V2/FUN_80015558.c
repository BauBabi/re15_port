/* FUN_80015558 @ 0x80015558  (Ghidra headless, raw MIPS overlay) */

void FUN_80015558(int param_1,short param_2,short param_3,int param_4)

{
  uint uVar1;
  short sVar2;
  uint uVar3;
  
  uVar1 = FUN_800154ac((int)*(short *)(param_1 + 0x38),(int)*(short *)(param_1 + 0x40),(int)param_2,
                       (int)param_3);
  if (param_4 << 0x10 < 0) {
    param_4 = -((param_4 << 0x10) >> 0x10);
    uVar1 = uVar1 + 0x800 & 0xfff;
  }
  uVar3 = param_4 + (uVar1 - *(ushort *)(param_1 + 0x76)) & 0xfff;
  if ((int)uVar3 < (param_4 << 0x10) >> 0xf) {
    *(short *)(param_1 + 0x76) = (short)uVar1;
  }
  else {
    sVar2 = *(ushort *)(param_1 + 0x76) - (short)param_4;
    *(short *)(param_1 + 0x76) = sVar2;
    if (uVar3 < 0x801) {
      *(short *)(param_1 + 0x76) = sVar2 + (short)param_4 * 2;
    }
  }
  return;
}


