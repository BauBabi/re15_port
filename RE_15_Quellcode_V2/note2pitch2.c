uint note2pitch2(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  short sVar4;
  
  iVar1 = ((uint)DAT_800b5320 + (uint)DAT_800b531b * 0x10) * 0x20 + DAT_800b2b3c;
  uVar3 = (int)((param_2 & 0xffff) + (uint)*(byte *)(iVar1 + 5)) >> 3;
  sVar4 = (short)uVar3;
  if (0xf < uVar3) {
    sVar4 = sVar4 + -0x10;
  }
  iVar2 = (int)(((uint)(0xf < uVar3) + ((param_1 + 0x3c) - (uint)*(byte *)(iVar1 + 4))) * 0x10000)
          >> 0x10;
  iVar1 = (iVar2 / 0xc + -5) * 0x10000 >> 0x10;
  uVar3 = (uint)*(ushort *)(&DAT_80077520 + (((iVar2 % 0xc) * 0x10000 >> 0xc) + (int)sVar4) * 2);
  if (iVar1 < 1) {
    if (iVar1 < 0) {
      uVar3 = (int)uVar3 >> (-iVar1 & 0x1fU);
    }
    return uVar3 & 0xffff;
  }
  uVar3 = VMANAGER_OBJ_1178();
  return uVar3;
}
