int FUN_80025360(short param_1,short param_2)

{
  short sVar1;
  int iVar2;
  int iVar3;
  
  sVar1 = (ushort)(((int)param_1 & 0x80000000U) == 0) * 0x800 + 0xc00;
  iVar2 = -(int)param_2;
  if (iVar2 != 0) {
    iVar3 = (int)param_1 << 0xc;
    if (iVar2 == 0) {
      trap(0x1c00);
    }
    if ((iVar2 == -1) && (iVar3 == -0x80000000)) {
      trap(0x1800);
    }
    iVar2 = catan(iVar3 / iVar2);
    sVar1 = (short)iVar2;
    if (0 < param_2) {
      sVar1 = sVar1 + 0x800;
    }
  }
  return (int)sVar1;
}
