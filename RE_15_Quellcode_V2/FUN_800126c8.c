undefined4 FUN_800126c8(short *param_1,short *param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = *(int *)(param_3 + 0x34) - DAT_800aca88;
  iVar4 = *(int *)(param_3 + 0x3c) - DAT_800aca90;
  iVar1 = FUN_80012944((int)DAT_8008f5e4,(int)DAT_8008f5e8,iVar5 - *param_1,iVar4 - param_1[2]);
  uVar2 = 0;
  if (iVar1 < 1) {
    iVar1 = FUN_80012944((int)DAT_8008f5e4,(int)DAT_8008f5e8,iVar5 - *param_2,iVar4 - param_2[2]);
    uVar2 = 0;
    if (-1 < iVar1) {
      iVar1 = FUN_80012944((int)*param_1 - (int)*param_2,(int)param_1[2] - (int)param_2[2],
                           iVar5 - DAT_8008f5e4,iVar4 - DAT_8008f5e8);
      uVar2 = 0;
      if (iVar1 < 1) {
        uVar3 = SquareRoot0(iVar5 * iVar5 + iVar4 * iVar4);
        if (uVar3 < DAT_8008f5e0) {
          uVar2 = 1;
          DAT_8008f5e0 = uVar3;
        }
        else {
          uVar2 = 0;
        }
      }
    }
  }
  return uVar2;
}
