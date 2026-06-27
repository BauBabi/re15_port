/* get_ofs @ 0x8009199c  (Ghidra headless, raw MIPS overlay) */

uint get_ofs(uint param_1,uint param_2)

{
  uint uVar1;
  
  if (1 < DAT_800b2700 - 1) {
    uVar1 = SYS_OBJ_1AF0();
    return uVar1;
  }
  return (param_2 & 0xfff) << 0xc | param_1 & 0xfff | 0xe5000000;
}


