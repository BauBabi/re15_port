/* checkRECT @ 0x8009040c  (Ghidra headless, raw MIPS overlay) */

void checkRECT(undefined4 param_1,short *param_2)

{
  int iVar1;
  int iVar2;
  
  if (DAT_800b2702 != '\x01') {
    if (DAT_800b2702 == '\x02') {
      (*(code *)PTR_printf_800b26fc)(&DAT_800125b0,param_1);
      (*(code *)PTR_printf_800b26fc)
                ("(%d,%d)-(%d,%d)\n",(int)*param_2,(int)param_2[1],(int)param_2[2],(int)param_2[3]);
      return;
    }
    SYS_OBJ_640();
    return;
  }
  iVar2 = (int)param_2[2];
  if ((iVar2 <= DAT_800b2704) && (iVar2 + *param_2 <= (int)DAT_800b2704)) {
    iVar1 = (int)param_2[1];
    if (((iVar1 <= DAT_800b2706) &&
        (((iVar1 + param_2[3] <= (int)DAT_800b2706 && (0 < iVar2)) && (-1 < *param_2)))) &&
       ((-1 < iVar1 && (0 < param_2[3])))) {
      return;
    }
  }
  SYS_OBJ_604("%s:bad RECT");
  return;
}


