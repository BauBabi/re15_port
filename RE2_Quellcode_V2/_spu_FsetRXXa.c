/* _spu_FsetRXXa @ 0x80078ff4  (Ghidra headless, raw MIPS overlay) */

uint _spu_FsetRXXa(int param_1,uint param_2)

{
  uint uVar1;
  
  if (DAT_800ab240 != 0) {
    if (DAT_800ab248 == 0) {
      trap(0x1c00);
    }
    if (param_2 % DAT_800ab248 != 0) {
      param_2 = param_2 + DAT_800ab248 & ~DAT_800ab24c;
    }
  }
  if (param_1 != -2) {
    if (param_1 == -1) {
      uVar1 = SPU_OBJ_A30();
      return uVar1;
    }
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_1 * 2) =
         (short)(param_2 >> (DAT_800ab244 & 0x1f));
    return param_2;
  }
  uVar1 = SPU_OBJ_A30();
  return uVar1;
}


