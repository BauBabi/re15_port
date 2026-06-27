/* get_dx @ 0x80091a64  (Ghidra headless, raw MIPS overlay) */

int get_dx(short *param_1)

{
  int iVar1;
  
  if (DAT_800b2700 == '\x01') {
    if (DAT_800b2703 != '\0') {
      iVar1 = SYS_OBJ_1C2C((int)*param_1);
      return iVar1;
    }
    return (int)*param_1;
  }
  if (DAT_800b2700 != '\x02') {
    iVar1 = SYS_OBJ_1C28();
    return iVar1;
  }
  if (DAT_800b2703 != '\0') {
    iVar1 = SYS_OBJ_1BC4((int)*param_1);
    return iVar1;
  }
  iVar1 = SYS_OBJ_1C2C();
  return iVar1;
}


