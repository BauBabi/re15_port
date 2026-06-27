uint note2pitch(void)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = (int)(((DAT_800b5316 + 0x3c) - (uint)DAT_800b5324) * 0x10000) >> 0x10;
  uVar2 = (uint)(DAT_800b5325 >> 3);
  if (0xf < uVar2) {
    uVar2 = 0xf;
  }
  uVar2 = (uint)*(ushort *)(&DAT_80077520 + (uVar2 + ((iVar1 % 0xc) * 0x10000 >> 0xc)) * 2);
  iVar1 = (iVar1 / 0xc + -5) * 0x10000 >> 0x10;
  if (0 < iVar1) {
    uVar2 = VMANAGER_OBJ_107C();
    return uVar2;
  }
  if (iVar1 < 0) {
    uVar2 = (int)uVar2 >> (-iVar1 & 0x1fU);
  }
  return uVar2 & 0xffff;
}
