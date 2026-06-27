void FUN_80020794(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  DAT_800b5576 = DAT_800b5576 + 2;
  uVar3 = (uint)DAT_800b5576;
  FUN_800209ec(0,7);
  iVar1 = 0;
  do {
    uVar3 = uVar3 + 2 & 0xff;
    iVar2 = iVar1 + 1;
    FUN_80020894(iVar1,-param_1,(int)(char)(&DAT_80072d70)[uVar3] / 0x40 + param_2);
    iVar1 = iVar2;
  } while (iVar2 != 0xf0);
  FUN_800209b8();
  return;
}
