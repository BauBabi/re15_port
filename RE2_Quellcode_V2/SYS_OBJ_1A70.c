/* SYS_OBJ_1A70 @ 0x80091950  (Ghidra headless, raw MIPS overlay) */

uint SYS_OBJ_1A70(undefined4 param_1,uint param_2,undefined4 param_3,uint param_4)

{
  uint uVar1;
  
  if (1 < DAT_800b2700 - 1) {
    uVar1 = SYS_OBJ_1AA4();
    return uVar1;
  }
  return (param_2 & 0xfff) << 0xc | param_4 & 0xfff | 0xe4000000;
}


