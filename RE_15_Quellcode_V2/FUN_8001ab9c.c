int FUN_8001ab9c(short param_1,short param_2,int param_3)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = FUN_8001a6d4((int)*(short *)(DAT_800ac784 + 0x34),(int)*(short *)(DAT_800ac784 + 0x3c),
                       (int)param_1,(int)param_2);
  uVar3 = param_3 + (iVar2 - (uint)*(ushort *)(DAT_800ac784 + 0x6a)) & 0xfff;
  sVar1 = (short)param_3;
  iVar2 = 0;
  if (((int)sVar1 << 1 <= (int)uVar3) && (iVar2 = (int)sVar1, 0x800 < uVar3)) {
    iVar2 = (int)-sVar1;
  }
  return iVar2;
}
