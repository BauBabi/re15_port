void FUN_80020674(int param_1)

{
  byte bVar1;
  undefined1 uVar2;
  int iVar3;
  
  DAT_800b5577 = DAT_800b5577 + 1 & 0xf;
  if (param_1 == 1) {
    bVar1 = (&DAT_80072e70)[(uint)DAT_800b5577 * 2];
    uVar2 = (&DAT_80072e71)[(uint)DAT_800b5577 * 2];
  }
  else {
    bVar1 = (&DAT_80072e90)[(uint)DAT_800b5577 * 2];
    uVar2 = (&DAT_80072e91)[(uint)DAT_800b5577 * 2];
  }
  if ((DAT_800aca3c & 2) == 0) {
    FUN_800209ec(0,7);
    iVar3 = 0;
    do {
      FUN_80020894(iVar3,-(uint)bVar1,uVar2);
      iVar3 = iVar3 + 1;
    } while (iVar3 != 0xf0);
    FUN_800209b8();
  }
  else {
    FUN_80020794((uint)bVar1,uVar2);
  }
  return;
}
