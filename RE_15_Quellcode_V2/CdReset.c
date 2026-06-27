undefined4 CdReset(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  if (param_1 == 2) {
    CD_initintr();
    uVar1 = SYS_OBJ_98();
    return uVar1;
  }
  iVar2 = CD_init();
  uVar1 = 0;
  if ((iVar2 == 0) && (uVar1 = 1, param_1 == 1)) {
    iVar2 = CD_initvol();
    uVar1 = 0;
    if (iVar2 == 0) {
      uVar1 = 1;
    }
  }
  return uVar1;
}
