undefined4 SpuVmPBVoice(short param_1,short param_2,short param_3,short param_4,int param_5)

{
  undefined2 uVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  
  iVar4 = (int)param_1;
  uVar2 = 0;
  if ((((&DAT_8008f852)[iVar4 * 0x1a] == param_2) &&
      (uVar2 = 0, (&DAT_8008f85a)[iVar4 * 0x1a] == param_3)) &&
     (uVar2 = 0, (&DAT_8008f856)[iVar4 * 0x1a] == param_4)) {
    uVar6 = (uint)(ushort)(&DAT_8008f850)[iVar4 * 0x1a];
    uVar3 = (uint)(ushort)(&DAT_8008f858)[iVar4 * 0x1a] + (uint)DAT_800b531b * 0x10;
    iVar4 = (param_5 + 0xffc0) * 0x10000 >> 0x10;
    if (0 < iVar4) {
      iVar4 = iVar4 * (uint)*(byte *)((uVar3 & 0xffff) * 0x20 + DAT_800b2b3c + 0xd);
      iVar5 = iVar4 % 0x3f;
      uVar2 = VMANAGER_OBJ_2440(iVar5,iVar5 * 2,uVar6 + iVar4 / 0x3f);
      return uVar2;
    }
    if (iVar4 < 0) {
      iVar4 = iVar4 * (uint)*(byte *)((uVar3 & 0xffff) * 0x20 + DAT_800b2b3c + 0xc);
      iVar5 = iVar4;
      if (iVar4 < 0) {
        iVar5 = iVar4 + 0x3f;
      }
      uVar2 = VMANAGER_OBJ_2440(iVar4,(iVar4 + (iVar5 >> 6) * -0x40) * 2 + 0x7f,
                                uVar6 + (iVar5 >> 6) + -1);
      return uVar2;
    }
    iVar4 = (int)param_1;
    DAT_800b5320 = *(undefined1 *)(&DAT_8008f858 + iVar4 * 0x1a);
    DAT_800b532e = param_1;
    uVar1 = note2pitch2(uVar6,0);
    (&DAT_8008f6b0)[iVar4 * 8] = uVar1;
    (&DAT_8008f82c)[iVar4] = (&DAT_8008f82c)[iVar4] | 4;
    uVar2 = 1;
  }
  return uVar2;
}
