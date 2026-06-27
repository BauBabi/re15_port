/* get_mode @ 0x800917ac  (Ghidra headless, raw MIPS overlay) */

uint get_mode(int param_1,int param_2,uint param_3)

{
  uint uVar1;
  
  if (DAT_800b2700 - 1 < 2) {
    uVar1 = 0xe1000000;
    if (param_2 != 0) {
      uVar1 = 0xe1000800;
    }
    param_3 = param_3 & 0x27ff;
    if (param_1 != 0) {
      uVar1 = SYS_OBJ_191C();
      return uVar1;
    }
  }
  else {
    uVar1 = 0xe1000000;
    if (param_2 != 0) {
      uVar1 = 0xe1000200;
    }
    param_3 = param_3 & 0x9ff;
    if (param_1 != 0) {
      param_3 = param_3 | 0x400;
    }
  }
  return uVar1 | param_3;
}


