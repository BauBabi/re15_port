int get_dx(short *param_1)

{
  int iVar1;
  
  if (DAT_8007e350 == '\x01') {
    if (DAT_8007e353 != '\0') {
      iVar1 = SYS_OBJ_1904((int)*param_1);
      return iVar1;
    }
    return (int)*param_1;
  }
  if (DAT_8007e350 != '\x02') {
    iVar1 = SYS_OBJ_1900();
    return iVar1;
  }
  if (DAT_8007e353 != '\0') {
    iVar1 = SYS_OBJ_189C((int)*param_1);
    return iVar1;
  }
  iVar1 = SYS_OBJ_1904();
  return iVar1;
}
