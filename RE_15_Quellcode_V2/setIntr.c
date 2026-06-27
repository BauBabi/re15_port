int setIntr(uint param_1,int param_2)

{
  ushort uVar1;
  int *piVar2;
  word wVar3;
  int iVar4;
  
  wVar3 = I_MASK;
  piVar2 = &DAT_800776fc + param_1;
  iVar4 = *piVar2;
  if ((param_2 != iVar4) && (DAT_800776f8 != 0)) {
    I_MASK = 0;
    if (param_2 != 0) {
      *piVar2 = param_2;
      DAT_80077728 = DAT_80077728 | (ushort)(1 << (param_1 & 0x1f));
      iVar4 = INTR_OBJ_514();
      return iVar4;
    }
    uVar1 = ~(ushort)(1 << (param_1 & 0x1f));
    *piVar2 = 0;
    wVar3 = wVar3 & uVar1;
    DAT_80077728 = DAT_80077728 & uVar1;
    if (param_1 == 0) {
      ChangeClearPAD(1);
      ChangeClearRCnt(3,1);
    }
    if (param_1 == 4) {
      ChangeClearRCnt(0,1);
    }
    if (param_1 == 5) {
      ChangeClearRCnt(1,1);
    }
    if (param_1 == 6) {
      ChangeClearRCnt(2,1);
    }
  }
  I_MASK = wVar3;
  return iVar4;
}
