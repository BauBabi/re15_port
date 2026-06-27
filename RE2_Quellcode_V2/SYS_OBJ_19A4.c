/* SYS_OBJ_19A4 @ 0x80091884  (Ghidra headless, raw MIPS overlay) */

uint SYS_OBJ_19A4(undefined4 param_1,uint param_2,undefined4 param_3,uint param_4)

{
  uint uVar1;
  
  if (1 < DAT_800b2700 - 1) {
    uVar1 = SYS_OBJ_19D8();
    return uVar1;
  }
  return (param_2 & 0xfff) << 0xc | param_4 & 0xfff | 0xe3000000;
}


