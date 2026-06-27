int FUN_8001a9cc(short *param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  short sVar3;
  
  iVar1 = FUN_8001a6d4((int)*(short *)(DAT_800ac784 + 0x34),(int)*(short *)(DAT_800ac784 + 0x3c),
                       (int)*param_1,(int)param_1[4]);
  uVar2 = param_2 + (iVar1 - (uint)*(ushort *)(DAT_800ac784 + 0x6a)) & 0xfff;
  sVar3 = (short)param_2;
  iVar1 = 0;
  if (((int)sVar3 << 1 <= (int)uVar2) && (iVar1 = (int)sVar3, 0x800 < uVar2)) {
    iVar1 = (int)-sVar3;
  }
  return iVar1;
}
