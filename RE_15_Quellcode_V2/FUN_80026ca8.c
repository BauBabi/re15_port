undefined4 FUN_80026ca8(byte *param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  
  FUN_80027488(DAT_801ff404,&DAT_801ff40c,"BISLPS-00222*");
  FUN_80026c34(param_1);
  uVar5 = 0;
  if (DAT_801ff556 == '\0') {
LAB_80026d74:
    iVar4 = FUN_800275f0(DAT_801ff404);
    uVar3 = 3;
    if (iVar4 == 0) {
      uVar3 = 0;
    }
    else if (iVar4 != 3) {
      uVar3 = FUN_800276b0(DAT_801ff404);
    }
  }
  else {
    iVar4 = 0x1430;
    do {
      bVar1 = *param_1;
      param_1 = param_1 + 1;
      if (bVar1 != 5) {
        iVar2 = FUN_80027368(param_2 + 0x200,&DAT_801ff40c + (uint)bVar1 * 0x16,0x80,0x200);
        if (iVar2 != 0) goto LAB_80026d74;
        FUN_8004ee38(param_2 + iVar4,param_2 + 0x200,0x80);
      }
      uVar5 = uVar5 + 1;
      iVar4 = iVar4 + 0x80;
    } while (uVar5 < 5);
    uVar3 = 0;
  }
  return uVar3;
}
