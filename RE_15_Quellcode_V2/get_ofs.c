uint get_ofs(uint param_1,uint param_2)

{
  uint uVar1;
  
  if (1 < DAT_8007e350 - 1) {
    uVar1 = SYS_OBJ_17BC();
    return uVar1;
  }
  return (param_2 & 0xfff) << 0xc | param_1 & 0xfff | 0xe5000000;
}
