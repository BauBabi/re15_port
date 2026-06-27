void checkRECT(undefined4 param_1,short *param_2)

{
  int iVar1;
  int iVar2;
  
  if (DAT_8007e352 != '\x01') {
    if (DAT_8007e352 == '\x02') {
      printf("%s:",param_1);
      printf("(%d,%d)-(%d,%d)\n",(int)*param_2,(int)param_2[1],(int)param_2[2],(int)param_2[3]);
      return;
    }
    SYS_OBJ_61C();
    return;
  }
  iVar2 = (int)param_2[2];
  if ((iVar2 <= DAT_8007e354) && (iVar2 + *param_2 <= (int)DAT_8007e354)) {
    iVar1 = (int)param_2[1];
    if (((iVar1 <= DAT_8007e356) &&
        (((iVar1 + param_2[3] <= (int)DAT_8007e356 && (0 < iVar2)) && (-1 < *param_2)))) &&
       ((-1 < iVar1 && (0 < param_2[3])))) {
      return;
    }
  }
  SYS_OBJ_5E0("%s:bad RECT");
  return;
}
